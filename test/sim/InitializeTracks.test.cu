//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTracks.test.cu
//---------------------------------------------------------------------------//
#include "sim/InitializeTracks.hh"
#include "InitializeTracks.test.hh"

#include <thrust/copy.h>
#include <thrust/device_vector.h>
#include "base/KernelParamCalculator.cuda.hh"

namespace celeritas_test
{
using namespace celeritas;

//---------------------------------------------------------------------------//
// KERNELS
//---------------------------------------------------------------------------//

__global__ void
initialize_vacancies_kernel(size_type num_tracks, span<size_type> vacancies)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id().get();
    if (thread_id < num_tracks)
    {
        vacancies.data()[thread_id] = thread_id;
    }
}

__global__ void interact_kernel(size_type                  num_tracks,
                                ParticleParamsPointers     pparams,
                                ParticleStatePointers      pstates,
                                SecondaryAllocatorPointers secondaries,
                                Interaction*               interactions)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id();
    if (thread_id.get() < num_tracks)
    {
        ParticleTrackView      particle(pparams, pstates, thread_id);
        SecondaryAllocatorView allocate_secondaries(secondaries);

        // Allow the particle to interact and create secondaries
        Interactor interact(particle, allocate_secondaries);
        interactions[thread_id.get()] = interact();
    }
}

__global__ void tracks_test_kernel(size_type              num_tracks,
                                   ParticleParamsPointers pparams,
                                   ParticleStatePointers  pstates,
                                   double*                output)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id();
    if (thread_id.get() < num_tracks)
    {
        ParticleTrackView particle(pparams, pstates, thread_id);
        output[thread_id.get()] = particle.energy().value();
    }
}

__global__ void
initializers_test_kernel(span<TrackInitializer> initializers, double* output)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id().get();
    if (thread_id < initializers.size())
    {
        TrackInitializer& init = initializers.data()[thread_id];
        output[thread_id]      = init.particle.energy.value();
    }
}

__global__ void
vacancies_test_kernel(span<size_type> vacancies, size_type* output)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id().get();
    if (thread_id < vacancies.size())
    {
        output[thread_id] = vacancies.data()[thread_id];
    }
}

//---------------------------------------------------------------------------//
// TESTING INTERFACE
//---------------------------------------------------------------------------//

void initialize_vacancies(size_type num_tracks, VacancyStore& vacancies)
{
    vacancies.resize(num_tracks);
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(num_tracks);
    initialize_vacancies_kernel<<<params.grid_size, params.block_size>>>(
        num_tracks, vacancies.device_pointers());

    CELER_CUDA_CHECK_ERROR();
}

void interact(size_type                  num_tracks,
              ParticleParamsPointers     pparams,
              ParticleStatePointers      pstates,
              SecondaryAllocatorPointers secondaries,
              span<Interaction>          interactions)
{
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(num_tracks);
    interact_kernel<<<params.grid_size, params.block_size>>>(
        num_tracks, pparams, pstates, secondaries, interactions.data());

    CELER_CUDA_CHECK_ERROR();
}

std::vector<double> tracks_test(size_type              num_tracks,
                                ParticleParamsPointers pparams,
                                ParticleStatePointers  pstates)
{
    // Allocate memory for results
    thrust::device_vector<double> output(num_tracks);

    // Launch a kernel to check the properties of the initialized tracks
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(num_tracks);
    tracks_test_kernel<<<params.grid_size, params.block_size>>>(
        num_tracks, pparams, pstates, thrust::raw_pointer_cast(output.data()));

    CELER_CUDA_CHECK_ERROR();

    // Copy data back to host
    std::vector<double> host_output(num_tracks);
    thrust::copy(output.begin(), output.end(), host_output.begin());

    return host_output;
}

std::vector<double> initializers_test(TrackInitializerStore& initializers)
{
    // Allocate memory for results
    thrust::device_vector<double> output(initializers.size());

    // Launch a kernel to check the properties of the track initializers
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(initializers.size());
    initializers_test_kernel<<<params.grid_size, params.block_size>>>(
        initializers.device_pointers(),
        thrust::raw_pointer_cast(output.data()));

    CELER_CUDA_CHECK_ERROR();

    // Copy data back to host
    std::vector<double> host_output(initializers.size());
    thrust::copy(output.begin(), output.end(), host_output.begin());

    return host_output;
}

std::vector<size_type> vacancies_test(VacancyStore& vacancies)
{
    // Allocate memory for results
    thrust::device_vector<size_type> output(vacancies.size());

    // Launch a kernel to check the indices of the empty slots
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(vacancies.size());
    vacancies_test_kernel<<<params.grid_size, params.block_size>>>(
        vacancies.device_pointers(), thrust::raw_pointer_cast(output.data()));

    CELER_CUDA_CHECK_ERROR();

    // Copy data back to host
    std::vector<size_type> host_output(vacancies.size());
    thrust::copy(output.begin(), output.end(), host_output.begin());

    return host_output;
}

//---------------------------------------------------------------------------//
} // namespace celeritas_test
