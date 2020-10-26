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
 * Store data for each volume.
 *
 * Used by the GeantGeometryMap class.
 *
 * The reason for this (currently absurdly simple) struct instead of just
 * keeping tab of volume names directly in the GeantGeometryMap, is to
 * easily import further volume information in the future.
 */
struct GeantVolume
{
    std::string name;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
