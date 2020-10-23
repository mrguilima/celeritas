//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantProcessHelper.hh
//! \brief Helper function to select a GeantProcess enum from an input string
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <unordered_map>

#include "GeantProcess.hh"
#include "base/Assert.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Helper function to retrieve the correct process enum from a given string
 */
GeantProcess select_geant_process(std::string& g4_process_name)
{
    const static std::unordered_map<std::string, GeantProcess> process_map = {
        // clang-format off
        {"not_defined",    GeantProcess::not_defined},
        {"ionIoni",        GeantProcess::ion_ioni},
        {"msc",            GeantProcess::msc},
        {"hIoni",          GeantProcess::h_ioni},
        {"hBrems",         GeantProcess::h_brems},
        {"hPairProd",      GeantProcess::h_pair_prod},
        {"CoulombScat",    GeantProcess::coulomb_scat},
        {"eIoni",          GeantProcess::e_ioni},
        {"eBrem",          GeantProcess::e_brem},
        {"phot",           GeantProcess::photoelectric},
        {"compt",          GeantProcess::compton},
        {"conv",           GeantProcess::conversion},
        {"Rayl",           GeantProcess::rayleigh},
        {"annihil",        GeantProcess::annihilation},
        {"muIoni",         GeantProcess::mu_ioni},
        {"muBrems",        GeantProcess::mu_brems},
        {"muPairProd",     GeantProcess::mu_pair_prod},
        {"Transportation", GeantProcess::transportation},

        // clang-format on
    };

    auto iter = process_map.find(g4_process_name);
    CHECK(iter != process_map.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
} // namespace celeritas
