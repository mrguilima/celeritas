//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file LinearPropagator.test.cc
//---------------------------------------------------------------------------//
#include "geometry/LinearPropagator.hh"

#include "GeoParamsTest.hh"
#include "geometry/GeoStateStore.hh"

#ifdef CELERITAS_USE_CUDA
#    include "LinearPropagator.test.hh"
#endif

#include "base/ArrayIO.hh"

using namespace celeritas;
using namespace celeritas_test;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class LinearPropagatorHostTest : public GeoParamsTest
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

//---------------------------------------------------------------------------//
// HOST TESTS
//---------------------------------------------------------------------------//

TEST_F(LinearPropagatorHostTest, accessors)
{
    const auto& geom = *params();
    EXPECT_EQ(11, geom.num_volumes());
    EXPECT_EQ(4, geom.max_depth());
    EXPECT_EQ("Shape20x7fafecc185a0", geom.id_to_label(VolumeId{0}));
    EXPECT_EQ("Shape10x7fafecc16590", geom.id_to_label(VolumeId{1}));
}

//----------------------------------------------------------------------------//

TEST_F(LinearPropagatorHostTest, track_line)
{
    GeoTrackView     geo(params_view, state_view, ThreadId(0));
    LinearPropagator propagate(geo); // one propagator per track

    {
        // Track from outside detector, moving right
        geo = {{-6, 0, 0}, {1, 0, 0}};
        EXPECT_EQ(VolumeId{2}, geo.volume_id()); // World

        geo.find_next_step();
        EXPECT_SOFT_EQ(0, geo.next_step());
        propagate();
        EXPECT_SOFT_EQ(-6, geo.pos()[0]);
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // Detector

        geo.find_next_step();
        EXPECT_SOFT_EQ(1., geo.next_step());
        propagate();
        EXPECT_EQ(VolumeId{0}, geo.volume_id()); // World
        EXPECT_EQ(false, geo.is_outside());

        geo.find_next_step();
        EXPECT_SOFT_EQ(10., geo.next_step());
        propagate();
        EXPECT_EQ(false, geo.is_outside());
    }

    {
        // Track from outside edge fails
        geo = {{24, 0, 0}, {-1, 0, 0}};
        EXPECT_EQ(true, geo.is_outside());
    }

    {
        // But it works when you move juuust inside
        real_type eps = 1e-6;
        geo           = {{24 - eps, 0, 0}, {-1, 0, 0}};
        EXPECT_EQ(false, geo.is_outside());
        EXPECT_EQ(VolumeId{10}, geo.volume_id()); // World
        geo.find_next_step();
        EXPECT_SOFT_EQ(17. - eps, geo.next_step());
        propagate();
        EXPECT_EQ(VolumeId{2}, geo.volume_id()); // Detector

        // debugging
        geo.volume().Print();
    }
    {
        std::cerr << " trkLine: spot 1\n";
        // Track from inside detector
        geo = {{10, 10, 10}, {-1, 0, 0}};
        EXPECT_EQ(VolumeId{10}, geo.volume_id()); // Detector

        std::cerr << " trkLine: spot 2\n";
        // debugging
        std::cerr << "\n === current position: " << geo.pos() << "\n";
        geo.volume().Print();

        std::cerr << " trkLine: spot 3\n";
        geo.find_next_step();
        std::cerr << " trkLine: spot 4\n";
        EXPECT_SOFT_EQ(34., geo.next_step());
        std::cerr << " trkLine: spot 5\n";
        propagate();
        std::cerr << " trkLine: spot 6\n";
        EXPECT_SOFT_EQ(-24.0, geo.pos()[0]);
        std::cerr << " trkLine: spot 7\n";
        EXPECT_EQ(VolumeId{10}, geo.volume_id()); // World
        std::cerr << " trkLine: spot 8\n";
        EXPECT_EQ(true, geo.is_outside());
        std::cerr << " trkLine: spot 9\n";

        geo.find_next_step();
        EXPECT_SOFT_EQ(45.0, geo.next_step());
        propagate();
        EXPECT_EQ(true, geo.is_outside());
        std::cerr << " trkLine: spot 10\n";
    }
}

//----------------------------------------------------------------------------//

TEST_F(LinearPropagatorHostTest, track_intraVolume)
{
    GeoTrackView     geo(params_view, state_view, ThreadId(0));
    LinearPropagator propagate(geo); // one propagator per track

    {
        // Track from outside detector, moving right
        geo = {{-6, 0, 0}, {1, 0, 0}};
        EXPECT_EQ(VolumeId{2}, geo.volume_id()); // World

        geo.find_next_step();
        EXPECT_SOFT_EQ(0, geo.next_step());

        // break next step into two
        propagate(0.5 * geo.next_step());
        EXPECT_SOFT_EQ(-5.5, geo.pos()[0]);
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // World

        propagate(geo.next_step()); // all remaining
        EXPECT_SOFT_EQ(-5.0, geo.pos()[0]);
        EXPECT_EQ(VolumeId{0}, geo.volume_id()); // Detector

        // break next step into more than two, re-calculating next_step each
        // time
        geo.find_next_step();
        EXPECT_SOFT_EQ(10.0, geo.next_step());

        propagate(0.3 * geo.next_step()); // step 1 inside Detector
        EXPECT_SOFT_EQ(-2.0, geo.pos()[0]);
        EXPECT_EQ(VolumeId{0}, geo.volume_id()); // Detector

        geo.find_next_step();
        EXPECT_SOFT_EQ(7.0, geo.next_step());

        propagate(0.5 * geo.next_step()); // step 2 inside Detector
        EXPECT_SOFT_EQ(1.5, geo.pos()[0]);
        EXPECT_EQ(VolumeId{0}, geo.volume_id()); // Detector

        geo.find_next_step();
        EXPECT_SOFT_EQ(3.5, geo.next_step());

        propagate(geo.next_step()); // last step inside Detector
        EXPECT_SOFT_EQ(5.0, geo.pos()[0]);
        EXPECT_EQ(VolumeId{1}, geo.volume_id()); // World
    }
}

//---------------------------------------------------------------------------//
// DEVICE TESTS
//---------------------------------------------------------------------------//
#if CELERITAS_USE_CUDA

class LinearPropagatorDeviceTest : public GeoParamsTest
{
};

TEST_F(LinearPropagatorDeviceTest, track_lines)
{
    CHECK(this->params());

    // Set up test input
    LinPropTestInput input;
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
    auto output = linProp_test(input);

    static const int expected_ids[] = {1, 0, 1, 0, 1, -1, -1, -1, -1, 1, 0, 1};
    static const double expected_distances[]
        = {1, 10, 45, 5, 45, -1, -1, -1, -1, 45 - 1e-6, 10, 45};

    // Check results
    EXPECT_VEC_EQ(expected_ids, output.ids);
    EXPECT_VEC_SOFT_EQ(output.distances, expected_distances);
}

//---------------------------------------------------------------------------//
#endif
