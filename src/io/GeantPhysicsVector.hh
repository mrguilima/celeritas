//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsVector.hh
//! \brief Store Geant4 physics vector data
//---------------------------------------------------------------------------//
#pragma once

#include "GeantPhysicsVectorType.hh"
#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store Geant4 physics vector data [see Geant4's G4PhysicsVector.hh].
 *
 * MeV and cm units are defined in
 * \c GeantPhysicsTableWriter::fill_physics_vectors(...)
 */
struct GeantPhysicsVector
{
    GeantPhysicsVectorType vector_type;
    bool                   is_eloss; // Check if it is a xs or eloss dataVector
    std::vector<real_type> energy;   // [MeV] Geant4 binVector
    std::vector<real_type> xs_eloss; // [1/cm or MeV] Geant4 dataVector
};

//---------------------------------------------------------------------------//
} // namespace celeritas
