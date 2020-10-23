//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantTableTypeHelper.hh
//! \brief Helper function to select a GeantTableType enum from a string
//---------------------------------------------------------------------------//
#pragma once 

#include <string>
#include <unordered_map>

#include "GeantTableType.hh"
#include "base/Assert.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Helper function to retrieve the correct process enum from a given string
 */
GeantTableType select_geant_table_type(std::string& g4_process_name)
{
    const static std::unordered_map<std::string, GeantTableType> process_map = {
        // clang-format off
        {"not_defined",  GeantTableType::not_defined},
        {"DEDX",         GeantTableType::dedx},
        {"Ionisation",   GeantTableType::ionisation},
        {"Range",        GeantTableType::range},
        {"RangeSec",     GeantTableType::range_sec},
        {"InverseRange", GeantTableType::inverse_range},
        {"Lambda",       GeantTableType::lambda},
        {"LambdaPrim",   GeantTableType::lambda_prim},
        {"LambdaMod1",   GeantTableType::lambda_mod_1},
        {"LambdaMod2",   GeantTableType::lambda_mod_2},
        {"LambdaMod3",   GeantTableType::lambda_mod_3},
        {"LambdaMod4",   GeantTableType::lambda_mod_4},
        // clang-format on
    };

    auto iter = process_map.find(g4_process_name);
    CHECK(iter != process_map.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
} // namespace celeritas