//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeoTrackView.test.cc
//---------------------------------------------------------------------------//
#include "geometry/GeoTrackView.hh"

#include <VecGeom/navigation/NavigationState.h>
#include "geometry/GeoParams.hh"
#include "geometry/GeoStateStore.hh"

#include "GeoParamsTest.hh"
#ifdef CELERITAS_USE_CUDA
#    include "GeoTrackView.test.hh"
#endif

#include "base/ArrayIO.hh"

using namespace celeritas;
using namespace celeritas_test;

//---------------------------------------------------------------------------//
// HOST TESTS
//---------------------------------------------------------------------------//

class GeoTrackViewHostTest : public GeoParamsTest
{
  public:
    using NavState = vecgeom::cxx::NavigationState;

    void SetUp() override
    {
        int max_depth = this->params()->max_depth();
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

TEST_F(GeoTrackViewHostTest, track_line)
{
    // Construct geometry interface from persistent geometry data, state view,
    // and thread ID (which for CPU is just zero).
    GeoTrackView geo(params_view, state_view, ThreadId(0));

    {
        // Track from outside detector, moving right
        geo = {{-6, 0, 0}, {1, 0, 0}};
        EXPECT_EQ(VolumeId{10}, geo.volume_id()); // World

        geo.find_next_step();
        EXPECT_SOFT_EQ(2.5, geo.next_step());
        geo.move_next_step();
        EXPECT_SOFT_EQ(-3.5, geo.pos()[0]);
        EXPECT_EQ(VolumeId{2}, geo.volume_id()); // Detector

        geo.find_next_step();
        EXPECT_SOFT_EQ(0.5, geo.next_step());
        geo.move_next_step();
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // World
        EXPECT_EQ(false, geo.is_outside());

        geo.find_next_step();
        EXPECT_SOFT_EQ(0.5, geo.next_step());
        geo.move_next_step();
        EXPECT_EQ(false, geo.is_outside());
    }

    {
        // Track from outside edge fails
        geo = {{24, 0, 0}, {-1, 0, 0}};
        EXPECT_EQ(true, geo.is_outside());
    }

    {
        // But it works when you move juuust inside
        geo = {{24 - 1e-3, 0, 0}, {-1, 0, 0}};
        EXPECT_EQ(false, geo.is_outside());
        EXPECT_EQ(VolumeId{10}, geo.volume_id()); // World
        geo.find_next_step();
        EXPECT_SOFT_EQ(20.5 - 1e-6, geo.next_step());
        geo.move_next_step();
        EXPECT_EQ(VolumeId{2}, geo.volume_id()); // Detector

        // debugging
        geo.volume().Print();
    }
    {
        std::cerr << " trkLine: spot 1\n";
        // Track from inside detector
        geo = {{5, 5, 5}, {-1, 0, 0}};
        EXPECT_EQ(VolumeId{10}, geo.volume_id()); // Detector

        std::cerr << " trkLine: spot 2\n";
        // debugging
        std::cerr << "\n === current position: " << geo.pos() << "\n";
        geo.volume().Print();

        std::cerr << " trkLine: spot 3\n";
        geo.find_next_step();
        std::cerr << " trkLine: spot 4\n";
        EXPECT_SOFT_EQ(36.0, geo.next_step());
        std::cerr << " trkLine: spot 5\n";
        geo.move_next_step();
        std::cerr << " trkLine: spot 6\n";
        EXPECT_SOFT_EQ(-24.0, geo.pos()[0]);
        std::cerr << " trkLine: spot 7\n";
        EXPECT_EQ(VolumeId{10}, geo.volume_id()); // World
        std::cerr << " trkLine: spot 8\n";
        EXPECT_EQ(false, geo.is_outside());
        std::cerr << " trkLine: spot 9\n";

        geo.find_next_step();
        EXPECT_SOFT_EQ(45.0, geo.next_step());
        geo.move_next_step();
        EXPECT_EQ(true, geo.is_outside());
        std::cerr << " trkLine: spot 10\n";
    }
}

#if CELERITAS_USE_CUDA
//---------------------------------------------------------------------------//
// DEVICE TESTS
//---------------------------------------------------------------------------//

class GeoTrackViewDeviceTest : public GeoParamsTest
{
};

TEST_F(GeoTrackViewDeviceTest, track_lines)
{
    CHECK(this->params());

    // Set up test input
    VGGTestInput input;
    input.init = {
        {{-6, 0, 0}, {1, 0, 0}},
        {{0, 0, 0}, {1, 0, 0}},
        {{50, 0, 0}, {-1, 0, 0}},
        {{50 - 1e-6, 0, 0}, {-1, 0, 0}},
    };
    input.max_segments = 3;
    input.shared       = this->params()->device_pointers();

    GeoStateStore device_states(*this->params(), input.init.size());
    input.state = device_states.device_pointers();

    // Run kernel
    auto output = vgg_test(input);

    static const int expected_ids[] = {1, 0, 1, 0, 1, -1, -1, -1, -1, 1, 0, 1};
    static const double expected_distances[]
        = {1, 10, 45, 5, 45, -1, -1, -1, -1, 45 - 1e-6, 10, 45};

    // Check results
    EXPECT_VEC_EQ(expected_ids, output.ids);
    EXPECT_VEC_SOFT_EQ(output.distances, expected_distances);
}

//---------------------------------------------------------------------------//
#endif
