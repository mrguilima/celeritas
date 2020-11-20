//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeoParams.test.cc
//---------------------------------------------------------------------------//
#include "geometry/GeoParamsTest.hh"

#include "celeritas_config.h"
#if CELERITAS_USE_CUDA
#    include <VecGeom/management/CudaManager.h>
#endif

// using namespace celeritas;
using namespace celeritas_test;

//---------------------------------------------------------------------------//
// HOST TESTS
//---------------------------------------------------------------------------//

class GeoParamsHostTest : public GeoParamsTest
{
  public:
    void SetUp() override
    {
        host_view = this->params()->host_pointers();
        CHECK(host_view.world_volume);
    }

    // Views
    GeoParamsPointers host_view;
};

#ifdef CELERITAS_USE_CUDA
class GeoParamsDeviceTest : public GeoParamsTest
{
  public:
    void SetUp() override
    {
        device_view = this->params()->device_pointers();
        CHECK(device_view.world_volume);
    }

    // Views
    GeoParamsPointers device_view;
};
#endif

//---------------------------------------------------------------------------//

TEST_F(GeoParamsHostTest, accessors)
{
    const auto& geom = *(this->params());
    EXPECT_EQ(20279, geom.num_volumes());
    EXPECT_EQ(16, geom.max_depth());
    EXPECT_EQ("OWWL0x7f4a8f617500",
              geom.id_to_label(VolumeId{
                  static_cast<unsigned int>(geom.num_volumes()) - 2}));
    EXPECT_EQ("OCMS0x7f4a9a758e00",
              geom.id_to_label(VolumeId{
                  static_cast<unsigned int>(geom.num_volumes()) - 1}));

    // print geometry information from CPU
    unsigned int nvols = geom.num_volumes();
    for (unsigned int i = 0; i < nvols; ++i)
    {
        std::cout << i << "" << geom.id_to_label(VolumeId{i}) << std::endl;
    }
}

//---------------------------------------------------------------------------//

#if CELERITAS_USE_CUDA
TEST_F(GeoParamsDeviceTest, accessors)
{
    const auto& geom = *(this->params());
    EXPECT_EQ(20279, geom.num_volumes());
    EXPECT_EQ(16, geom.max_depth());
    EXPECT_EQ("OWWL0x7f4a8f617500",
              geom.id_to_label(VolumeId{
                  static_cast<unsigned int>(geom.num_volumes()) - 2}));
    EXPECT_EQ("OCMS0x7f4a9a758e00",
              geom.id_to_label(VolumeId{
                  static_cast<unsigned int>(geom.num_volumes()) - 1}));
    // EXPECT_EQ(2, geom.num_volumes());
    // EXPECT_EQ(2, geom.max_depth());
    // EXPECT_EQ("Detector", geom.id_to_label(VolumeId{0}));
    // EXPECT_EQ("World", geom.id_to_label(VolumeId{1}));

    // print geometry information from device
    // vecgeom::cxx::CudaManager::Instance().PrintGeometry();
}
#endif
