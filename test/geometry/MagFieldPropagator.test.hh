//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file MagFieldPropagator.test.hh
//---------------------------------------------------------------------------//
#pragma once

#include <vector>
#include "geometry/GeoStatePointers.hh"
#include "geometry/GeoParamsPointers.hh"
#include "magfield/MagFieldPropagator.hh"

namespace celeritas_test
{
using namespace celeritas;
//---------------------------------------------------------------------------//
// TESTING INTERFACE
//---------------------------------------------------------------------------//
using MagPropTestInit = GeoStateInitializer;

//! Input data
struct MagPropTestInput
{
    std::vector<MagPropTestInit> init;
    int                          max_segments = 0;
    GeoParamsPointers            shared;
    GeoStatePointers             state;
};

//---------------------------------------------------------------------------//
//! Output results
struct MagPropTestOutput
{
    std::vector<int>    ids;
    std::vector<double> distances;
};

//---------------------------------------------------------------------------//
//! Run on device and return results
MagPropTestOutput MagProp_test(MagPropTestInput);

//---------------------------------------------------------------------------//
} // namespace celeritas_test
