//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTableWriter.hh
//! \brief Write Geant physics tables into a ROOT file
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

//---------------------------------------------------------------------------//
/*!
 * Use an existing TFile address as input to create a new "tables" TTree used
 * store Geant physics tables
 *
 * TFile passed to the constructor must be open with the "recreate" flag so the
 * class has writing privileges.
 */
class GeantPhysicsTableWriter
{
  public:
    // Constructor creates a new "tables" TTree to the existing ROOT TFile
    GeantPhysicsTableWriter(TFile* root_file);
    // Default destructor
    ~GeantPhysicsTableWriter() = default;

    // Write physics table of a given particle and physics process
    // Expected to be called within a G4ParticleTable iterator loop
    void add_physics_table(G4VProcess* process, G4ParticleDefinition* particle);

  protected:
    // Write a given table as a new entry in the tables TBranch
    void fill_physics_vectors(G4PhysicsTable* table);

  protected:
    // TTree created by the constructor
    std::unique_ptr<TTree> tree_tables_;
    // Object written in the TTree. Each table is a new TTree entry
    GeantPhysicsTable      table_;
};