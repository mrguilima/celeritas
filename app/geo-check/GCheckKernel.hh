//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GCheckKernel.hh
//---------------------------------------------------------------------------//
#pragma once

#include "geometry/GeoInterface.hh"
#include "geometry/GeoParams.hh"

namespace geo_check
{
//---------------------------------------------------------------------------//
using celeritas::GeoTrackInitializer;
using celeritas::MemSpace;
using celeritas::Ownership;

using GeoParamsCRefHost
    = celeritas::GeoParamsData<Ownership::const_reference, MemSpace::host>;
using GeoParamsCRefDevice
    = celeritas::GeoParamsData<Ownership::const_reference, MemSpace::device>;

using GeoStateRefDevice
    = celeritas::GeoStateData<Ownership::reference, MemSpace::device>;

using SPConstGeo = std::shared_ptr<const celeritas::GeoParams>;

//! Input struct
struct GCheckInput
{
    std::vector<celeritas::GeoTrackInitializer> init;
    int                                         max_steps = 0;
    GeoParamsCRefDevice                         params;
    GeoStateRefDevice                           state;
};

//! Run tracking on the CPU
void run_cpu(const SPConstGeo&          geo_params,
             const GeoTrackInitializer* track_init,
             int                        max_steps);

//! Run tracking on the GPU
void run_gpu(GCheckInput init);

#if !CELERITAS_USE_CUDA
inline void run_gpu(GCheckInput)
{
    CELER_NOT_CONFIGURED("CUDA");
}
#endif

//---------------------------------------------------------------------------//
} // namespace geo_check
