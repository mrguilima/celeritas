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
class G4VEmProcess;
class G4VEnergyLossProcess;
class G4VMultipleScattering;
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
    // Constructor adds a new "tables" TTree to the existing ROOT TFile
    GeantPhysicsTableWriter(TFile* root_file);
    // Default destructor
    ~GeantPhysicsTableWriter() = default;

    // Write the physics tables from a given particle and physics process
    // Expected to be called within a G4ParticleTable iterator loop
    void
    add_physics_tables(G4VProcess* process, G4ParticleDefinition* particle);

  protected:
    // Loop over EM processes and write tables to the ROOT file
    void fill_em_tables(G4VEmProcess* em_process);
    // Loop over energy loss processes and write tables to the ROOT file
    void fill_energy_loss_tables(G4VEnergyLossProcess* eloss_process);
    // Loop over multiple scattering processes and write tables to the ROOT
    // file
    void fill_multiple_scattering_tables(G4VMultipleScattering* msc_process);
    // Write the physics vectors from a given G4PhysicsTable to this->table_
    void fill_physics_vectors(G4PhysicsTable* table);

  protected:
    // TTree created by the constructor
    std::unique_ptr<TTree> tree_tables_;
    // Object written in the TTree. Each GeantPhysicsTable is a new TTree entry
    GeantPhysicsTable table_;
};