//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInit.hh
//---------------------------------------------------------------------------//
#pragma once

#include "TrackInitializer.hh"
#include "TrackInitializerStore.hh"
#include "base/Span.hh"
#include "base/KernelParamCalculator.cuda.hh"
#include "physics/base/Interaction.hh"
#include "physics/base/Primary.hh"
#include "physics/base/ParticleStatePointers.hh"
#include "physics/base/ParticleParamsPointers.hh"
#include "geometry/GeoStatePointers.hh"
#include "geometry/GeoParamsPointers.hh"
#include "thrust/device_vector.h"

namespace celeritas
{
using thrust::device_vector;

//---------------------------------------------------------------------------//
// Initialize the track states on device.
void initialize_tracks(device_vector<unsigned long long int>& vacancies,
                       TrackInitializerStore&                 storage,
                       // const SimParamsPointers                sparams,
                       // const SimStatePointers                 sstates,
                       const ParticleParamsPointers pparams,
                       const ParticleStatePointers  pstates,
                       const GeoParamsPointers      gparams,
                       const GeoStatePointers       gstates);

//---------------------------------------------------------------------------//
// Find empty slots in the vector of track states
void find_vacancies(span<const Interaction>                interactions,
                    device_vector<unsigned long long int>& num_vacancies,
                    device_vector<unsigned long long int>& vacancies);

//---------------------------------------------------------------------------//
// Count the number of secondaries that survived cutoffs for each interaction.
void count_secondaries(span<const Interaction>   interactions,
                       device_vector<size_type>& num_secondaries);

//---------------------------------------------------------------------------//
// Create track initializers on device from primary particles
void primary_initializers(span<const Primary>    primaries,
                          TrackInitializerStore& storage);

//---------------------------------------------------------------------------//
// Create track initializers on device from secondary particles
void secondary_initializers(device_vector<size_type>& num_secondaries,
                            span<const Interaction>   interactions,
                            TrackInitializerStore&    storage);

//---------------------------------------------------------------------------//
} // namespace celeritas
