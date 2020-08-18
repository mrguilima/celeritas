//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTrack.cu
//---------------------------------------------------------------------------//
#include "InitializeTrack.cuh"

#include <algorithm>
#include <vector>
#include <thrust/device_vector.h>
#include "base/KernelParamCalculator.cuda.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
__global__ initialize_tracks(size_type                    size,
                             TrackInitializer*            initializers,
                             size_type*                   vacancies,
                             //const SimParamsPointers      sparams,
                             //const SimStatePointers       sstates,
                             const ParticleParamsPointers pparams,
                             const ParticleStatePointers  pstates,
                             const GeoParamsPointers      gparams,
                             const GeoStatePointers       gstates)
{
    auto thread_id = KernelParamCalculator::thread_id();
    if (thread_id < size)
    {
        // Index of the empty slot in the track states
        ThreadId slot_id = ThreadId(vacancies[thread_id.get()]);

        // Index of the initializer used to fill the empty slot. thread id 0
        // will use the last track initializer in the vector, thread id 1 the
        // next to last, etc.
        size_type init_id = size - thread_id.get() + 1;

        const TrackInitializer& initializer = initializers[init_id];
 
        // Initialize sim state
        {
            //SimTrackView sim(sparams, sstates, slot_id);
            //sim = initializer.sim;
        }
 
        // Initialize particle physics data
        {
            ParticleTrackView particle(pparams, pstates, slot_id);
            particle = initializer.particle;
        }
 
        // Initialize geometry state
        {
            GeoTrackView geo(gparams, gstates, slot_id);
            geo = initializer.geo;
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 *
 */
void pop_tracks(thrust::device_vector<const TrackInitializer>& initializers,
                thrust::device_vector<size_type>&              vacancies,
                //const SimParamsPointers                        sparams,
                //const SimStatePointers                         sstates,
                const ParticleParamsPointers                   pparams,
                const ParticleStatePointers                    pstates,
                const GeoParamsPointers                        gparams,
                const GeoStatePointers                         gstates)
{
    // Number of tracks to initialize
    size_type num_new_tracks = std::min(initializers.size(), vacancies.size());

    // If there are more tracks than vacant slots, resize the vacancies to the
    // number of tracks available
    vacancies.resize(num_new_tracks);

    // Launch kernel to create new tracks from track initializers
    KernelParamCalculator calc_launch_params;
    auto params = calc_launch_params(num_new_tracks);
    initialize_tracks<<<params.grid_size, params.block_size>>>(
        num_new_tracks, thrust::raw_pointer_cast(initializers.data()),
        thrust::raw_pointer_cast(vacancies.data()), //sparams, sstates,
        pparams, pstates, gparams, gstates);

    // Resize the vector of track initializers to hold the tracks that have not
    // been created yet
    size_type size = initializers.size() - num_new_tracks;
    initializers.resize(size);
}

//---------------------------------------------------------------------------//
} // namespace celeritas
