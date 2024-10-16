//---------------------------------*-C++-*-----------------------------------//
// Copyright 2020-2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/ext/detail/VecgeomCompatibility.hh
//---------------------------------------------------------------------------//
#pragma once

#include <VecGeom/base/Vector3D.h>

#include "corecel/Macros.hh"
#include "corecel/cont/Array.hh"
#include "corecel/cont/Span.hh"

namespace celeritas
{
namespace detail
{
//---------------------------------------------------------------------------//
/*!
 * Copy a length-3 span into a Vector3D
 */
template<class T>
CELER_FUNCTION inline auto to_vector(Span<T, 3> s)
    -> vecgeom::Vector3D<std::remove_cv_t<T>>
{
    return {s[0], s[1], s[2]};
}

//---------------------------------------------------------------------------//
// Copy a length-3 array into a Vector3D
template<class T>
CELER_FUNCTION inline auto to_vector(Array<T, 3> const& arr)
    -> vecgeom::Vector3D<T>
{
    return to_vector(celeritas::make_span<T, 3>(arr));
}

//---------------------------------------------------------------------------//
}  // namespace detail
}  // namespace celeritas
