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
    this->root_file_.reset(root_file);
}

//---------------------------------------------------------------------------//
/*!
 * Add a new physics table as a new TTree by providing process and particle
 */
void GeantPhysicsTableParser::add_physics_table(G4VProcess*           process,
                                                G4ParticleDefinition* particle)
{
    // TO BE FIXED
    G4ProcessType process_type    = process->GetProcessType();
    std::string process_type_name = process->GetProcessTypeName(process_type);

    std::string tree_name;

    this->table_.particle = PDGNumber(particle->GetPDGEncoding());

    this->particle_ = particle->GetParticleName();
    this->pdg_      = particle->GetPDGEncoding();
    this->process_  = process->GetProcessName();
    this->model_    = "NA";

    auto em_process          = dynamic_cast<G4VEmProcess*>(process);
    auto eloss_process       = dynamic_cast<G4VEnergyLossProcess*>(process);
    auto multiple_scattering = dynamic_cast<G4VMultipleScattering*>(process);

    //-----------------------------------------------------------------------//
    //! G4VEmProcess
    if (em_process)
    {
        for (int i = 0; i < em_process->GetNumberOfModels(); i++)
        {
            G4PhysicsTable* table = nullptr;

            model_ = em_process->GetModelByIndex(i)->GetName();

            if (em_process->LambdaTable())
            {
                table       = em_process->LambdaTable();
                table_type_ = "Lambda";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (em_process->LambdaTablePrim())
            {
                table       = em_process->LambdaTablePrim();
                table_type_ = "LambdaPrim";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (table == nullptr)
            {
                std::cout << "No available table in ";
                std::cout << "GeantPhysicsTableParser::addPhysicsTableTree ";
                std::cout << "for G4VEmProcess " << process_ << ".";
                std::cout << particle_ << std::endl;
            }
        }
    }

    //-----------------------------------------------------------------------//
    //! G4VEnergyLossProcess
    else if (eloss_process)
    {
        for (int i = 0; i < eloss_process->NumberOfModels(); i++)
        {
            G4PhysicsTable* table = nullptr;

            model_ = eloss_process->GetModelByIndex(i)->GetName();

            if (eloss_process->DEDXTable())
            {
                table       = eloss_process->DEDXTable();
                table_type_ = "DEDX";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->DEDXTableForSubsec())
            {
                table       = eloss_process->DEDXTableForSubsec();
                table_type_ = "SubDEDX";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->DEDXunRestrictedTable())
            {
                table       = eloss_process->DEDXunRestrictedTable();
                table_type_ = "DEDXnr";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->IonisationTable())
            {
                table       = eloss_process->IonisationTable();
                table_type_ = "Ionisation";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->IonisationTableForSubsec())
            {
                table       = eloss_process->IonisationTableForSubsec();
                table_type_ = "SubIonisation";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->CSDARangeTable())
            {
                table       = eloss_process->CSDARangeTable();
                table_type_ = "CSDARange";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->SecondaryRangeTable())
            {
                table       = eloss_process->SecondaryRangeTable();
                table_type_ = "RangeSec";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->RangeTableForLoss())
            {
                table       = eloss_process->RangeTableForLoss();
                table_type_ = "Range";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->InverseRangeTable())
            {
                table       = eloss_process->InverseRangeTable();
                table_type_ = "InverseRange";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->LambdaTable())
            {
                table       = eloss_process->LambdaTable();
                table_type_ = "Lambda";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (eloss_process->SubLambdaTable())
            {
                table       = eloss_process->SubLambdaTable();
                table_type_ = "SubLambda";
                tree_name = table_type_ + "_" + process_ + "_" + model_ + "_";
                tree_name += particle_;
                write_tree(tree_name, table);
            }

            if (table == nullptr)
            {
                std::cout << "No available table in ";
                std::cout << "GeantPhysicsTableParser::addPhysicsTableTree ";
                std::cout << "for G4VEnergyLossProcess " << process_ << ".";
                std::cout << particle_ << std::endl;
            }
        }
    }

    //-----------------------------------------------------------------------//
    //! G4VMultipleScattering
    else if (multiple_scattering)
    {
        for (int i = 0; i < 4; i++)
        {
            G4VEmModel* model = multiple_scattering->GetModelByIndex(i);

            if (model)
            {
                G4PhysicsTable* table = model->GetCrossSectionTable();
                if (table)
                {
                    model_      = model->GetName();
                    table_type_ = "LambdaMod" + std::to_string(i + 1);
                    tree_name   = table_type_ + "_" + process_ + "_";
                    tree_name += model_ + "_" + particle_;
                    write_tree(tree_name, table);
                }
            }
        }
    }

    //-----------------------------------------------------------------------//
    else
    {
        std::cout << "No available code for ";
        std::cout << particle_ << "." << process_ << std::endl;
    }
}

//---------------------------------------------------------------------------//
/*!
 * Writes a given physics table as a tree to this->root_output_
 */
void GeantPhysicsTableParser::write_tree(std::string     tree_name,
                                         G4PhysicsTable* table)
{
    // Replacing special characters. ROOT TTrees do not bode well with them
    replace_last_character(tree_name, "+", "Plus");
    replace_last_character(tree_name, "-", "Minus");
    replace_characters(tree_name, ".", "_");
    replace_characters(tree_name, "-", "_");

    root_output_
        = std::make_unique<TFile>(root_output_filename_.c_str(), "update");

    TDirectory* tables = (TDirectory*)root_output_->Get("tables");

    if (!tables)
    {
        tables = root_output_->mkdir("tables");
        tables->cd();
    }

    else
    {
        root_output_->Cd("tables");
    }

    // Creating the TTree
    TTree* tree = new TTree(tree_name.c_str(), tree_name.c_str());

    tree->Branch("processType", &process_type_, "processType/I");
    tree->Branch("processTypeName", &process_type_name_);
    tree->Branch("tableType", &table_type_);
    tree->Branch("process", &process_);
    tree->Branch("model", &model_);
    tree->Branch("particle", &particle_);
    tree->Branch("pdg", &pdg_, "pdg/I");
    tree->Branch("vectorType", &vector_type_);
    tree->Branch("energy", &energy_);
    tree->Branch("xs_eloss", &xs_eloss_);

    this->vector_type_.clear();
    this->energy_.clear();
    this->xs_eloss_.clear();

    // G4PhysicsTable is a std::vector<G4PhysicsVector>
    for (auto phys_vector : *table)
    {
        std::vector<double> phys_vector_energy;
        std::vector<double> phys_vector_xs_eloss;

        for (std::size_t j = 0; j < phys_vector->GetVectorLength(); j++)
        {
            phys_vector_energy.push_back(phys_vector->Energy(j));
            phys_vector_xs_eloss.push_back((*phys_vector)[j]);
        }

        this->vector_type_.push_back(phys_vector->GetType());
        this->energy_.push_back(phys_vector_energy);
        this->xs_eloss_.push_back(phys_vector_xs_eloss);
    }

    tree->Fill();

    this->root_output_->Write();
    this->root_output_->Close();

    std::cout << "  Added " << tree_name << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Replaces characters in a string
 */
void replace_characters(std::string&      aString,
                        std::string const search,
                        std::string const replace)
{
    size_t position = 0;

    while ((position = aString.find(search, position)) != std::string::npos)
    {
        aString.replace(position, search.length(), replace);
        position += replace.length();
    }
}

//---------------------------------------------------------------------------//
/*!
 * Replaces last character in a string
 */
void replace_last_character(std::string&      aString,
                            std::string const search,
                            std::string const replace)
{
    if (aString.back() == search)
    {
        aString.pop_back();
        aString += replace;
    }
}