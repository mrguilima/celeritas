//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantElement.hh
//! \brief Store element information
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <vector>

#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store element data. Used by GeantMaterial and GeantGeometryMap.
 */
struct GeantElement
{
    std::string name;
    real_type   fraction;
    int         z;
    real_type   atomic_mass;
};

//---------------------------------------------------------------------------//
} // namespace celeritas