//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file demo-rasterizer/RDemoRunner.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>

#include "celeritas/geo/GeoMaterialParams.hh"
#include "celeritas/geo/GeoParams.hh"
#include "celeritas/global/CoreParams.hh"

#include "ImageStore.hh"

namespace demo_rasterizer
{
//---------------------------------------------------------------------------//
/*!
 * Set up and run rasterization of the given device image.
 */
class RDemoRunner
{
  public:
    //!@{
    //! Type aliases
    using SPConstGeo = std::shared_ptr<const celeritas::GeoParams>;
    using SPConstGeoMaterial
        = std::shared_ptr<const celeritas::GeoMaterialParams>;
    using Args       = ImageRunArgs;
    //!@}

  public:
    // Construct with geometry
    explicit RDemoRunner(SPConstGeo geometry, SPConstGeoMaterial geomat);

    // Trace an image
    void operator()(ImageStore* image, int ntimes = 0) const;

  private:
    SPConstGeo geo_params_;
    SPConstGeoMaterial geomat_params_;
};

//---------------------------------------------------------------------------//
} // namespace demo_rasterizer
