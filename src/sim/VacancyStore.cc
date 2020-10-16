//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file VacancyStore.cc
//---------------------------------------------------------------------------//
#include "VacancyStore.hh"

#include "base/Assert.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct with the maximum number of elements to allocate on device.
 */
VacancyStore::VacancyStore(size_type capacity)
    : allocation_(capacity), size_(0)
{
    REQUIRE(capacity > 0);
}

//---------------------------------------------------------------------------//
/*!
 * Change the size (without changing capacity)
 */
void VacancyStore::resize(size_type size)
{
    REQUIRE(size <= this->capacity());
    size_ = size;
}

//---------------------------------------------------------------------------//
/*!
 * Get a view to the managed data.
 */
auto VacancyStore::device_pointers() -> Span
{
    return {allocation_.device_pointers().data(), this->size()};
}

//---------------------------------------------------------------------------//
} // namespace celeritas
