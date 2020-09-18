//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantImporter.hh
//! \brief Load particle data and physics tables from the app/geant-exporter
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "GeantParticle.hh"
#include "GeantPhysicsTable.hh"
#include "physics/base/ParticleParams.hh"
#include "physics/base/ParticleDef.hh"
#include "base/Types.hh"
#include "base/Macros.hh"

// ROOT
class TFile;

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * GeantImporter loads particle and physics table data from the ROOT file
 * created by the app/geant-exporter external code.
 *
 * Usage:
 * \code
 *  GeantImporter import("/path/to/rootfile.root");
 *  auto geant_data = import();
 * \endcode
 *
 * Physics tables currently are a vector<GeantPhysicsTable>, since many
 * parameters are at play when selecting a given table:
 * GeantParticle, GeantTableType, GeantProcess, and GeantModel.
 * See GeantImporter.test.cc for an example.
 * This method will probably have to be improved.
 */
class GeantImporter
{
  public:
    struct result_type
    {
        std::shared_ptr<ParticleParams> particle_params;
        std::shared_ptr<std::vector<GeantPhysicsTable>> physics_tables;
        // TODO:
        // std::shared_ptr<std::vector<GeantMaterialTable> material_tables;
    };

  public:
    // Construct with exported ROOT file
    explicit GeantImporter(const char* filename);

    // Release ROOT file on exit
    ~GeantImporter();

    // Load data from the ROOT file into result_type
    result_type operator()();

  private:
    // Populate the shared_ptr<ParticleParams> with particle information
    std::shared_ptr<ParticleParams> load_particle_data();
    // Populate a vector of GeantPhysicsTable objects
    std::shared_ptr<std::vector<GeantPhysicsTable>> load_physics_table_data();
    // TODO
    // std::shared_ptr<std::vector<GeantMaterialTable>> load_material_table_data();

  public:
    std::unique_ptr<TFile> root_input_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
