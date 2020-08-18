//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file Primary.hh
//---------------------------------------------------------------------------//
#pragma once

#include "Types.hh"
// XXX Consider moving initializer_t to a traits class for each "state" thing
#include "physics/base/ParticleTrackView.hh"
#include "geometry/GeoTrackView.hh"

namespace celeritas
{
// XXX temporary, move to simstate
struct SimTrackView
{
    struct Initializer_t
    {
        EventId event_id;
        TrackId track_id;
    };
};

//---------------------------------------------------------------------------//
/*!
 * Starting "source" particle. One or more of these are emitted by an Event.
 */
struct Primary
{
    // Simulation state
    SimTrackView::Initializer_t      sim;
    GeoTrackView::Initializer_t      geo;
    ParticleTrackView::Initializer_t particle;
};

//---------------------------------------------------------------------------//
} // namespace celeritas

//---------------------------------------------------------------------------//
