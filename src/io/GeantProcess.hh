//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantProcess.hh
//! \brief Enumerator for the available physics processes
//---------------------------------------------------------------------------//
#pragma once

#include <string>

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * This enum does not exists in Geant4. It was created to safely access the
 * many physics tables imported from it.
 *
 * Geant4 has process names hardcoded as strings. The GeantProcess provides
 * an interface to select a GeantProcess enum from a string
 */
enum class GeantProcess
{
    // EM process list
    not_defined,
    ion_ioni,
    msc,
    h_ioni,
    h_brems,
    h_pair_prod,
    coulomb_scat,
    e_ioni,
    e_brem,
    photoelectric,
    compton,
    conversion,
    rayleigh,
    annihilation,
    mu_ioni,
    mu_brems,
    mu_pair_prod,

    // Extra
    transportation
};

//---------------------------------------------------------------------------//
} // namespace celeritas
