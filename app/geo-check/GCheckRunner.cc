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
void GCheckRunner::operator()(const real_type* trkdata) const
{
    CELER_EXPECT(trkdata);

    // run on the CPU
    CELER_LOG(status) << "Propagating track(s) on CPU";
    Real3               pos{trkdata[0], trkdata[1], trkdata[2]};
    Real3               dir{trkdata[3], trkdata[4], trkdata[5]};
    GeoTrackInitializer gti{pos, dir};
    run_cpu(geo_params_, &gti, this->max_steps_);

    // run on the GPU
    GCheckInput input;
    input.init.push_back(gti);
    input.max_steps = 200;
    input.params    = this->geo_params_->device_pointers();

    CollectionStateStore<GeoStateData, MemSpace::device> states(
        *this->geo_params_, 1);
    input.state = states.ref();

    CELER_LOG(status) << "Propagating track(s) on GPU";
    run_gpu(input);
}
//---------------------------------------------------------------------------//

/*!
 * Propagate a track for debugging purposes
 */
void GCheckRunner::operator()(const GeoTrackInitializer* gti) const
{
    using StateStore = CollectionStateStore<GeoStateData, MemSpace::device>;

    CELER_EXPECT(gti);

    // run on the CPU
    CELER_LOG(status) << "Propagating track(s) on CPU";
    run_cpu(geo_params_, gti, this->max_steps_);

    // run on the GPU
    GCheckInput input;
    input.init.push_back(*gti);
    input.max_steps = 200;
    input.params    = this->geo_params_->device_pointers();

    StateStore states(*this->geo_params_, 1);
    input.state = states.ref();

    CELER_LOG(status) << "Propagating track(s) on GPU";
    run_gpu(input);
}
//---------------------------------------------------------------------------//

} // namespace geo_check
