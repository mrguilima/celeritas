//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file RungeKuttaStepper.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Array.hh"
#include "base/Range.hh"
#include "base/Types.hh"
#include "base/Macros.hh"

#include "FieldInterface.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Adaptive step size control with the 4th order classical Runge-Kutta
 * method.
 *
 * This method estimates the updated state from an initial state and evaluates
 * the truncation error, with fourth-order accuracy based on description in
 * Numerical Recipes in C, The Art of Scientific Computing, Sec. 16.2,
 * Adaptive Stepsize Control for Runge-Kutta.
 */
template<class FieldEquation_T>
class RungeKuttaStepper
{
  public:
    //!@{
    //! Type aliases
    using Result = StepperResult;
    //!@}

  public:
    //! Construct with the equation of motion
    CELER_FUNCTION
    RungeKuttaStepper(const FieldEquation_T& eq) : equation_(eq) {}

    // Adaptive step size control
    inline CELER_FUNCTION auto
    operator()(real_type step, const OdeState& beg_state) -> Result;

  private:
    //// DATA ////

    // Equation of motion
    const FieldEquation_T& equation_;

    //// HELPER FUNCTIONS ////

    // Return the final state by the 4th order Runge-Kutta method
    inline CELER_FUNCTION auto do_step(real_type       step,
                                       const OdeState& beg_state,
                                       const OdeState& end_slope) const
        -> OdeState;
};

//---------------------------------------------------------------------------//
} // namespace celeritas

#include "RungeKuttaStepper.i.hh"
