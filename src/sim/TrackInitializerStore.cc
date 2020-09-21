//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TrackInitializerStore.cc
//---------------------------------------------------------------------------//
#include "TrackInitializerStore.hh"

#include "base/Assert.hh"
#include "base/Memory.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct with the number of bytes to allocate on device.
 */
TrackInitializerStore::TrackInitializerStore(size_type capacity)
    : allocation_(capacity), size_allocation_(1)
{
    REQUIRE(capacity > 0);
    this->clear();
    ENSURE(this->get_size() == 0);
}

//---------------------------------------------------------------------------//
/*!
 * Get a view to the managed data.
 */
TrackInitializerPointers TrackInitializerStore::device_pointers()
{
    REQUIRE(!allocation_.empty());
    TrackInitializerPointers view;
    view.storage = allocation_.device_pointers();
    view.size    = size_allocation_.device_pointers().data();
    return view;
}

//---------------------------------------------------------------------------//
/*!
 * Clear allocated data.
 *
 * This executes a kernel launch which simply resets the allocated size to
 * zero. It does not change the allocation itself.
 */
void TrackInitializerStore::clear()
{
    REQUIRE(!size_allocation_.empty());
    device_memset_zero(size_allocation_.device_pointers());
}

//---------------------------------------------------------------------------//
/*!
 * Use a host->device copy to obtain the currently used size.
 *
 * This will have high latency because it's a host-device copy, and it should
 * \em definitely not be used if a kernel is potentially changing the size of
 * the stored track initializers.
 */
auto TrackInitializerStore::get_size() -> size_type
{
    REQUIRE(!size_allocation_.empty());
    size_type result;
    size_allocation_.copy_to_host({&result, 1});
    return result;
}

//---------------------------------------------------------------------------//
/*!
 * Use a host->device copy to change the size.
 *
 * This will have high latency because it's a host-device copy, and it should
 * \em definitely not be used if a kernel is potentially changing the size of
 * the stored track initializers.
 */
void TrackInitializerStore::resize(size_type size)
{
    REQUIRE(!size_allocation_.empty());
    size_allocation_.copy_to_device({&size, 1});
}

//---------------------------------------------------------------------------//
} // namespace celeritas
