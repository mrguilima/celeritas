//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsVector.hh
//! \brief Stores Geant4 physics vectors
//---------------------------------------------------------------------------//
#pragma once

#include "GeantPhysicsVectorType.hh"
#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store Geant4 physics vector data
 */
struct GeantPhysicsVector
{
    GeantPhysicsVectorType vector_type;
    std::vector<real_type> energy;   // Geant4 binVector
    std::vector<real_type> xs_eloss; // Geant4 dataVector
};

//---------------------------------------------------------------------------//
} // namespace celeritas
