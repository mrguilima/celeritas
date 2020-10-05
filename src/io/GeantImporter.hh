//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantImporter.hh
//! \brief Import all the data exported by the app/geant-exporter
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "GeantParticle.hh"
#include "GeantPhysicsTable.hh"
#include "GeantMaterialTable.hh"
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
 * See GeantImporter.test.cc for an example on how to fetch a given table.
 * This method will probably have to be improved.
 *
 * Material and volume information is stored in a GeantMaterialTable object.
 */
class GeantImporter
{
  public:
    struct result_type
    {
        std::shared_ptr<ParticleParams>                 particle_params;
        std::shared_ptr<std::vector<GeantPhysicsTable>> physics_tables;
        std::shared_ptr<GeantMaterialTable>             materials;
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
    // Load the GeantMaterialTable and update the shared_ptr address
    std::shared_ptr<GeantMaterialTable> load_material_data();

  public:
    std::unique_ptr<TFile> root_input_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
