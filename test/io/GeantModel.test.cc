//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantModel.test.cc
//---------------------------------------------------------------------------//
#include "io/GeantModel.hh"

#include "gtest/Main.hh"
#include "gtest/Test.hh"

using celeritas::GeantModel;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class GeantModelTest : public celeritas::Test
{
  protected:
    void SetUp() override {}
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//
TEST_F(GeantModelTest, full_test)
{
    GeantModel model1;
    EXPECT_EQ(model1, GeantModel::not_defined);

    model1("Klein-Nishina");
    EXPECT_EQ(model1, GeantModel::KleinNishina);

    GeantModel model2("not_defined");
    EXPECT_EQ(model2, GeantModel::not_defined);

    GeantModel model3(GeantModel::KleinNishina);
    EXPECT_EQ(model3, GeantModel::KleinNishina);
}