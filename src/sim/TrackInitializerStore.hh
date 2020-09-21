//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInitializerStore.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/DeviceVector.hh"
#include "TrackInitializerPointers.hh"
#include "TrackInitializer.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Manage device data for track initializers.
 *
 * The capacity is known by the host, but the data and size are both stored on
 * device.
 */
class TrackInitializerStore
{
  public:
    // Construct with the number of track initializers to allocate on device
    explicit TrackInitializerStore(size_type capacity);

    // >>> HOST ACCESSORS

    //! Size of the allocation
    size_type capacity() const { return allocation_.size(); }

    // Get the actual size via a device->host copy
    size_type get_size();

    // Resize the allocation (without changing the capacity)  via a
    // device->host copy
    void resize(size_type size);

    // Clear allocated data
    void clear();

    // >>> DEVICE ACCESSORS

    // Get a view to the managed data
    TrackInitializerPointers device_pointers();

  private:
    DeviceVector<TrackInitializer> allocation_;
    DeviceVector<size_type>        size_allocation_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
