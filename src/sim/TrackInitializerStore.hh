//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInitializerStore.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/DeviceVector.hh"
#include "TrackInitializer.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Manage device data for track initializers.
 */
class TrackInitializerStore
{
  public:
    //@{
    //! Type aliases
    using Span = span<TrackInitializer>;
    //@}

  public:
    // Construct with the maximum number of track initializers to store on
    // device
    explicit TrackInitializerStore(size_type capacity);

    // Get the number of elements
    size_type capacity() const { return allocation_.size(); }

    // Get the number of elements
    size_type size() const { return size_; }

    // Change the size (without changing capacity)
    void resize(size_type size);

    // Get a view to the managed data
    Span device_pointers();

  private:
    DeviceVector<TrackInitializer> allocation_;
    size_type                      size_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
