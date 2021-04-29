//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GCheckKernel.cc
//---------------------------------------------------------------------------//
#include "GCheckKernel.hh"

#include "geometry/GeoInterface.hh"
#include "geometry/LinearPropagator.hh"
#include "base/CollectionStateStore.hh"

using namespace celeritas;

namespace geo_check
{
using namespace celeritas;

/*!
 *  Run tracking on the CPU
 */
void run_cpu(const SPConstGeo&          params,
             const GeoTrackInitializer* init,
             int                        max_steps)
{
    using StateStore = CollectionStateStore<GeoStateData, MemSpace::host>;

    StateStore state = StateStore(*params, 1);

    GeoTrackView geo(params->host_pointers(), state.ref(), ThreadId(0));
    geo = GeoTrackInitializer{init->pos, init->dir};

    LinearPropagator propagate(&geo); // one propagator per track

    printf("Initial track: pos=(%f, %f, %f), dir=(%f, %f, %f), outside=%i\n",
           geo.pos()[0],
           geo.pos()[1],
           geo.pos()[2],
           geo.dir()[0],
           geo.dir()[1],
           geo.dir()[2],
           geo.is_outside());

    // Track from outside detector, moving right
    for (int istep = 0; istep < max_steps; ++istep)
    {
        if (geo.is_outside())
            break;

        auto step = propagate(); // to next boundary
        printf("step=%i: volid=%i dist=%f, curpos=(%f, %f, %f)\n",
               istep,
               (geo.is_outside() ? -1 : step.volume.get()),
               step.distance,
               geo.pos()[0],
               geo.pos()[1],
               geo.pos()[2]);
    }
}

} // namespace geo_check
