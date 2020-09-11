//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantParticle.hh
//! \brief Store Geant4 G4ParticleDefinition data
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store G4ParticleDefinition data from Geant4.
 *
 * GeantParticle objects are stored into a ROOT file by the app/geant-exporter
 * and retrieved by the GeantImporter class.
 */
struct GeantParticle
{
    std::string name;
    int         pdg;
    real_type   mass; // [MeV]
    real_type   charge;
    real_type   spin;
    real_type   lifetime; // [s]
    bool        is_stable;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
