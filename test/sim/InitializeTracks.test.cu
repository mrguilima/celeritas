//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInit.test.cu
//---------------------------------------------------------------------------//
#include "InitializeTracks.test.hh"

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/fill.h>
#include <thrust/binary_search.h>
#include <thrust/scan.h>
#include <thrust/sort.h>
#include "base/KernelParamCalculator.cuda.hh"
#include "physics/base/SecondaryAllocatorStore.hh"
#include "sim/TrackInitializerPointers.hh"
#include "sim/TrackInitializerStore.hh"
#include "sim/InitializeTracks.hh"
#include "gtest/Main.hh"
#include "gtest/Test.hh"

using namespace celeritas;

namespace celeritas_test
{
//---------------------------------------------------------------------------//
// KERNELS
//---------------------------------------------------------------------------//
__global__ void interact(ITTestInput input, Interaction* interactions)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id();
    if (thread_id.get() < input.num_threads)
    {
        ParticleTrackView particle(input.pparams, input.pstates, thread_id);
        SecondaryAllocatorView allocate(input.sa_view);

        // Allow the particle to interact and create secondaries
        Interactor interact(particle, allocate);
        interactions[thread_id.get()] = interact(thread_id.get() % 3 == 0);
    }
}

__global__ void test_tracks(ITTestInput input, double* output)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id();
    if (thread_id.get() < input.num_threads)
    {
        ParticleTrackView particle(input.pparams, input.pstates, thread_id);
        output += thread_id.get();
        *output = particle.energy();
    }
}

__global__ void
test_initializers(TrackInitializerPointers initializers, double* output)
{
    auto thread_id = celeritas::KernelParamCalculator::thread_id();
    if (thread_id.get() < *initializers.size)
    {
        TrackInitializer& init = initializers.storage[thread_id.get()];
        output += thread_id.get();
        *output = init.particle.energy;
    }
}

//---------------------------------------------------------------------------//
// TESTING INTERFACE
//---------------------------------------------------------------------------//
class TrackInitTest : public celeritas::Test
{
  protected:
    void SetUp() override
    {
        input.num_threads = 8;

        // Set up shared standard model particle data
        ParticleParams::VecAnnotatedDefs defs
            = {{{"gamma", pdg::gamma()},
                {0, 0, ParticleDef::stable_decay_constant()}}};
        particle_params = std::make_shared<ParticleParams>(std::move(defs));
        input.pparams   = particle_params->device_pointers();

        // Create primary particles
        for (unsigned long int i = 0; i < input.num_threads; ++i)
        {
            Primary p;
            p.def_id    = ParticleDefId{0};
            p.energy    = 10. * (i + 1);
            p.position  = {0., 0., 0.};
            p.direction = {0., 0., 1.};
            p.event_id  = EventId{0};
            host_primaries.push_back(p);
        }
        primaries = host_primaries;

        // Set the indices of the empty slots in the vector of track states; at
        // the start they are all empty
        for (auto i = 0; i < input.num_threads; ++i)
        {
            host_vacancies.push_back(i);
        }
        vacancies = host_vacancies;

        // Allocate memory for interactions
        interactions.resize(input.num_threads);
    }

    ITTestInput                        input;
    std::shared_ptr<ParticleParams>    particle_params;
    thrust::host_vector<Primary>       host_primaries;
    thrust::device_vector<Primary>     primaries;
    thrust::host_vector<ull_int>       host_vacancies;
    thrust::device_vector<ull_int>     vacancies;
    thrust::device_vector<ull_int>     num_vacancies;
    thrust::device_vector<size_type>   num_secondaries;
    thrust::device_vector<Interaction> interactions;
};

void print_tracks(ITTestInput input)
{
    // Allocate memory for results
    thrust::device_vector<double> output(input.num_threads);

    // Launch a kernel to check the properties of the initialized tracks
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(input.num_threads);
    test_tracks<<<params.grid_size, params.block_size>>>(
        input, raw_pointer_cast(output.data()));
    CELER_CUDA_CHECK_ERROR();

    // Copy data back to host
    thrust::host_vector<double> host_output = output;
    // const double expected[] = {80., 70., 60., 50., 40., 30., 20., 10.};
    // EXPECT_VEC_SOFT_EQ(expected, host_output);

    // Print the energies of the primary particles that were initialized
    std::cout << "\nTrack energies:" << std::endl;
    for (double x : host_output)
    {
        std::cout << x << std::endl;
    }
}

void print_initializers(TrackInitializerStore& ti_store)
{
    // Allocate memory for results
    size_type                     num_init = ti_store.get_size();
    thrust::device_vector<double> output(num_init);

    // Launch another kernel to check the properties of the initialized
    // tracks
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(num_init);
    test_initializers<<<params.grid_size, params.block_size>>>(
        ti_store.device_pointers(), raw_pointer_cast(output.data()));
    CELER_CUDA_CHECK_ERROR();

    // Copy data back to host
    thrust::host_vector<double> host_output = output;

    // Print the energies of the secondaries that will be used to initialize
    // new tracks
    std::cout << "\nInitializer energies:" << std::endl;
    for (auto x : host_output)
    {
        std::cout << x << std::endl;
    }
}

TEST_F(TrackInitTest, run)
{
    using thrust::raw_pointer_cast;

    // Allocate memory for particle track states
    ParticleStateStore pstore(input.num_threads);
    input.pstates = pstore.device_pointers();

    // Allocate memory for secondaries
    int                     storage_size = 1024;
    SecondaryAllocatorStore sa_store(storage_size);
    input.sa_view = sa_store.device_pointers();

    // Allocate memory for track initializers
    TrackInitializerStore ti_store(storage_size);

    // Create track initializers on device from primary particles
    span<const Primary> primary_ptrs
        = {raw_pointer_cast(primaries.data()), primaries.size()};
    primary_initializers(primary_ptrs, ti_store);

    // Initialize the primary tracks on device
    initialize_tracks(vacancies,
                      ti_store,
                      input.pparams,
                      input.pstates,
                      input.gparams,
                      input.gstates);

    // Print the energy of the initialize tracks
    print_tracks(input);

    // Launch kernel to process interactions
    KernelParamCalculator calc_launch_params;
    auto                  params = calc_launch_params(input.num_threads);
    interact<<<params.grid_size, params.block_size>>>(
        input, raw_pointer_cast(interactions.data()));
    CELER_CUDA_CHECK_ERROR();

    // Launch kernel to find the indices of the empty slots in track vector
    span<const Interaction> interaction_ptrs
        = {raw_pointer_cast(interactions.data()), interactions.size()};
    find_vacancies(interaction_ptrs, num_vacancies, vacancies);

    // Launch kernel to count the number of secondaries produced in each
    // interaction
    count_secondaries(interaction_ptrs, num_secondaries);

    // Launch a kernel to create track initializers from
    // interactions/secondaries
    secondary_initializers(num_secondaries, interaction_ptrs, ti_store);

    // Print the energies of the track initializers created from secondaries
    print_initializers(ti_store);

    // Initialize secondaries on device
    initialize_tracks(vacancies,
                      ti_store,
                      input.pparams,
                      input.pstates,
                      input.gparams,
                      input.gstates);

    // Print the energies of the newly initialized tracks
    print_tracks(input);
}

//---------------------------------------------------------------------------//
} // namespace celeritas_test
