//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTracks.cu
//---------------------------------------------------------------------------//
#include "InitializeTracks.hh"

#include <thrust/device_ptr.h>
#include <thrust/reduce.h>
#include <thrust/remove.h>
#include <thrust/scan.h>
#include <vector>
#include "base/Atomics.hh"
#include "base/DeviceVector.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
// KERNELS
//---------------------------------------------------------------------------//
/*!
 * Initialize the track states on device. The track initializers are created
 * from either primary particles or secondaries. The new tracks are inserted
 * into empty slots (vacancies) in the track vector.
 *
 * TODO: Add sim states
 */
__global__ void
initialize_tracks_kernel(const span<const size_type>  vacancies,
                         const span<TrackInitializer> initializers,
                         const ParticleParamsPointers pparams,
                         const ParticleStatePointers  pstates,
                         const GeoParamsPointers      gparams,
                         const GeoStatePointers       gstates)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < vacancies.size())
    {
        // Get the track initializer, starting from the back of the vector
        size_type               init_id = initializers.size() - thread_id - 1;
        const TrackInitializer& init    = initializers[init_id];

        // Index of the empty slot to create the new track in
        size_type slot_id = vacancies[thread_id];

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
__global__ void find_vacancies_kernel(span<size_type>         vacancies,
                                      span<const Interaction> interactions)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < interactions.size())
    {
        const Interaction& result = interactions[thread_id];

        // Determine which indices in the track states are available for
        // initializing new particles
        if (action_killed(result.action))
        {
            vacancies[thread_id] = thread_id;
        }
        else
        {
            // Flag as a track that's still alive
            vacancies[thread_id] = occupied_flag();
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Count the number of secondaries that survived cutoffs for each interaction.
 */
__global__ void count_secondaries_kernel(size_type* secondary_count,
                                         span<const Interaction> interactions)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < interactions.size())
    {
        secondary_count[thread_id] = 0;
        const Interaction& result = interactions[thread_id];

        // Count how many secondaries survived cutoffs for each track
        for (const auto& secondary : result.secondaries)
        {
            if (secondary.energy.value() > 0)
            {
                ++secondary_count[thread_id];
            }
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers on device from primary particles.
 */
__global__ void
create_from_primaries_kernel(span<const Primary>    primaries,
                             span<TrackInitializer> initializers)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < primaries.size())
    {
        size_type         offset_id = initializers.size();
        TrackInitializer& init      = initializers[offset_id + thread_id];

        // Create a new track initializer from a primary particle
        init = primaries[thread_id];
    }
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers on device from secondary particles.
 */
__global__ void
create_from_secondaries_kernel(size_type*              cum_secondaries,
                               span<const Interaction> interactions,
                               span<TrackInitializer>  initializers)
{
    auto thread_id = KernelParamCalculator::thread_id().get();
    if (thread_id < interactions.size())
    {
        const Interaction& result = interactions[thread_id];

        // Starting index in the vector of track initializers
        size_type index = cum_secondaries[thread_id];

        for (const auto& secondary : result.secondaries)
        {
            // If the secondary survived cutoffs
            if (secondary.energy.value() > 0)
            {
                TrackInitializer& init = initializers[index++];

                // Create a new track initializer from a secondary
                init = secondary;
            }
        }
    }
}

//---------------------------------------------------------------------------//
// KERNEL INTERFACE
//---------------------------------------------------------------------------//
/*!
 * Initialize the track states on device.
 */
void initialize_tracks(VacancyStore&          vacancies,
                       TrackInitializerStore& initializers,
                       const ParticleParamsPointers pparams,
                       const ParticleStatePointers  pstates,
                       const GeoParamsPointers      gparams,
                       const GeoStatePointers       gstates)
{
    // The number of new tracks to initialize is the smaller of the number of
    // empty slots in the track vector and the number of track initializers
    size_type num_tracks = std::min(vacancies.size(), initializers.size());
    vacancies.resize(num_tracks);

    // Initialize tracks on device
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(num_tracks);
    initialize_tracks_kernel<<<params.grid_size, params.block_size>>>(
        vacancies.device_pointers(),
        initializers.device_pointers(),
        pparams,
        pstates,
        gparams,
        gstates);

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Resize the vector of track initializers
    initializers.resize(initializers.size() - num_tracks);
}

//---------------------------------------------------------------------------//
/*!
 * Find empty slots in the vector of tracks
 */
void find_vacancies(VacancyStore&           vacancies,
                    span<const Interaction> interactions)
{
    // Resize the vector of vacancies to be equal to the number of tracks
    size_type num_tracks = interactions.size();
    vacancies.resize(num_tracks);

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(num_tracks);
    find_vacancies_kernel<<<params.grid_size, params.block_size>>>(
        vacancies.device_pointers(), interactions);

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Remove all the elements in the vacancy vector that were flagged as
    // active tracks, so we are left with a vector containing the (sorted)
    // indices of the empty slots
    thrust::device_ptr<size_type> end = thrust::remove_if(
        thrust::device_pointer_cast(vacancies.device_pointers().data()),
        thrust::device_pointer_cast(vacancies.device_pointers().data()
                                    + vacancies.size()),
        is_not_vacant(occupied_flag()));

    // Resize the vector of vacancies to be equal to the number of empty slots
    vacancies.resize(thrust::raw_pointer_cast(end)
                     - vacancies.device_pointers().data());
}

//---------------------------------------------------------------------------//
/*!
 * Count the number of secondaries that survived cutoffs for each interaction.
 */
void count_secondaries(span<size_type>         secondary_count,
                       span<const Interaction> interactions)
{
    REQUIRE(interactions.size() == secondary_count.size());

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(interactions.size());
    count_secondaries_kernel<<<params.grid_size, params.block_size>>>(
        secondary_count.data(), interactions);

    CELER_CUDA_CALL(cudaDeviceSynchronize());
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers from primary particles
 */
void create_from_primaries(span<const Primary>    primaries,
                           TrackInitializerStore& initializers)
{
    REQUIRE(primaries.size() <= initializers.capacity() - initializers.size());

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(primaries.size());
    create_from_primaries_kernel<<<params.grid_size, params.block_size>>>(
        primaries, initializers.device_pointers());

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Resize the vector of track initializers
    initializers.resize(initializers.size() + primaries.size());
}

//---------------------------------------------------------------------------//
/*!
 * Create track initializers from secondary particles
 */
void create_from_secondaries(span<size_type>         secondary_count,
                             span<const Interaction> interactions,
                             TrackInitializerStore&  initializers)
{
    REQUIRE(secondary_count.size() == interactions.size());

    // Sum the total number secondaries produced in all interactions
    size_type num_secondaries
        = thrust::reduce(thrust::device_pointer_cast(secondary_count.data()),
                         thrust::device_pointer_cast(secondary_count.data())
                             + secondary_count.size(),
                         0,
                         thrust::plus<size_type>());

    REQUIRE(num_secondaries <= initializers.capacity() - initializers.size());

    // The exclusive prefix sum of the number of secondaries produced in each
    // interaction is used to get the starting index in the vector of track
    // initializers for creating initializers from secondaries from an
    // interaction
    thrust::exclusive_scan(thrust::device_pointer_cast(secondary_count.data()),
                           thrust::device_pointer_cast(secondary_count.data())
                               + secondary_count.size(),
                           secondary_count.data(),
                           0);

    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(interactions.size());
    create_from_secondaries_kernel<<<params.grid_size, params.block_size>>>(
        secondary_count.data(), interactions, initializers.device_pointers());

    CELER_CUDA_CALL(cudaDeviceSynchronize());

    // Resize the vector of track initializers
    initializers.resize(initializers.size() + num_secondaries);
}

//---------------------------------------------------------------------------//
} // namespace celeritas
