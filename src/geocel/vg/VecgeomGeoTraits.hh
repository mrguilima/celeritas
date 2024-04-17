//----------------------------------*-C++-*----------------------------------//
// Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file geocel/vg/VecgeomGeoTraits.hh
//---------------------------------------------------------------------------//
#pragma once

#include "geocel/GeoTraits.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
class VecgeomParams;
class VecgeomTrackView;
template<Ownership W, MemSpace M>
struct VecgeomParamsData;
template<Ownership W, MemSpace M>
struct VecgeomStateData;

//---------------------------------------------------------------------------//
/*!
 * Traits specialization for VecGeom geometry.
 */
template<>
struct GeoTraits<VecgeomParams>
{
    //! Params data used during runtime
    template<Ownership W, MemSpace M>
    using ParamsData = VecgeomParamsData<W, M>;

    //! State data used during runtime
    template<Ownership W, MemSpace M>
    using StateData = VecgeomStateData<W, M>;

    //! Geometry track view
    using TrackView = VecgeomTrackView;

    //! Descriptive name for the geometry
    static inline char const* name = "VecGeom";

    //! TO BE REMOVED: "native" file extension for this geometry
    static inline char const* ext = ".gdml";
};

//---------------------------------------------------------------------------//
}  // namespace celeritas
