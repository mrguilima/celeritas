//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantMaterialTable.hh
//! \brief Store material information
//---------------------------------------------------------------------------//
#pragma once

#include <string>

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store material information. The material index is the position of said
 * material in the vector<GeantPhysicsVector> in each table
 */
struct GeantMaterialTable
{
    std::string name;
    ssize_t     index;
};

//---------------------------------------------------------------------------//
} // namespace celeritas