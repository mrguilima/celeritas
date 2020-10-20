//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file LinearPropagationHandler.test.cc
//---------------------------------------------------------------------------//
#include "geometry/LinearPropagator.hh"

#include "geometry/GeoTrackView.hh"
#include "GeoParamsTest.hh"
#include "magfield/InFieldPropagator.hh"

using namespace celeritas;
using namespace celeritas_test;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class FieldPropagHandlerHostTest : public GeoParamsTest
{
  public:
    using NavState = vecgeom::cxx::NavigationState;

    void SetUp()
    {
        int max_depth = params()->max_depth();
        state.reset(NavState::MakeInstance(max_depth));
        next_state.reset(NavState::MakeInstance(max_depth));

        state_view.size       = 1;
        state_view.vgmaxdepth = max_depth;
        state_view.pos        = &this->pos;
        state_view.dir        = &this->dir;
        state_view.next_step  = &this->next_step;
        state_view.vgstate    = this->state.get();
        state_view.vgnext     = this->next_state.get();

        params_view = this->params()->host_pointers();
        CHECK(params_view.world_volume);
    }

  protected:
    // State data
    Real3                     pos;
    Real3                     dir;
    real_type                 next_step;
    std::unique_ptr<NavState> state;
    std::unique_ptr<NavState> next_state;

    // Views
    GeoStatePointers  state_view;
    GeoParamsPointers params_view;
};

//---------------------------------------------------------------------------//
// HOST TESTS
//---------------------------------------------------------------------------//

TEST_F(FieldPropagHandlerHostTest, accessors)
{
    const auto& geom = *params();
    EXPECT_EQ(2, geom.num_volumes());
    EXPECT_EQ(2, geom.max_depth());
    EXPECT_EQ("Detector", geom.id_to_label(VolumeId{0}));
    EXPECT_EQ("World", geom.id_to_label(VolumeId{1}));
}

TEST_F(FieldPropagHandlerHostTest, track_helix)
{
    GeoTrackView      geo(params_view, state_view, ThreadId(0));
    InFieldPropagator propagate(geo); // one propHandler per track

    {
        // Track from outside detector, moving right
        geo = {{-6, 0, 0}, {1, 0, 0}};
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // World

        geo.find_next_step();
        EXPECT_SOFT_EQ(1.0, geo.next_step());
        propagate();
        EXPECT_SOFT_EQ(-5.0, geo.pos()[0]);
        EXPECT_EQ(VolumeId{0}, geo.volume_id()); // Detector

        geo.find_next_step();
        EXPECT_SOFT_EQ(10.0, geo.next_step());
        propagate();
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // World
        EXPECT_EQ(false, geo.is_outside());

        geo.find_next_step();
        EXPECT_SOFT_EQ(45.0, geo.next_step());
        propagate();
        EXPECT_EQ(true, geo.is_outside());
    }

    {
        // Track from outside edge fails
        geo = {{50, 0, 0}, {-1, 0, 0}};
        EXPECT_EQ(true, geo.is_outside());
    }

    {
        // But it works when you move juuust inside
        geo = {{50 - 1e-6, 0, 0}, {-1, 0, 0}};
        EXPECT_EQ(false, geo.is_outside());
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // World
        geo.find_next_step();
        EXPECT_SOFT_EQ(45.0 - 1e-6, geo.next_step());
        propagate();
        EXPECT_EQ(VolumeId{0}, geo.volume_id()); // Detector
    }
    {
        // Track from inside detector
        geo = {{0, 0, 0}, {1, 0, 0}};
        EXPECT_EQ(VolumeId{0}, geo.volume_id()); // Detector

        geo.find_next_step();
        EXPECT_SOFT_EQ(5.0, geo.next_step());
        propagate();
        EXPECT_SOFT_EQ(5.0, geo.pos()[0]);
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // World
        EXPECT_EQ(false, geo.is_outside());

        geo.find_next_step();
        EXPECT_SOFT_EQ(45.0, geo.next_step());
        propagate();
        EXPECT_EQ(true, geo.is_outside());
    }
}
