//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTableParser.cc
//---------------------------------------------------------------------------//

// C++
#include <fstream>

// Root
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TBranch.h>

// Geant4
#include <G4VProcess.hh>
#include <G4ProcessManager.hh>
#include <G4VEmProcess.hh>
#include <G4VEnergyLossProcess.hh>
#include <G4VMultipleScattering.hh>
#include <G4eplusAnnihilation.hh>

// Celeritas
#include "io/GeantPhysicsTable.hh"
#include "io/GeantPhysicsVector.hh"
// Celeritas enums
#include "io/GeantTableType.hh"
#include "io/GeantProcessType.hh"
#include "io/GeantProcess.hh"
#include "io/GeantPhysicsVectorType.hh"
#include "io/GeantModel.hh"

#include "GeantPhysicsTableParser.hh"

//---------------------------------------------------------------------------//
/*!
 * Construct with existing TFile reference
 */
GeantPhysicsTableParser::GeantPhysicsTableParser(TFile* root_file)
{
    REQUIRE(root_file);
    this->tree_tables_ = std::make_unique<TTree>("tables", "tables");
    tree_tables_->Branch("GeantPhysicsTable", &(this->table_));
}

//---------------------------------------------------------------------------//
/*!
 * Add a new physics table as a new TTree by providing process and particle
 */
void GeantPhysicsTableParser::add_physics_table(G4VProcess*           process,
                                                G4ParticleDefinition* particle)
{
    // Particle name
    std::string particle_name = particle->GetParticleName();
    replace_characters(particle_name, "+", "Plus");
    replace_characters(particle_name, "-", "Minus");

    // Process type
    G4ProcessType process_type    = process->GetProcessType();
    std::string process_type_name = process->GetProcessTypeName(process_type);

    // Process
    std::string  process_name = process->GetProcessName();
    GeantProcess process_enum(process_name);

    // Write table_
    this->table_.process_type = (GeantProcessType)process_type;
    this->table_.process      = process_enum;
    this->table_.particle     = PDGNumber(particle->GetPDGEncoding());

    auto em_process          = dynamic_cast<G4VEmProcess*>(process);
    auto energy_loss         = dynamic_cast<G4VEnergyLossProcess*>(process);
    auto multiple_scattering = dynamic_cast<G4VMultipleScattering*>(process);

    //-----------------------------------------------------------------------//
    // G4VEmProcess tables
    if (em_process)
    {
        for (ssize_t i = 0; i < em_process->GetNumberOfModels(); i++)
        {
            G4PhysicsTable* table = nullptr;

            // Model
            std::string model_name = em_process->GetModelByIndex(i)->GetName();
            GeantModel  model(model_name);

            // Write table_
            this->table_.model = model;

            // A same model can have both Lambda and LambdaPrim tables
            if (em_process->LambdaTable())
            {
                table = em_process->LambdaTable();

                std::string    table_type_name = "Lambda";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (em_process->LambdaTablePrim())
            {
                table = em_process->LambdaTablePrim();

                std::string    table_type_name = "LambdaPrim";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (table == nullptr)
            {
                std::cout << "No available table in "
                             "GeantPhysicsTableParser::addPhysicsTableTree "
                             "for G4VEmProcess "
                          << process_name << "." << particle_name << std::endl;
            }
        }
    }

    //-----------------------------------------------------------------------//
    // G4VEnergyLossProcess tables
    else if (energy_loss)
    {
        for (ssize_t i = 0; i < energy_loss->NumberOfModels(); i++)
        {
            G4PhysicsTable* table = nullptr;

            // Model
            std::string model_name = energy_loss->GetModelByIndex(i)->GetName();
            GeantModel  model(model_name);

            // Write table_
            this->table_.model = model;

            if (energy_loss->DEDXTable())
            {
                table = energy_loss->DEDXTable();

                std::string    table_type_name = "DEDX";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->DEDXTableForSubsec())
            {
                table = energy_loss->DEDXTableForSubsec();

                std::string    table_type_name = "SubDEDX";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->DEDXunRestrictedTable())
            {
                table = energy_loss->DEDXunRestrictedTable();

                std::string    table_type_name = "DEDXnr";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->IonisationTable())
            {
                table = energy_loss->IonisationTable();

                std::string    table_type_name = "Ionisation";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->IonisationTableForSubsec())
            {
                table = energy_loss->IonisationTableForSubsec();

                std::string    table_type_name = "SubIonisation";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->CSDARangeTable())
            {
                table = energy_loss->CSDARangeTable();

                std::string    table_type_name = "CSDARange";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->SecondaryRangeTable())
            {
                table = energy_loss->SecondaryRangeTable();

                std::string    table_type_name = "RangeSec";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->RangeTableForLoss())
            {
                table = energy_loss->RangeTableForLoss();

                std::string    table_type_name = "Range";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->InverseRangeTable())
            {
                table = energy_loss->InverseRangeTable();

                std::string    table_type_name = "InverseRange";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->LambdaTable())
            {
                table = energy_loss->LambdaTable();

                std::string    table_type_name = "Lambda";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (energy_loss->SubLambdaTable())
            {
                table = energy_loss->SubLambdaTable();

                std::string    table_type_name = "SubLambda";
                GeantTableType table_type(table_type_name);
                this->table_.table_type = table_type;

                std::string tree_name = table_type_name + "_" + process_name
                                        + "_" + model_name + "_"
                                        + particle_name;

                write_tree(tree_name, table);
            }

            if (table == nullptr)
            {
                std::cout << "No available table in "
                             "GeantPhysicsTableParser::addPhysicsTableTree "
                             "for G4VEnergyLossProcess "
                          << process_name << "." << particle_name << std::endl;
            }
        }
    }

    //-----------------------------------------------------------------------//
    // G4VMultipleScattering tables
    else if (multiple_scattering)
    {
        // TODO: Figure out a call to get the number of models. Max is 4.
        for (ssize_t i = 0; i < 4; i++)
        {
            G4VEmModel* model = multiple_scattering->GetModelByIndex(i);

            if (model)
            {
                G4PhysicsTable* table = model->GetCrossSectionTable();

                if (table)
                {
                    std::string model_name      = model->GetName();
                    std::string table_type_name = "LambdaMod"
                                                  + std::to_string(i + 1);
                    GeantTableType table_type(table_type_name);
                    this->table_.table_type = table_type;

                    std::string tree_name = table_type_name + "_"
                                            + process_name + "_" + model_name
                                            + "_" + particle_name;

                    write_tree(tree_name, table);
                }
            }
        }
    }

    //-----------------------------------------------------------------------//
    else
    {
        std::cout << "No available code for " << particle_name << "."
                  << process_name << std::endl;
    }
}

//---------------------------------------------------------------------------//
/*!
 * Write a given physics table as a tree to this->root_file_
 */
void GeantPhysicsTableParser::write_tree(std::string     tree_name,
                                         G4PhysicsTable* table)
{
    // Replace special characters
    replace_characters(tree_name, ".", "_");
    replace_characters(tree_name, "-", "_");

    this->table_.physics_vectors.clear();

    // G4PhysicsTable is a std::vector<G4PhysicsVector>
    for (auto phys_vector : *table)
    {
        GeantPhysicsVector geant_physics_vector;

        geant_physics_vector.vector_type
            = (GeantPhysicsVectorType)phys_vector->GetType();

        for (ssize_t j = 0; j < phys_vector->GetVectorLength(); j++)
        {
            geant_physics_vector.energy.push_back(phys_vector->Energy(j));
            geant_physics_vector.xs_eloss.push_back((*phys_vector)[j]);
        }

        this->table_.physics_vectors.push_back(geant_physics_vector);
    }

    this->tree_tables_->Fill();

    std::cout << "  Added " << tree_name << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Replace characters in a string
 */
void GeantPhysicsTableParser::replace_characters(std::string&      string,
                                                 std::string const search,
                                                 std::string const replace)
{
    size_t position = 0;

    while ((position = string.find(search, position)) != std::string::npos)
    {
        string.replace(position, search.length(), replace);
        position += replace.length();
    }
}