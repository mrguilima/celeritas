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
#include "GeantGeometryMap.hh"
#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Enum for storing G4State enumerators
 * [See G4Material.hh]
 */
enum class GeantMaterialState
{
    not_defined,
    solid,
    liquid,
    gas
};

//---------------------------------------------------------------------------//
/*!
 * Store data of a given material and its elements.
 *
 * Used by the GeantGeometryMap class.
 *
 * The data is exported via the app/geant-exporter. For further expanding
 * this struct, add the aproppriate variables here and fetch the new values in
 * \c app/geant-exporter.cc : store_geometry(...).
 *
 * Units are defined at export time in the aforementioned function.
 */
struct GeantMaterial
{
    std::string              name;
    GeantMaterialState       state;
    real_type                temperature;        // [K]
    real_type                density;            // [g/cm^3]
    real_type                electron_density;   // [1/cm^3]
    real_type                atomic_density;     // [1/cm^3]
    real_type                radiation_length;   // [cm]
    real_type                nuclear_int_length; // [cm]
    std::vector<int>         elements;
    std::map<int, real_type> fractions;
};

//---------------------------------------------------------------------------//
} // namespace celeritas