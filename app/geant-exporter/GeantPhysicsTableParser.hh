//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTableParser.hh
//---------------------------------------------------------------------------//

#pragma once

// C++
#include <iostream>
#include <vector>
#include <memory>

#include "io/GeantPhysicsTable.hh"

// ROOT
class TFile;

// Geant
class G4VProcess;
class G4ParticleDefinition;
class G4PhysicsTable;

using namespace celeritas;

class GeantPhysicsTableParser
{
  public:
    GeantPhysicsTableParser(TFile* root_file);
    ~GeantPhysicsTableParser() = default;

    void add_physics_table(G4VProcess* process, G4ParticleDefinition* particle);

  private:
    void write_tree(std::string tree_name, G4PhysicsTable* table);
    void replace_characters(std::string&      string,
                            std::string const search,
                            std::string const replace);

  private:
    std::unique_ptr<TFile>       root_file_;
    GeantPhysicsTable table_;

    // int                              process_type_;
    // std::string                      process_type_name_;
    // std::string                      table_type_;
    // std::string                      process_;
    // std::string                      model_;
    // std::string                      particle_;
    // int                              pdg_;
    // std::vector<int>                 vector_type_;
    // std::vector<std::vector<double>> energy_;   // Geant4 binVector
    // std::vector<std::vector<double>> xs_eloss_; // Geant4 dataVector
};