//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InitializeTracks.hh
//---------------------------------------------------------------------------//
#pragma once

#include "TrackInitializer.hh"
#include "TrackInitializerStore.hh"
#include "VacancyStore.hh"
#include "base/KernelParamCalculator.cuda.hh"
#include "base/Span.hh"
#include "physics/base/Interaction.hh"
#include "physics/base/Primary.hh"
#include "physics/base/ParticleStatePointers.hh"
#include "physics/base/ParticleParamsPointers.hh"
#include "geometry/GeoStatePointers.hh"
#include "geometry/GeoParamsPointers.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
// Predicate used to check whether the track at a given index in the track
// vector is alive
struct is_not_vacant
{
    size_type flag;

    is_not_vacant(size_type flag) : flag(flag){};

    CELER_FUNCTION bool operator()(const size_type x) { return x == flag; }
};

//---------------------------------------------------------------------------//
// Mark a track state as not vacant, i.e., not available to initialize a new
// track in
CELER_CONSTEXPR_FUNCTION size_type occupied_flag()
{
    return numeric_limits<size_type>::max();
}

//---------------------------------------------------------------------------//
// Initialize the track states on device.
void initialize_tracks(VacancyStore&          vacancies,
                       TrackInitializerStore& initializers,
                       const ParticleParamsPointers pparams,
                       const ParticleStatePointers  pstates,
                       const GeoParamsPointers      gparams,
                       const GeoStatePointers       gstates);

//---------------------------------------------------------------------------//
// Find empty slots in the vector of track states
void find_vacancies(VacancyStore&           vacancies,
                    span<const Interaction> interactions);
//---------------------------------------------------------------------------//
// Count the number of secondaries that survived cutoffs for each interaction.
void count_secondaries(span<size_type>         secondary_count,
                       span<const Interaction> interactions);

//---------------------------------------------------------------------------//
// Create track initializers on device from primary particles
void create_from_primaries(span<const Primary>    primaries,
                           TrackInitializerStore& initializers);

//---------------------------------------------------------------------------//
// Create track initializers on device from secondary particles
void create_from_secondaries(span<size_type>         secondary_count,
                             span<const Interaction> interactions,
                             TrackInitializerStore&  initializers);

//---------------------------------------------------------------------------//
} // namespace celeritas
