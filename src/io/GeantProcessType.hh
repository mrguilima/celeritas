//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantProcessType.hh
//! \brief Enumerator for physics process types
//---------------------------------------------------------------------------//
#pragma once

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Same enum as in Geant4. [See Geant4's G4ProcessType.hh]
 */
enum class GeantProcessType
{
    not_defined,
    transportation,
    electromagnetic,
    optical,
    hadronic,
    photolepton_hadron,
    decay,
    general,
    parameterisation,
    user_defined,
    parallel,
    phonon,
    ucn
};

//---------------------------------------------------------------------------//
} // namespace celeritas
