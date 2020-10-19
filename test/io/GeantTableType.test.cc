//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantTableType.test.cc
//---------------------------------------------------------------------------//
#include "io/GeantTableType.hh"

#include "gtest/Main.hh"
#include "gtest/Test.hh"

using celeritas::GeantTableType;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class GeantTableTypeTest : public celeritas::Test
{
  protected:
    void SetUp() override {}
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//
TEST_F(GeantTableTypeTest, full_test)
{
    // Test default constructor
    GeantTableType table1;
    EXPECT_EQ(table1, GeantTableType::not_defined);

    // Test operator()
    table1("Lambda");
    EXPECT_EQ(table1, GeantTableType::Lambda);

    // Test constructor with string input
    GeantTableType table2("DEDX");
    EXPECT_EQ(table2, GeantTableType::DEDX);

    // Test constructor with enum input
    GeantTableType table3(GeantTableType::Lambda);
    EXPECT_EQ(table3, GeantTableType::Lambda);
}