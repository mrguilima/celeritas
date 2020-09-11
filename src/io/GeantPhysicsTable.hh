//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTable.hh
//! \brief Store Geant4 physics tables
//---------------------------------------------------------------------------//
#pragma once

#include <vector>

#include "GeantProcessType.hh"
#include "GeantTableType.hh"
#include "GeantProcess.hh"
#include "GeantModel.hh"
#include "physics/base/ParticleMd.hh"
#include "GeantPhysicsVector.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store Geant4 physics tables.
 * The geant-exporter app stores Geant4 physics tables into a ROOT file, while
 * the GeantImporter class is responsible for loading said data into memory.
 */
struct GeantPhysicsTable
{
    GeantProcessType                process_type;
    GeantTableType                  table_type;
    GeantProcess                    process;
    GeantModel                      model;
    PDGNumber                       particle;
    std::vector<GeantPhysicsVector> physics_vectors;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
