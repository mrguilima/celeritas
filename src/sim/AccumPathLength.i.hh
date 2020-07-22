//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file AccumPathLength.i.hh
//---------------------------------------------------------------------------//
#include <limits>
#include "base/Assert.hh"
#include "base/NumericLimits.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct a path length at the beginning of a particle lifetime.
 */
CELER_CONSTEXPR_FUNCTION AccumPathLength AccumPathLength::zero()
{
    AccumPathLength result;
    result.distance_ = 0;
    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Construct a path length longer than any other possible path length.
 */
CELER_CONSTEXPR_FUNCTION AccumPathLength AccumPathLength::infinity()
{
    AccumPathLength result;
    result.distance_ = numeric_limits<real_type>::infinity();
    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Construct with invalid path length.
 */
CELER_CONSTEXPR_FUNCTION AccumPathLength::AccumPathLength() : distance_(-1) {}

//---------------------------------------------------------------------------//
/*!
 * Whether the path length is valid.
 */
CELER_FORCEINLINE_FUNCTION AccumPathLength::operator bool() const
{
    return distance_ >= 0;
}

//---------------------------------------------------------------------------//
/*!
 * Get the distance traveled since another cumulative path length.
 */
CELER_FORCEINLINE_FUNCTION real_type
AccumPathLength::operator-(AccumPathLength other) const
{
    REQUIRE(*this && other);
    return this->distance_ - other.distance_;
}

//---------------------------------------------------------------------------//
/*!
 * Get the path length after a distance traveled.
 */
CELER_FORCEINLINE_FUNCTION AccumPathLength
AccumPathLength::operator+(real_type distance) const
{
    REQUIRE(*this);
    REQUIRE(distance >= 0);
    AccumPathLength result(*this);
    result.distance_ += distance;
    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Add to the path length.
 */
CELER_FORCEINLINE_FUNCTION AccumPathLength&
AccumPathLength::operator+=(real_type distance)
{
    REQUIRE(*this);
    REQUIRE(distance >= 0);
    this->distance_ += distance;
    return *this;
}

//---------------------------------------------------------------------------//
// COMPARISON FUNCTIONS
//
// Both values should be valid in order to test this. This avoids the ambiguity
// of behaving as NaN or not.
//---------------------------------------------------------------------------//

CELER_FORCEINLINE_FUNCTION bool
AccumPathLength::operator==(AccumPathLength other) const
{
    REQUIRE(*this && other);
    return (distance_ == other.distance_);
}
CELER_FORCEINLINE_FUNCTION bool
AccumPathLength::operator!=(AccumPathLength other) const
{
    REQUIRE(*this && other);
    return (distance_ != other.distance_);
}
CELER_FORCEINLINE_FUNCTION bool
AccumPathLength::operator>(AccumPathLength other) const
{
    REQUIRE(*this && other);
    return (distance_ > other.distance_);
}
CELER_FORCEINLINE_FUNCTION bool
AccumPathLength::operator>=(AccumPathLength other) const
{
    REQUIRE(*this && other);
    return (distance_ >= other.distance_);
}
CELER_FORCEINLINE_FUNCTION bool
AccumPathLength::operator<(AccumPathLength other) const
{
    REQUIRE(*this && other);
    return (distance_ < other.distance_);
}
CELER_FORCEINLINE_FUNCTION bool
AccumPathLength::operator<=(AccumPathLength other) const
{
    REQUIRE(*this && other);
    return (distance_ <= other.distance_);
}

//---------------------------------------------------------------------------//
} // namespace celeritas
