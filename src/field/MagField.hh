//---------------------------------*-CUDA-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file MagField.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Array.hh"
#include "base/Macros.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * The MagField evaluates the magnetic field value at a given position.
 */
class MagField
{
  public:
    // Construct from a uniform field
    explicit inline CELER_FUNCTION MagField(const Real3& value);

    // Return a magnetic field value at a given position
    inline CELER_FUNCTION Real3 operator()() const;

  private:
    // Shared/persistent field data
    Real3 value_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas

#include "MagField.i.hh"
