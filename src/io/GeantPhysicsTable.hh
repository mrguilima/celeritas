//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTable.hh
//! \brief Stores physics table data
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
 * Used by the app/geant-exporter external code to store table data into a
 * ROOT file. The GeantImporter class is responsible for loading
 * said data from the ROOT file into memory.
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
