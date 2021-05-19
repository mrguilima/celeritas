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
void GCheckRunner::operator()(const GeoTrackInitializer* gti) const
{
    using StateStore = CollectionStateStore<GeoStateData, MemSpace::device>;

    CELER_EXPECT(gti);

    // run on the CPU
    CELER_LOG(status) << "Propagating track(s) on CPU";
    auto cpudata = run_cpu(geo_params_, gti, this->max_steps_);

    // run on the GPU
    GCheckInput input;
    input.init.push_back(*gti);
    input.max_steps = this->max_steps_;
    input.params    = this->geo_params_->device_pointers();

    StateStore states(*this->geo_params_, 1);
    input.state = states.ref();

    CELER_LOG(status) << "Propagating track(s) on GPU";
    auto gpudata = run_gpu(input);

    std::cerr << " # steps: cpu=" << cpudata.distances.size()
              << " gpu=" << gpudata.distances.size() << "\n";
    CELER_ASSERT(cpudata.ids.size() == gpudata.ids.size());

    if (!(cpudata.ids == gpudata.ids && cpudata.distances == gpudata.distances))
    {
        for (size_type i = 0; i < cpudata.distances.size(); ++i)
        {
            if (fabs(cpudata.distances[i] - gpudata.distances[i]) > 1.0e-18)
            {
                std::cout
                    << "compare i=" << i << ": ids=" << cpudata.ids[i] << ", "
                    << gpudata.ids[i] << "; cpudist=" << cpudata.distances[i]
                    << ", " << gpudata.distances[i] << " , diff="
                    << cpudata.distances[i] - gpudata.distances[i] << "\n";
            }
        }
    }
    CELER_ASSERT(cpudata.ids == gpudata.ids);
    CELER_LOG(status) << "Comparison successful.\n";
}
//---------------------------------------------------------------------------//

} // namespace geo_check
