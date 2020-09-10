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
class TTree;

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

  protected:
    void write_tree(std::string tree_name, G4PhysicsTable* table);
    void replace_characters(std::string&      string,
                            std::string const search,
                            std::string const replace);

  protected:
    std::unique_ptr<TTree> tree_tables_;
    GeantPhysicsTable      table_;
};