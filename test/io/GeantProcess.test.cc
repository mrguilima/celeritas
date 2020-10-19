//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantProcess.test.cc
//---------------------------------------------------------------------------//
#include "io/GeantProcess.hh"

#include "gtest/Main.hh"
#include "gtest/Test.hh"

using celeritas::GeantProcess;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class GeantProcessTest : public celeritas::Test
{
  protected:
    void SetUp() override {}
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//
TEST_F(GeantProcessTest, full_test)
{
    // Test default constructor
    GeantProcess process1;
    EXPECT_EQ(process1, GeantProcess::not_defined);

    // Test operator()
    process1("compt");
    EXPECT_EQ(process1, GeantProcess::compt);

    // Test constructor with string input
    GeantProcess process2("not_defined");
    EXPECT_EQ(process2, GeantProcess::not_defined);

    // Test constructor with enum input
    GeantProcess process3(GeantProcess::compt);
    EXPECT_EQ(process3, GeantProcess::compt);
}