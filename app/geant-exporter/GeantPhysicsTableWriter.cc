//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTableWriter.cc
//---------------------------------------------------------------------------//

// C++
#include <fstream>

// ROOT
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

// Geant4
#include <G4VProcess.hh>
#include <G4VEmProcess.hh>
#include <G4VEnergyLossProcess.hh>
#include <G4VMultipleScattering.hh>

// Celeritas
#include "io/GeantPhysicsTable.hh"
#include "io/GeantPhysicsVector.hh"
// Celeritas enums
#include "io/GeantTableType.hh"
#include "io/GeantProcessType.hh"
#include "io/GeantProcess.hh"
#include "io/GeantPhysicsVectorType.hh"
#include "io/GeantModel.hh"

#include "GeantPhysicsTableWriter.hh"

//---------------------------------------------------------------------------//
/*!
 * Construct with existing TFile reference
 */
GeantPhysicsTableWriter::GeantPhysicsTableWriter(TFile* root_file)
{
    REQUIRE(root_file);
    this->tree_tables_ = std::make_unique<TTree>("tables", "tables");
    tree_tables_->Branch("GeantPhysicsTable", &(this->table_));
}

//---------------------------------------------------------------------------//
/*!
 * Add physics tables to the ROOT file from given process and particle
 */
void GeantPhysicsTableWriter::add_physics_tables(G4VProcess*           process,
                                                 G4ParticleDefinition* particle)
{
    // Process type
    G4ProcessType process_type    = process->GetProcessType();
    std::string process_type_name = process->GetProcessTypeName(process_type);

    // Process
    std::string  process_name = process->GetProcessName();
    GeantProcess process_enum(process_name);

    // Write this->table_
    this->table_.process_type = (GeantProcessType)process_type;
    this->table_.process      = process_enum;
    this->table_.particle     = PDGNumber(particle->GetPDGEncoding());

    // Just doing if (auto em_process = dynamic_cast...) does not work...
    // ... not sure why yet. Will revisit.
    auto em_process          = dynamic_cast<G4VEmProcess*>(process);
    auto energy_loss         = dynamic_cast<G4VEnergyLossProcess*>(process);
    auto multiple_scattering = dynamic_cast<G4VMultipleScattering*>(process);

    // G4VEmProcess tables
    if (em_process)
        fill_em_tables(em_process);

    // G4VEnergyLossProcess tables
    else if (energy_loss)
        fill_energy_loss_tables(energy_loss);

    // G4VMultipleScattering tables
    else if (multiple_scattering)
        fill_multiple_scattering_tables(multiple_scattering);

    else
        std::cout << "  No available code for " << process_name << std::endl;
}

//----------------------------------------------------------------------------//
/*!
 * Write EM process tables to the TTree
 */
void GeantPhysicsTableWriter::fill_em_tables(G4VEmProcess* em_process)
{
    std::string process_name = em_process->GetProcessName();

    for (ssize_t i = 0; i < em_process->GetNumberOfModels(); i++)
    {
        G4PhysicsTable* table = nullptr;

        // Model
        std::string model_name = em_process->GetModelByIndex(i)->GetName();
        GeantModel  model(model_name);

        // Write table_
        this->table_.model = model;

        // The same model can have both Lambda and LambdaPrim tables, so no
        // else if (...) is used
        if (em_process->LambdaTable())
        {
            table = em_process->LambdaTable();

            // Table type
            std::string    table_type_name = "Lambda";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            // Populate this->table_.physics_vectors and fill the TTree
            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            // Print added table
            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (em_process->LambdaTablePrim())
        {
            table = em_process->LambdaTablePrim();

            std::string    table_type_name = "LambdaPrim";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (table == nullptr)
        {
            std::cout << "  No available table in "
                         "GeantPhysicsTableWriter::add_physics_tables "
                         "for G4VEmProcess "
                      << process_name << std::endl;
        }
    }
}

//----------------------------------------------------------------------------//
/*!
 * Write energy loss tables to the TTree
 */
void GeantPhysicsTableWriter::fill_energy_loss_tables(
    G4VEnergyLossProcess* eloss_process)
{
    std::string process_name = eloss_process->GetProcessName();

    for (ssize_t i = 0; i < eloss_process->NumberOfModels(); i++)
    {
        G4PhysicsTable* table = nullptr;

        // Model
        std::string model_name = eloss_process->GetModelByIndex(i)->GetName();
        GeantModel  model(model_name);

        // Write table_
        this->table_.model = model;

        if (eloss_process->DEDXTable())
        {
            table = eloss_process->DEDXTable();

            // Table type
            std::string    table_type_name = "DEDX";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            // Populate this->table_.physics_vectors and fill the TTree
            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            // Print added table
            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->DEDXTableForSubsec())
        {
            table = eloss_process->DEDXTableForSubsec();

            std::string    table_type_name = "SubDEDX";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->DEDXunRestrictedTable())
        {
            table = eloss_process->DEDXunRestrictedTable();

            std::string    table_type_name = "DEDXnr";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->IonisationTable())
        {
            table = eloss_process->IonisationTable();

            std::string    table_type_name = "Ionisation";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->IonisationTableForSubsec())
        {
            table = eloss_process->IonisationTableForSubsec();

            std::string    table_type_name = "SubIonisation";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->CSDARangeTable())
        {
            table = eloss_process->CSDARangeTable();

            std::string    table_type_name = "CSDARange";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->SecondaryRangeTable())
        {
            table = eloss_process->SecondaryRangeTable();

            std::string    table_type_name = "RangeSec";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->RangeTableForLoss())
        {
            table = eloss_process->RangeTableForLoss();

            std::string    table_type_name = "Range";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->InverseRangeTable())
        {
            table = eloss_process->InverseRangeTable();

            std::string    table_type_name = "InverseRange";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->LambdaTable())
        {
            table = eloss_process->LambdaTable();

            std::string    table_type_name = "Lambda";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (eloss_process->SubLambdaTable())
        {
            table = eloss_process->SubLambdaTable();

            std::string    table_type_name = "SubLambda";
            GeantTableType table_type(table_type_name);
            this->table_.table_type = table_type;

            fill_physics_vectors(table);
            this->tree_tables_->Fill();

            std::string table_name = table_type_name + "_" + process_name + "_"
                                     + model_name;
            std::cout << "  Added " << table_name << std::endl;
        }

        if (table == nullptr)
        {
            std::cout << "  No available table in "
                         "GeantPhysicsTableWriter::add_physics_tables "
                         "for G4VEnergyLossProcess "
                      << process_name << std::endl;
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Write multiple scattering tables to the TTree
 */
void GeantPhysicsTableWriter::fill_multiple_scattering_tables(
    G4VMultipleScattering* msc_process)
{
    std::string process_name = msc_process->GetProcessName();

    // TODO: Figure out a method to get the number of models. Max is 4.
    // Other classes have a NumberOfModels(), but not G4VMultipleScattering
    for (ssize_t i = 0; i < 4; i++)
    {
        G4VEmModel* model = msc_process->GetModelByIndex(i);

        if (model)
        {
            G4PhysicsTable* table = model->GetCrossSectionTable();

            if (table)
            {
                // Table type
                std::string model_name      = model->GetName();
                std::string table_type_name = "LambdaMod"
                                              + std::to_string(i + 1);
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                // Populate this->table_.physics_vectors and fill the TTree
                fill_physics_vectors(table);
                this->tree_tables_->Fill();

                // Print added table
                std::string table_name = table_type_name + "_" + process_name
                                         + "_" + model_name;
                std::cout << "  Added " << table_name << std::endl;
            }
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Write a given G4PhysicsTable as a GeantPhysicsVector in this->table_
 */
void GeantPhysicsTableWriter::fill_physics_vectors(G4PhysicsTable* table)
{
    // Clean up so previous vector data is not carried forward
    this->table_.physics_vectors.clear();

    // Loop over G4PhysicsTable
    for (auto phys_vector : *table)
    {
        GeantPhysicsVector geant_physics_vector;

        // Populate GeantPhysicsVector and push it back to this->table_
        geant_physics_vector.vector_type
            = (GeantPhysicsVectorType)phys_vector->GetType();

        for (ssize_t j = 0; j < phys_vector->GetVectorLength(); j++)
        {
            geant_physics_vector.energy.push_back(phys_vector->Energy(j));
            geant_physics_vector.xs_eloss.push_back((*phys_vector)[j]);
        }
        this->table_.physics_vectors.push_back(geant_physics_vector);
    }
}
