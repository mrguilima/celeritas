//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInitializer.hh
//---------------------------------------------------------------------------//
#pragma once

#include "Types.hh"
#include "physics/base/Interaction.hh"
#include "physics/base/Primary.hh"
#include "physics/base/Secondary.hh"
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
 * Lightweight version of a track used to initialize new tracks from primaries
 * or secondaries
 */
struct TrackInitializer
{
    // Simulation state
    SimTrackView::Initializer_t      sim;
    GeoTrackView::Initializer_t      geo;
    ParticleTrackView::Initializer_t particle;

    // Initialize from a primary particle
    CELER_FUNCTION TrackInitializer& operator=(const Primary& primary)
    {
        particle.def_id = primary.def_id;
        particle.energy = primary.energy;
        geo.dir         = primary.direction;
        geo.pos         = primary.position;
        sim.event_id    = primary.event_id;
        return *this;
    }

    // Initialize from a secondary particle
    CELER_FUNCTION TrackInitializer& operator=(const Secondary& secondary)
    {
        particle.def_id = secondary.def_id;
        particle.energy = secondary.energy;
        geo.dir         = secondary.direction;
        // TODO: Get the position and event id from somewhere
        // geo.pos         = secondary.position;
        // sim.event_id    = secondary.event_id;
        return *this;
    }
};

//---------------------------------------------------------------------------//
} // namespace celeritas
