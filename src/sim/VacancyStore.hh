//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file VacancyStore.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/DeviceVector.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Manage device data for vacancies.
 */
class VacancyStore
{
  public:
    //@{
    //! Type aliases
    using Span = span<size_type>;
    //@}

  public:
    // Construct with the maximum number of indices to store on device
    explicit VacancyStore(size_type capacity);

    // Get the number of elements
    size_type capacity() const { return allocation_.size(); }

    // Get the number of elements
    size_type size() const { return size_; }

    // Change the size (without changing capacity)
    void resize(size_type size);

    // Get a view to the managed data
    Span device_pointers();

  private:
    DeviceVector<size_type> allocation_;
    size_type               size_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas
