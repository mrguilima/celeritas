//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file AccumPathLength.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Macros.hh"
#include "base/Types.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Cumulative distance traveled by a particle in its lifetime.
 *
 * If the core transport algorithm is interpreted as a discrete event
 * simulation (DES), then the cumulative path length traveled by the particle
 * is equivalent to the total simulated time in the DES simulation. It's always
 * increasing, and each proposed 'next event' being scheduled will be greater
 * than the current cumulative path length.
 *
 * This class will be used by tallies: they store the path length whenever a
 * particle enters a phase space region of interest, then when it leaves (by
 * changing energy, space, etc.) the total path length is the starting length
 * subtracted from the new cumulative path length.
 *
 * The class is also used by event interfaces: the "next event" for a class is
 * the calculated interaction distance added to the particle's current
 * cumulative path length. If the interaction distance is invalidated, the
 * next-event value is set to AccumPath().
 *
 * The lifetime path length of a particle is strictly nonnegative, and it only
 * is zero at birth. Since many arithmetic operations don't make sense with
 * path length (why would you multiply two of them?) we don't define them.
 */
class AccumPathLength
{
  public:
    // >>> CONSTRUCTORS

    // Construct a path length at the beginning of a particle lifetime
    static CELER_CONSTEXPR_FUNCTION AccumPathLength zero();

    // Construct a path length longer than any other possible path length
    static CELER_CONSTEXPR_FUNCTION AccumPathLength infinity();

    // Construct with invalid path length
    CELER_CONSTEXPR_FUNCTION AccumPathLength();

    // >>> OPERATORS

    // Whether the path length is valid
    CELER_FORCEINLINE_FUNCTION explicit operator bool() const;

    // Get the distance traveled since another cumulative path length.
    CELER_FORCEINLINE_FUNCTION real_type operator-(AccumPathLength other) const;

    // Get the path length after a distance traveled.
    CELER_FORCEINLINE_FUNCTION AccumPathLength
    operator+(real_type distance) const;

    // Add to the path length
    CELER_FORCEINLINE_FUNCTION AccumPathLength& operator+=(real_type distance);

    // >>> ORDERING

    //@{
    //! Comparison functions
    CELER_FORCEINLINE_FUNCTION bool operator==(AccumPathLength other) const;
    CELER_FORCEINLINE_FUNCTION bool operator!=(AccumPathLength other) const;
    CELER_FORCEINLINE_FUNCTION bool operator>(AccumPathLength other) const;
    CELER_FORCEINLINE_FUNCTION bool operator>=(AccumPathLength other) const;
    CELER_FORCEINLINE_FUNCTION bool operator<(AccumPathLength other) const;
    CELER_FORCEINLINE_FUNCTION bool operator<=(AccumPathLength other) const;
    //@}

  private:
    // >>> DATA

    real_type distance_;
};

//! Symmetric addition operator
CELER_FORCEINLINE_FUNCTION AccumPathLength operator+(real_type       lhs,
                                                     AccumPathLength rhs)
{
    return rhs + lhs;
}

//---------------------------------------------------------------------------//
} // namespace celeritas

#include "AccumPathLength.i.hh"
