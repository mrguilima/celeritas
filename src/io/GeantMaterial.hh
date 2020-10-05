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

#include "GeantElement.hh"
#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store data of a given material and its elements. Used by GeantMaterialTable.
 */
struct GeantMaterial
{
    std::string               name;
    real_type                 density; // [g/cm^3]
    std::vector<GeantElement> elements;
};

//---------------------------------------------------------------------------//
} // namespace celeritas