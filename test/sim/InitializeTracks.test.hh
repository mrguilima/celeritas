//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInit.test.hh
//---------------------------------------------------------------------------//
#include "physics/base/Interaction.hh"
#include "physics/base/SecondaryAllocatorPointers.hh"
#include "physics/base/SecondaryAllocatorView.hh"
#include "physics/base/ParticleParams.hh"
#include "physics/base/ParticleStateStore.hh"
#include "physics/base/ParticleTrackView.hh"
#include "geometry/GeoParamsPointers.hh"
#include "geometry/GeoStatePointers.hh"

using namespace celeritas;

namespace celeritas_test
{
//---------------------------------------------------------------------------//
// TESTING INTERFACE
//---------------------------------------------------------------------------//
//! Interactor
struct Interactor
{
    __device__ Interactor(const ParticleTrackView& particle,
                          SecondaryAllocatorView&  allocate)
        : inc_energy(particle.energy()), allocate(allocate)
    {
    }

    __device__ Interaction operator()(bool kill)
    {
        // Create secondary particles
        int        alloc_size = 4;
        Secondary* allocated  = this->allocate(alloc_size);
        if (!allocated)
        {
            return Interaction::from_failure();
        }
        Interaction result;
        result.action = kill ? Action::absorbed : Action::scattered;

        // Initialize secondaries, including some with zero energy that will be
        // pruned when applying cutoffs
        result.secondaries = {allocated, alloc_size};
        for (int i = 0; i < alloc_size; ++i)
        {
            result.secondaries[i].def_id = ParticleDefId(0);
            result.secondaries[i].energy = inc_energy - inc_energy / (i + 1);
        }
        return result;
    }

    SecondaryAllocatorView& allocate;
    double                  inc_energy;
};

//---------------------------------------------------------------------------//
//! Input data
struct ITTestInput
{
    int                        num_threads;
    SecondaryAllocatorPointers sa_view;
    ParticleParamsPointers     pparams;
    ParticleStatePointers      pstates;
    GeoParamsPointers          gparams;
    GeoStatePointers           gstates;
    // SimParamsPointers          sparams;
    // SimStatePointers           sstates;
};

//---------------------------------------------------------------------------//
} // namespace celeritas_test
