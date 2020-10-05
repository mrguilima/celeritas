//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantVolume.hh
//! \brief Store volume information
//---------------------------------------------------------------------------//
#pragma once

#include <string>

#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store data for each volume. Struct used by the GeantMaterialTable class.
 *
 * The reason to create a separate struct instead of just populate a map with
 * the volume name and id directly into GeantMaterialTable is to easily
 * expand volume information if needed.
 */
struct GeantVolume
{
    std::string name;
};

//---------------------------------------------------------------------------//
} // namespace celeritas