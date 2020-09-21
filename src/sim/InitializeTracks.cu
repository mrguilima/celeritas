//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTracks.cu
//---------------------------------------------------------------------------//
#include "InitializeTracks.hh"

#include "TrackInitializerPointers.hh"
#include "base/Atomics.hh"
#include "thrust/host_vector.h"
#include "thrust/scan.h"
#include "thrust/sort.h"
#include "thrust/fill.h"

namespace celeritas
{
//---------------------------------------------------------------------------//
// KERNELS
//---------------------------------------------------------------------------//
/*!
 * Initialize the track states on device. The track initializers are created
 * from either primary particles or secondaries. The new tracks are inserted
 * into empty slots (vacancies) in the track vector.
 */
__global__ void
initialize_tracks_kernel(size_type                      num_tracks,
                         const unsigned long long int*  vacancies,
                         const TrackInitializerPointers initializers,
                         // const SimParamsPointers        sparams,
                         // const SimStatePointers         sstates,
                         const ParticleParamsPointers pparams,
                         const ParticleStatePointers  pstates,
                         const GeoParamsPointers      gparams,
                         const GeoStatePointers       gstates)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < num_tracks)
    {
        // Get the track initializer, starting from the back of the vector
        size_type               init_id = *initializers.size - thread_id - 1;
        const TrackInitializer& init    = initializers.storage[init_id];

        // Index of the empty slot to create the new track in
        size_type slot_id = vacancies[thread_id];

        // Initialize sim state
        // SimTrackView sim(sparams, sstates, ThreadId(slot_id));
        // sim = init.sim;

        // Initialize particle physics data
        ParticleTrackView particle(pparams, pstates, ThreadId(slot_id));
        particle = init.particle;

        // Initialize geometry state
        // GeoTrackView geo(gparams, gstates, ThreadId(slot_id));
        // geo = init.geo;
    }
}

//---------------------------------------------------------------------------//
/*!
 * Find empty slots in the track vector.
 */
__global__ void find_vacancies_kernel(span<const Interaction> interactions,
                                      unsigned long long int* num_vacancies,
                                      unsigned long long int* vacancies)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < interactions.size())
    {
        const Interaction& result = interactions[thread_id];

        // Determine which indices in the track states are available for
        // initializing new particles
        if (action_killed(result.action))
        {
            unsigned long long int index = atomic_add(num_vacancies, 1ull);
            vacancies[index]             = thread_id;
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Count the number of secondaries that survived cutoffs for each interaction.
 */
__global__ void count_secondaries_kernel(span<const Interaction> interactions,
                                         size_type* num_secondaries)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < interactions.size())
    {
        const Interaction& result = interactions[thread_id];

        // Count how many secondaries survived cutoffs for each track
        for (auto secondary : result.secondaries)
        {
            if (secondary.energy > 0)
            {
                ++num_secondaries[thread_id];
            }
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers on device from primary particles.
 */
__global__ void
primary_initializers_kernel(span<const Primary>      primaries,
                            TrackInitializerPointers initializers)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < primaries.size())
    {
        size_type         offset_id = *initializers.size;
        TrackInitializer& init = initializers.storage[offset_id + thread_id];

        // Create a new track initializer from a primary particle
        init = primaries[thread_id];
    }
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers on device from secondary particles.
 */
__global__ void
secondary_initializers_kernel(size_type*               cum_secondaries,
                              span<const Interaction>  interactions,
                              TrackInitializerPointers initializers)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < interactions.size())
    {
        const Interaction& result = interactions[thread_id];

        // Starting index in the vector of track initializers
        size_type index = cum_secondaries[thread_id];

        for (auto secondary : result.secondaries)
        {
            // If the secondary survived cutoffs
            if (secondary.energy > 0)
            {
                // TODO: right now only copying energy
                TrackInitializer& init = initializers.storage[index];
                init.particle.energy   = secondary.energy;
                ++index;
            }
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers on device from primary particles.
 */
//---------------------------------------------------------------------------//
// KERNEL INTERFACE
//---------------------------------------------------------------------------//
/*!
 * Initialize the track states on device.
 */
void initialize_tracks(device_vector<unsigned long long int>& vacancies,
                       TrackInitializerStore&                 storage,
                       // const SimParamsPointers                sparams,
                       // const SimStatePointers                 sstates,
                       const ParticleParamsPointers pparams,
                       const ParticleStatePointers  pstates,
                       const GeoParamsPointers      gparams,
                       const GeoStatePointers       gstates)
{
    // Resize vacancy vector to the number of tracks to be inserted
    size_type count = std::min(vacancies.size(), storage.get_size());
    vacancies.resize(count);

    // Initialize tracks on device
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(count);
    initialize_tracks_kernel<<<params.grid_size, params.block_size>>>(
        count,
        thrust::raw_pointer_cast(vacancies.data()),
        storage.device_pointers(),
        // sparams,
        // sstates,
        pparams,
        pstates,
        gparams,
        gstates);

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Resize the vector of track initializers
    storage.resize(storage.get_size() - count);
}

//---------------------------------------------------------------------------//
/*!
 * Find empty slots in the vector of tracks
 */
void find_vacancies(span<const Interaction>                interactions,
                    device_vector<unsigned long long int>& num_vacancies,
                    device_vector<unsigned long long int>& vacancies)
{
    vacancies.resize(interactions.size());
    num_vacancies.resize(1);
    thrust::fill(num_vacancies.begin(), num_vacancies.end(), 0);

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(interactions.size());
    find_vacancies_kernel<<<params.grid_size, params.block_size>>>(
        interactions,
        thrust::raw_pointer_cast(num_vacancies.data()),
        thrust::raw_pointer_cast(vacancies.data()));

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Get the number of empty slots via host-device copy and resize the
    // vacancy vector
    thrust::host_vector<unsigned long long int> size = num_vacancies;
    vacancies.resize(size.front());

    // Sort the indices of the empty slots
    thrust::sort(vacancies.begin(), vacancies.end());
}

//---------------------------------------------------------------------------//
/*!
 * Count the number of secondaries that survived cutoffs for each interaction.
 */
void count_secondaries(span<const Interaction>   interactions,
                       device_vector<size_type>& num_secondaries)
{
    // Resize and reset count
    num_secondaries.resize(interactions.size());
    thrust::fill(num_secondaries.begin(), num_secondaries.end(), 0);

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(interactions.size());
    count_secondaries_kernel<<<params.grid_size, params.block_size>>>(
        interactions, thrust::raw_pointer_cast(num_secondaries.data()));

    CELER_CUDA_CALL(cudaDeviceSynchronize());
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers from primary particles
 */
void primary_initializers(span<const Primary>    primaries,
                          TrackInitializerStore& storage)
{
    REQUIRE(primaries.size() <= storage.capacity() - storage.get_size());

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(primaries.size());
    primary_initializers_kernel<<<params.grid_size, params.block_size>>>(
        primaries, storage.device_pointers());

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Resize the vector of track initializers
    storage.resize(storage.get_size() + primaries.size());
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers from secondary particles
 */
void secondary_initializers(device_vector<size_type>& num_secondaries,
                            span<const Interaction>   interactions,
                            TrackInitializerStore&    storage)
{
    REQUIRE(interactions.size() == num_secondaries.size());

    // The exclusive prefix sum of the number of secondaries produced in each
    // interaction is used to get the starting index in the vector of track
    // initializers for creating initializers from secondaries from an
    // interaction
    size_type count = num_secondaries.back();
    thrust::exclusive_scan(num_secondaries.begin(),
                           num_secondaries.end(),
                           num_secondaries.begin(),
                           0);
    count += num_secondaries.back();

    REQUIRE(count <= storage.capacity() - storage.get_size());

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(interactions.size());
    secondary_initializers_kernel<<<params.grid_size, params.block_size>>>(
        thrust::raw_pointer_cast(num_secondaries.data()),
        interactions,
        storage.device_pointers());

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Resize the vector of track initializers
    storage.resize(storage.get_size() + count);
}

//---------------------------------------------------------------------------//
} // namespace celeritas
