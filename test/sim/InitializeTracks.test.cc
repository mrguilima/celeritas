//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTracks.test.cc
//---------------------------------------------------------------------------//
#include "InitializeTracks.test.hh"

#include "physics/base/SecondaryAllocatorStore.hh"
#include "gtest/Main.hh"
#include "gtest/Test.hh"
#include "sim/InitializeTracks.hh"

namespace celeritas_test
{
using namespace celeritas;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class TrackInitTest : public celeritas::Test
{
  protected:
    void SetUp() override
    {
        using units::MevEnergy;
        auto zero = zero_quantity();

        num_tracks = 10;

        // Set up shared standard model particle data
        ParticleParams::VecAnnotatedDefs defs
            = {{{"gamma", pdg::gamma()},
                {zero, zero, ParticleDef::stable_decay_constant()}}};
        pparams = std::make_shared<ParticleParams>(std::move(defs));

        // Create primary particles
        for (size_type i = 0; i < num_tracks; ++i)
        {
            host_primaries.push_back({ParticleDefId{0},
                                      MevEnergy{100. * (i + 1)},
                                      {0., 0., 0.},
                                      {0., 0., 1.},
                                      EventId{0}});
        }
    }

    size_type                       num_tracks;
    std::shared_ptr<ParticleParams> pparams;
    GeoParamsPointers               gparams;
    GeoStatePointers                gstates;
    std::vector<Primary>            host_primaries;
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

TEST_F(TrackInitTest, run)
{
    // Allocate device data with the given size
    ParticleStateStore        pstates(num_tracks);
    DeviceVector<Primary>     primaries(num_tracks);
    DeviceVector<Interaction> interactions(num_tracks);
    DeviceVector<size_type>   secondary_count(num_tracks);

    // Allocate device data with the given capacity
    size_type               capacity = 4 * num_tracks;
    SecondaryAllocatorStore secondaries(capacity);
    TrackInitializerStore   initializers(capacity);
    VacancyStore            vacancies(num_tracks);

    // Copy host-side primaries to device
    primaries.copy_to_device(make_span(host_primaries));

    // Launch a kernel to set the indices of the empty slots in the vector of
    // track states.
    initialize_vacancies(num_tracks, vacancies);

    // Check the vacancies
    std::vector<size_type> output_vacancies   = vacancies_test(vacancies);
    std::vector<size_type> expected_vacancies = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_VEC_EQ(expected_vacancies, output_vacancies);

    // Create track initializers on device from primary particles
    create_from_primaries(primaries.device_pointers(), initializers);

    // Check the energies of the track initializers created from primaries
    std::vector<double> output_initializers = initializers_test(initializers);
    std::vector<double> expected_initializers
        = {100., 200., 300., 400., 500., 600., 700., 800., 900., 1000.};
    EXPECT_VEC_SOFT_EQ(expected_initializers, output_initializers);

    // Initialize the primary tracks on device
    initialize_tracks(vacancies,
                      initializers,
                      pparams->device_pointers(),
                      pstates.device_pointers(),
                      gparams,
                      gstates);

    // Check the energies of the initialized tracks
    std::vector<double> output_tracks = tracks_test(
        num_tracks, pparams->device_pointers(), pstates.device_pointers());
    std::vector<double> expected_tracks
        = {1000., 900., 800., 700., 600., 500., 400., 300., 200., 100.};
    EXPECT_VEC_SOFT_EQ(expected_tracks, output_tracks);

    // Launch kernel to process interactions
    interact(num_tracks,
             pparams->device_pointers(),
             pstates.device_pointers(),
             secondaries.device_pointers(),
             interactions.device_pointers());

    // Launch kernel to find the indices of the empty slots in track vector
    find_vacancies(vacancies, interactions.device_pointers());

    // Check the vacancies
    output_vacancies   = vacancies_test(vacancies);
    expected_vacancies = {1, 4, 7};
    EXPECT_VEC_EQ(expected_vacancies, output_vacancies);

    // Launch kernel to count the number of secondaries produced in each
    // interaction
    count_secondaries(secondary_count.device_pointers(),
                      interactions.device_pointers());

    // Launch a kernel to create track initializers from
    // interactions/secondaries
    create_from_secondaries(secondary_count.device_pointers(),
                            interactions.device_pointers(),
                            initializers);

    // Check the energies of the track initializers created from secondaries
    output_initializers   = initializers_test(initializers);
    expected_initializers = {10., 20., 30., 9.,  18., 27., 8.,  16., 24., 7.,
                             14., 21., 6.,  12., 18., 5.,  10., 15., 4.,  8.,
                             12., 3.,  6.,  9.,  2.,  4.,  6.,  1.,  2.,  3.};
    EXPECT_VEC_SOFT_EQ(expected_initializers, output_initializers);

    // Initialize secondaries on device
    initialize_tracks(vacancies,
                      initializers,
                      pparams->device_pointers(),
                      pstates.device_pointers(),
                      gparams,
                      gstates);

    // Check the energies of the initialized tracks
    output_tracks = tracks_test(
        num_tracks, pparams->device_pointers(), pstates.device_pointers());
    expected_tracks = {1000., 3., 800., 700., 2., 500., 400., 1., 200., 100.};
    EXPECT_VEC_SOFT_EQ(expected_tracks, output_tracks);
}

//---------------------------------------------------------------------------//
} // namespace celeritas_test
