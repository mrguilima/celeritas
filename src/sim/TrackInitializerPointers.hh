//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInitializerPointers.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Macros.hh"
#include "base/Span.hh"
#include "TrackInitializer.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * View to track initializers.
 */
struct TrackInitializerPointers
{
    span<TrackInitializer> storage; // View to storage space for initializers
    size_type* size = nullptr;      // Total number of initializers stored

    // Whether the pointers are assigned
    explicit inline CELER_FUNCTION operator bool() const;
};

//---------------------------------------------------------------------------//
// INLINE FUNCTIONS
//---------------------------------------------------------------------------//
/*!
 * Check whether the pointers are assigned.
 */
CELER_FUNCTION TrackInitializerPointers::operator bool() const
{
    REQUIRE(storage.empty() || size);
    return !storage.empty();
}

//---------------------------------------------------------------------------//
} // namespace celeritas
