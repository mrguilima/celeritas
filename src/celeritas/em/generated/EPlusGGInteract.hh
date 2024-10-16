//----------------------------------*-C++-*----------------------------------//
// Copyright 2021-2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/em/generated/EPlusGGInteract.hh
//! \note Auto-generated by gen-interactor.py: DO NOT MODIFY!
//---------------------------------------------------------------------------//
#pragma once

#include "corecel/Assert.hh"
#include "corecel/Macros.hh"
#include "corecel/Types.hh"
#include "celeritas/em/data/EPlusGGData.hh" // IWYU pragma: associated
#include "celeritas/global/CoreTrackData.hh"

namespace celeritas
{
namespace generated
{
void eplusgg_interact(
    celeritas::EPlusGGHostRef const&,
    celeritas::CoreRef<celeritas::MemSpace::host> const&);

void eplusgg_interact(
    celeritas::EPlusGGDeviceRef const&,
    celeritas::CoreRef<celeritas::MemSpace::device> const&);

#if !CELER_USE_DEVICE
inline void eplusgg_interact(
    celeritas::EPlusGGDeviceRef const&,
    celeritas::CoreRef<celeritas::MemSpace::device> const&)
{
    CELER_ASSERT_UNREACHABLE();
}
#endif

}  // namespace generated
}  // namespace celeritas
