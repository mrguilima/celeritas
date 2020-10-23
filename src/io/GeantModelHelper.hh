//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantModelHelper.hh
//! \brief Helper function to select a GeantModel enum from an input string
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <unordered_map>

#include "GeantModel.hh"
#include "base/Assert.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Helper function to retrieve the correct model enum from a given string
 */
GeantModel select_geant_model(std::string& g4_model_name)
{
    const static std::unordered_map<std::string, GeantModel> model_map = {
        // clang-format off
        {"not_defined",         GeantModel::not_defined},
        {"BraggIon",            GeantModel::bragg_ion},
        {"BetheBloch",          GeantModel::bethe_bloch},
        {"UrbanMsc",            GeantModel::urban_msc},
        {"ICRU73QO",            GeantModel::icru_73_qo},
        {"WentzelVIUni",        GeantModel::wentzel_VI_uni},
        {"hBrem",               GeantModel::h_brem},
        {"hPairProd",           GeantModel::h_pair_prod},
        {"eCoulombScattering",  GeantModel::e_coulomb_scattering},
        {"Bragg",               GeantModel::bragg},
        {"MollerBhabha",        GeantModel::moller_bhabha},
        {"eBremSB",             GeantModel::e_brem_sb},
        {"eBremLPM",            GeantModel::e_brem_lpm},
        {"eplus2gg",            GeantModel::e_plus_to_gg},
        {"LivermorePhElectric", GeantModel::livermore_photoelectric},
        {"Klein-Nishina",       GeantModel::klein_nishina},
        {"BetheHeitlerLPM",     GeantModel::bethe_heitler_lpm},
        {"LivermoreRayleigh",   GeantModel::livermore_rayleigh},
        {"MuBetheBloch",        GeantModel::mu_bethe_bloch},
        {"MuBrem",              GeantModel::mu_brem},
        {"muPairProd",          GeantModel::mu_pair_prod},
        // clang-format on
    };

    auto iter = model_map.find(g4_model_name);
    CHECK(iter != model_map.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
} // namespace celeritas