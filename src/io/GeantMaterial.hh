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
 * Store data of a given material and its elements.
 *
 * Used by the GeantGeometryMap class.
 *
 * The data is exported via the app/geant-exporter. For further expanding
 * this struct, add the aproppriate variables here and fetch the new values in
 * \c app/geant-exporter.cc : store_geometry(...)
 */
struct GeantMaterial
{
    std::string               name;
    real_type                 density;            // [g/cm^3]
    real_type                 electron_density;   // [1/cm^3]
    real_type                 atomic_density;     // [1/cm^3]
    real_type                 radiation_length;   // [g/cm^2]
    real_type                 nuclear_int_length; // [g/cm^2]
    std::vector<GeantElement> elements;
};

//---------------------------------------------------------------------------//
} // namespace celeritas