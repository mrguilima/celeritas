//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTrack.cu
//---------------------------------------------------------------------------//
#include "InitializeTrack.cuh"

namespace celeritas
{
//---------------------------------------------------------------------------//
__global__ initialize_tracks(span<const Primary>          primaries,
                             const ParticleParamsPointers pparams,
                             const ParticleStatePointers  pstates,
                             const GeoParamsPointers      gparams,
                             const GeoStatePointers       gstates,
                             ThreadId starting_thread_offset)
{
    auto local_thread_id = celeritas::KernelParamCalculator::thread_id();
    if (!(local_thread_id < primaries.size()))
        return;
    const Primary& primary = primaries[local_thread_id.get()];
    ThreadId       offset_thread_id{starting_thread_offset.get()
                              + local_thread_id.get()};

    // TODO: Initialize sim state
    {}

    // Initialize particle physics data
    {
        ParticleTrackView particle(pparams, pstates, offset_thread_id);
        particle = primary.particle;
    }

    // Initialize geometry state
    {
        GeoTrackView geo(gparams, gstates, offset_thread_id);
        geo = primary.geo;
    }
}

//---------------------------------------------------------------------------//
} // namespace celeritas
