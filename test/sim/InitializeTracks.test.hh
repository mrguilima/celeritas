//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTracks.test.hh
//---------------------------------------------------------------------------//
#include "physics/base/Interaction.hh"
#include "physics/base/SecondaryAllocatorPointers.hh"
#include "physics/base/SecondaryAllocatorView.hh"
#include "physics/base/ParticleParams.hh"
#include "physics/base/ParticleStateStore.hh"
#include "physics/base/ParticleTrackView.hh"
#include "geometry/GeoParamsPointers.hh"
#include "geometry/GeoStatePointers.hh"
#include "sim/TrackInitializerStore.hh"
#include "sim/VacancyStore.hh"

namespace celeritas_test
{
using namespace celeritas;

//---------------------------------------------------------------------------//
// TESTING INTERFACE
//---------------------------------------------------------------------------//
//! Interactor
struct Interactor
{
    __device__ Interactor(const ParticleTrackView& particle,
                          SecondaryAllocatorView&  allocate_secondaries)
        : energy(particle.energy()), allocate_secondaries(allocate_secondaries)
    {
    }

    __device__ Interaction operator()()
    {
        // Create secondary particles
        unsigned long int alloc_size = 4;
        Secondary*        allocated  = this->allocate_secondaries(alloc_size);
        if (!allocated)
        {
            return Interaction::from_failure();
        }
        Interaction result;

        // Kill some tracks to create vacancies in the track vector
        if (static_cast<int>(energy) % 3)
        {
            result.action = Action::scattered;
        }
        else
        {
            result.action = Action::absorbed;
        }

        // Initialize secondaries, including one with zero energy to account
        // for secondaries being pruned when applying cutoffs
        result.secondaries = {allocated, alloc_size};
        for (unsigned long int i = 0; i < alloc_size; ++i)
        {
            result.secondaries[i].def_id = ParticleDefId(0);
            result.secondaries[i].energy = i * energy / 100;
        }
        return result;
    }

    double                  energy;
    SecondaryAllocatorView& allocate_secondaries;
};

//---------------------------------------------------------------------------//
//! Launch a kernel to set the indices of the empty slots in the vector of
//! track states. At the start of the simulation they are all empty.
void initialize_vacancies(size_type num_tracks, VacancyStore& vacancies);

//---------------------------------------------------------------------------//
//! Launch a kernel to process interactions
void interact(size_type                  num_tracks,
              ParticleParamsPointers     pparams,
              ParticleStatePointers      pstates,
              SecondaryAllocatorPointers secondaries,
              span<Interaction>          interactions);

//---------------------------------------------------------------------------//
//! Launch a kernel to get the energies of the initialized tracks
std::vector<double> tracks_test(size_type              num_tracks,
                                ParticleParamsPointers pparams,
                                ParticleStatePointers  pstates);

//---------------------------------------------------------------------------//
//! Launch a kernel to get the energies of the track initializers created from
//! primaries or secondaries
std::vector<double> initializers_test(TrackInitializerStore& initializers);

//---------------------------------------------------------------------------//
//! Launch a kernel to get the indices of the vacant slots in the track vector
std::vector<size_type> vacancies_test(VacancyStore& vacancies);

//---------------------------------------------------------------------------//
} // namespace celeritas_test
