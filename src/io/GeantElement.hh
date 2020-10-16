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
 *
 * The data is exported via the app/geant-exporter. For further expanding this
 * struct, add the aproppriate variables here and fetch the new values in
 * \c app/geant-exporter.cc : store_geometry(...)
 */
struct GeantElement
{
    std::string name;
    int         atomic_number;
    real_type   atomic_mass;           // [atomic mass unit]
    real_type   radiation_length_tsai; // [g/cm^2]
    real_type   coulomb_factor;
};

//---------------------------------------------------------------------------//
} // namespace celeritas