//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTableWriter.cc
//---------------------------------------------------------------------------//
#include "GeantPhysicsTableWriter.hh"

#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include <G4VProcess.hh>
#include <G4VEmProcess.hh>
#include <G4VEnergyLossProcess.hh>
#include <G4VMultipleScattering.hh>
#include <G4SystemOfUnits.hh>

#include "io/GeantPhysicsTable.hh"
#include "io/GeantPhysicsVector.hh"
#include "io/GeantTableType.hh"
#include "io/GeantTableTypeHelper.hh"
#include "io/GeantProcessType.hh"
#include "io/GeantProcess.hh"
#include "io/GeantProcessHelper.hh"
#include "io/GeantPhysicsVectorType.hh"
#include "io/GeantModel.hh"
#include "io/GeantModelHelper.hh"
#include "base/Range.hh"
#include "base/Types.hh"

using celeritas::GeantModel;
using celeritas::GeantPhysicsVector;
using celeritas::GeantPhysicsVectorType;
using celeritas::GeantProcess;
using celeritas::GeantProcessType;
using celeritas::GeantTableType;
using celeritas::PDGNumber;
using celeritas::real_type;
using celeritas::select_geant_model;
using celeritas::select_geant_process;
using celeritas::select_geant_table_type;

//---------------------------------------------------------------------------//
/*!
 * Construct with existing TFile reference
 */
GeantPhysicsTableWriter::GeantPhysicsTableWriter(TFile* root_file)
{
    REQUIRE(root_file);
    this->tree_tables_ = std::make_unique<TTree>("tables", "tables");
    tree_tables_->Branch("GeantPhysicsTable", &(table_));
}

//---------------------------------------------------------------------------//
/*!
 * Add physics tables to the ROOT file from given process and particle
 */
void GeantPhysicsTableWriter::add_physics_tables(
    G4VProcess& process, const G4ParticleDefinition& particle)
{
    // Process name
    std::string process_name = process.GetProcessName();

    // Write this->table_
    table_.process_type = select_geant_process_type(process.GetProcessType());
    table_.process      = select_geant_process(process_name);
    table_.particle     = PDGNumber(particle.GetPDGEncoding());

    if (auto em_process = dynamic_cast<G4VEmProcess*>(&process))
    {
        // G4VEmProcess tables
        this->fill_em_tables(*em_process);
    }
    else if (auto energy_loss = dynamic_cast<G4VEnergyLossProcess*>(&process))
    {
        // G4VEnergyLossProcess tables
        this->fill_energy_loss_tables(*energy_loss);
    }

    else if (auto multiple_scattering
             = dynamic_cast<G4VMultipleScattering*>(&process))
    {
        // G4VMultipleScattering tables
        this->fill_multiple_scattering_tables(*multiple_scattering);
    }

    else
    {
        std::cout << "  No available code for " << process_name << std::endl;
    }
}

//----------------------------------------------------------------------------//
/*!
 * Write EM process tables to the TTree
 */
void GeantPhysicsTableWriter::fill_em_tables(const G4VEmProcess& em_process)
{
    std::string process_name = em_process.GetProcessName();

    for (auto i : celeritas::range(em_process.GetNumberOfModels()))
    {
        // Model
        std::string model_name = em_process.GetModelByIndex(i)->GetName();

        // Write table_
        table_.model = select_geant_model(model_name);

        std::string table_name = process_name + "_" + model_name;

        // The same model can have both Lambda and LambdaPrim tables
        if (G4PhysicsTable* table = em_process.LambdaTable())
        {
            this->fill_tables_tree(*table, "Lambda", table_name, "xs");
        }

        if (G4PhysicsTable* table = em_process.LambdaTablePrim())
        {
            this->fill_tables_tree(*table, "LambdaPrim", table_name, "xs");
        }
    }
}

//----------------------------------------------------------------------------//
/*!
 * Write energy loss tables to the TTree
 */
void GeantPhysicsTableWriter::fill_energy_loss_tables(
    const G4VEnergyLossProcess& eloss_process)
{
    std::string process_name = eloss_process.GetProcessName();

    for (auto i : celeritas::range(eloss_process.NumberOfModels()))
    {
        // Model
        std::string model_name = eloss_process.GetModelByIndex(i)->GetName();

        // Write table_
        table_.model = select_geant_model(model_name);

        std::string table_name = process_name + "_" + model_name;

        if (G4PhysicsTable* table = eloss_process.DEDXTable())
        {
            this->fill_tables_tree(*table, "DEDX", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.DEDXTableForSubsec())
        {
            this->fill_tables_tree(*table, "SubDEDX", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.DEDXunRestrictedTable())
        {
            this->fill_tables_tree(*table, "DEDXnr", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.IonisationTable())
        {
            this->fill_tables_tree(*table, "Ionisation", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.IonisationTableForSubsec())
        {
            this->fill_tables_tree(
                *table, "SubIonisation", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.CSDARangeTable())
        {
            this->fill_tables_tree(*table, "CSDARange", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.SecondaryRangeTable())
        {
            this->fill_tables_tree(*table, "RangeSec", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.RangeTableForLoss())
        {
            this->fill_tables_tree(*table, "Range", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.InverseRangeTable())
        {
            this->fill_tables_tree(*table, "InverseRange", table_name, "eloss");
        }

        if (G4PhysicsTable* table = eloss_process.LambdaTable())
        {
            this->fill_tables_tree(*table, "Lambda", table_name, "xs");
        }

        if (G4PhysicsTable* table = eloss_process.SubLambdaTable())
        {
            this->fill_tables_tree(*table, "SubLambda", table_name, "xs");
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Write multiple scattering tables to the TTree
 */
void GeantPhysicsTableWriter::fill_multiple_scattering_tables(
    const G4VMultipleScattering& msc_process)
{
    std::string process_name = msc_process.GetProcessName();

    // TODO: Figure out a method to get the number of models. Max is 4.
    // Other classes have a NumberOfModels(), but not G4VMultipleScattering
    for (auto i : celeritas::range(4))
    {
        if (G4VEmModel* model = msc_process.GetModelByIndex(i))
        {
            if (G4PhysicsTable* table = model->GetCrossSectionTable())
            {
                // Table type
                std::string model_name = model->GetName();

                // Write table_
                table_.model = select_geant_model(model_name);

                std::string table_name      = process_name + "_" + model_name;
                std::string table_type_name = "LambdaMod"
                                              + std::to_string(i + 1);
                this->fill_tables_tree(
                    *table, table_type_name, table_name, "eloss");
            }
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Write a given G4PhysicsTable as a GeantPhysicsVector in this->table_
 */
void GeantPhysicsTableWriter::fill_physics_vectors(G4PhysicsTable& table,
                                                   std::string     xs_or_eloss)
{
    REQUIRE(&table);

    // Clean up so previous vector data is not carried forward
    table_.physics_vectors.clear();

    // Loop over G4PhysicsTable
    for (auto phys_vector : table)
    {
        GeantPhysicsVector geant_physics_vector;

        // Populate GeantPhysicsVector and push it back to this->table_
        geant_physics_vector.vector_type
            = this->select_geant_physics_vector_type(phys_vector->GetType());

        if (xs_or_eloss == "xs")
        {
            geant_physics_vector.is_eloss = false;
        }
        else
        {
            geant_physics_vector.is_eloss = true;
        }

        for (auto j : celeritas::range(phys_vector->GetVectorLength()))
        {
            // Code interface for changing G4PhysicsVector data units
            real_type energy   = phys_vector->Energy(j) / MeV;
            real_type xs_eloss = (*phys_vector)[j];

            if (geant_physics_vector.is_eloss)
            {
                xs_eloss /= MeV;
            }
            else
            {
                xs_eloss /= (1 / cm);
            }

            geant_physics_vector.energy.push_back(energy);     // [MeV]
            geant_physics_vector.xs_eloss.push_back(xs_eloss); // [1/cm or MeV]
        }
        table_.physics_vectors.push_back(geant_physics_vector);
    }
}

//---------------------------------------------------------------------------//
/*!
 * To be called after a G4PhysicsTable has been assigned.
 * It finishes writing the remaining elements of this->table_ and fills the
 * "tables" TTree.
 */
void GeantPhysicsTableWriter::fill_tables_tree(G4PhysicsTable& table,
                                               std::string     table_type_name,
                                               std::string     table_name,
                                               std::string     xs_or_eloss)
{
    REQUIRE(&table);

    // Table type
    table_.table_type = select_geant_table_type(table_type_name);

    // Populate this->table_.physics_vectors and fill the TTree
    fill_physics_vectors(table, xs_or_eloss);
    tree_tables_->Fill();

    // Print message
    table_name = table_type_name + "_" + table_name;
    std::cout << "  Added " << table_name << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Safely switch from G4PhysicsVectorType to GeantPhysicsVectorType.
 * [See G4PhysicsVectorType.hh]
 */
const GeantPhysicsVectorType
GeantPhysicsTableWriter::select_geant_physics_vector_type(
    const G4PhysicsVectorType g4_vector_type)
{
    switch (g4_vector_type)
    {
        case G4PhysicsVectorType::T_G4PhysicsVector:
            return GeantPhysicsVectorType::base;
        case G4PhysicsVectorType::T_G4PhysicsLinearVector:
            return GeantPhysicsVectorType::linear;
        case G4PhysicsVectorType::T_G4PhysicsLogVector:
            return GeantPhysicsVectorType::log;
        case G4PhysicsVectorType::T_G4PhysicsLnVector:
            return GeantPhysicsVectorType::ln;
        case G4PhysicsVectorType::T_G4PhysicsFreeVector:
            return GeantPhysicsVectorType::free;
        case G4PhysicsVectorType::T_G4PhysicsOrderedFreeVector:
            return GeantPhysicsVectorType::ordered_free;
        case G4PhysicsVectorType::T_G4LPhysicsFreeVector:
            return GeantPhysicsVectorType::low_energy_free;
    }
    CHECK(false);
}

//---------------------------------------------------------------------------//
/*!
 * Safely switch from G4PhysicsVectorType to GeantPhysicsVectorType.
 * [See G4PhysicsVectorType.hh]
 */
const GeantProcessType GeantPhysicsTableWriter::select_geant_process_type(
    const G4ProcessType g4_process_type)
{
    switch (g4_process_type)
    {
        case G4ProcessType::fNotDefined:
            return GeantProcessType::not_defined;
        case G4ProcessType::fTransportation:
            return GeantProcessType::transportation;
        case G4ProcessType::fElectromagnetic:
            return GeantProcessType::electromagnetic;
        case G4ProcessType::fOptical:
            return GeantProcessType::optical;
        case G4ProcessType::fHadronic:
            return GeantProcessType::hadronic;
        case G4ProcessType::fPhotolepton_hadron:
            return GeantProcessType::photolepton_hadron;
        case G4ProcessType::fDecay:
            return GeantProcessType::decay;
        case G4ProcessType::fGeneral:
            return GeantProcessType::general;
        case G4ProcessType::fParameterisation:
            return GeantProcessType::parameterisation;
        case G4ProcessType::fUserDefined:
            return GeantProcessType::user_defined;
        case G4ProcessType::fParallel:
            return GeantProcessType::parallel;
        case G4ProcessType::fPhonon:
            return GeantProcessType::phonon;
        case G4ProcessType::fUCN:
            return GeantProcessType::ucn;
    }
    CHECK(false);
}