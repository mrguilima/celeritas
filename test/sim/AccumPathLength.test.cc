//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file AccumPathLength.test.cc
//---------------------------------------------------------------------------//
#include "sim/AccumPathLength.hh"

#include "gtest/Main.hh"
#include "gtest/Test.hh"

using celeritas::AccumPathLength;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class AccumPathLengthTest : public celeritas::Test
{
  protected:
    void SetUp() override {}
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

TEST_F(AccumPathLengthTest, all)
{
    // Default construction is invalid
    AccumPathLength apl;
    EXPECT_FALSE(static_cast<bool>(apl));

#if CELERITAS_DEBUG
    // Require apl to be in a valid state before operating/comparing
    EXPECT_THROW({ bool failure = (apl == apl); }, celeritas::DebugError);
    EXPECT_THROW(apl += 3.0, celeritas::DebugError);
    EXPECT_THROW(apl - apl, celeritas::DebugError);
#endif

    // Assign zero value
    apl = AccumPathLength::zero();
    EXPECT_TRUE(apl == apl);
    EXPECT_TRUE(apl <= apl);
    EXPECT_TRUE(apl >= apl);
    EXPECT_FALSE(apl != apl);
    EXPECT_FALSE(apl < apl);
    EXPECT_FALSE(apl > apl);

    EXPECT_EQ(0.0, apl - apl);

    apl += 3.0;
    EXPECT_SOFT_EQ(3.0, apl - AccumPathLength::zero());
    EXPECT_FALSE(apl == AccumPathLength::zero());
    EXPECT_FALSE(apl <= AccumPathLength::zero());
    EXPECT_FALSE(AccumPathLength::zero() >= apl);
    EXPECT_TRUE(apl != AccumPathLength::zero());
    EXPECT_TRUE(AccumPathLength::zero() < apl);
    EXPECT_TRUE(apl > AccumPathLength::zero());

    auto cpl_five = apl + 2.0;
    EXPECT_SOFT_EQ(5.0, cpl_five - AccumPathLength::zero());
    EXPECT_SOFT_EQ(2.0, cpl_five - apl);

    auto cpl_seven = 2.0 + cpl_five;
    EXPECT_SOFT_EQ(4.0, cpl_seven - apl);

    // Default assignment should invalidate
    apl = {};
    EXPECT_FALSE(static_cast<bool>(apl));
}
