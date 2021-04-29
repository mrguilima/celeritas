//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GCheckRunner.cc
//---------------------------------------------------------------------------//
#include "GCheckRunner.hh"

#include "base/CollectionStateStore.hh"
#include "base/Stopwatch.hh"
#include "base/ColorUtils.hh"
#include "comm/Logger.hh"
#include "geometry/GeoParams.hh"
#include "geometry/GeoInterface.hh"
#include "geometry/GeoTrackView.hh"
#include "geometry/LinearPropagator.hh"
#include "GCheckKernel.hh"
#include "VecGeom/navigation/NavigationState.h"

using namespace celeritas;

namespace geo_check
{
using NavState = vecgeom::NavigationState;

//---------------------------------------------------------------------------//
/*!
 * Constructor, takes ownership of SPConstGeo object received
 */
GCheckRunner::GCheckRunner(const SPConstGeo& geometry, int max_steps)
    : geo_params_(std::move(geometry)), max_steps_(max_steps)
{
    CELER_EXPECT(geo_params_);
    CELER_EXPECT(max_steps > 0);
}

//---------------------------------------------------------------------------//
/*!
 * Propagate a track for debugging purposes
 */
void GCheckRunner::operator()(const GeoTrackInitializer* init,
                              int /*ntrks*/) const
{
    CELER_EXPECT(init);

    // run on the CPU
    CELER_LOG(status) << "Propagating track(s) on CPU";
    run_cpu(geo_params_, init, max_steps_);

    // run on the GPU
    CELER_LOG(status) << "Propagating track(s) on GPU";
    run_gpu(geo_params_, init, max_steps_);
}

//---------------------------------------------------------------------------//
} // namespace geo_check
