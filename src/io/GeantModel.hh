//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantModel.hh
//! \brief Enumerator for the available physics models
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
 * Geant4 has model names hardcoded as strings. The GeantModelHelper provides 
 * an interface to select a GeantModel enum from a string
 */
enum class GeantModel
{
    // EM model list
    not_defined,
    bragg_ion,
    bethe_bloch,
    urban_msc,
    icru_73_qo,
    wentzel_VI_uni,
    h_brem,
    h_pair_prod,
    e_coulomb_scattering,
    bragg,
    moller_bhabha,
    e_brem_sb,
    e_brem_lpm,
    e_plus_to_gg,
    livermore_photoelectric,
    klein_nishina,
    bethe_heitler_lpm,
    livermore_rayleigh,
    mu_bethe_bloch,
    mu_brem,
    mu_pair_prod
};

//---------------------------------------------------------------------------//
} // namespace celeritas
