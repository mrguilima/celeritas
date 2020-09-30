//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantMaterial.hh
//! \brief Store material information
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <vector>

#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store data for each individual element and material
 */
struct GeantElement
{
    std::string name;
    real_type   fraction;
    int         z;
    real_type   atomic_mass;
};

struct GeantMaterial
{
    std::string               name;
    real_type                 density; // [g/cm^3]
    std::vector<GeantElement> elements;
};

//---------------------------------------------------------------------------//
} // namespace celeritas