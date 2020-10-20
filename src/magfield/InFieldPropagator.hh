//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file InFieldPropagator.hh
//---------------------------------------------------------------------------//
/**
 * @brief Interface between the scheduler and the field integrator.
 *
 * Originated from GeantV
 */
//===----------------------------------------------------------------------===//
#pragma once

#include "base/Units.hh"
#include "geometry/GeoTrackView.hh"
#include "physics/base/ParticleTrackView.hh"
//#include "geometry/track/GeoTrackState.hh"

namespace celeritas {

class FieldLookup 
{
public:
  static constexpr real_type _bmag          = 3 * units::tesla;
  static constexpr Real3 _bfield = {0.0, 0.0, _bmag};

  CELER_FUNCTION
  static void get_field_value(const Real3 &pos, Real3 &magFld, double &bmag) {
    (void)pos[0]; // avoid unused variable warning
    bmag   = _bmag;
    magFld = _bfield; //.x(), _bfield.y(), _bfield.z()};
  }
};

class InFieldPropagator
{
public:
   using ThreeVector_t       = Real3;

   CELER_FUNCTION
   InFieldPropagator(GeoTrackView &track, ParticleTrackView &particle)
     : track_(track)
     , particle_(particle)
   {
     //REQUIRE(track_);
     //REQUIRE(particle_);
   }

   CELER_FUNCTION
   double curvature() const;

   CELER_FUNCTION
   double curvature(const ThreeVector_t &magFld, double bmag) const;

   CELER_FUNCTION
   bool operator()() const;

   CELER_FUNCTION
   void propagate_in_volume(double crtstep, const ThreeVector_t &BfieldInitial, 
			    double bmag) const;

   // CELER_FUNCTION
   // bool IsSameLocation(GeoTrackView &track) const;

   CELER_FORCEINLINE_FUNCTION
   double safe_length(double eps, const ThreeVector_t &magFld, double bmag) const
   {
      // Returns the propagation length in field such that the propagated point is
      // shifted less than eps with respect to the linear propagation.
      // OLD: return 2. * sqrt(eps / track.Curvature(Bz));
      double c   = this->curvature(magFld, bmag); //, td);
      double val = 0.0;
      // if (c < 1.E-10) { val= 1.E50; } else
      val = 2. * sqrt(eps / c);
      return val;
   }


// #ifndef __NVCC__
//    void PrintStats() const;
// #endif

   CELER_FUNCTION
   void check_track(const char *msg, double epsilon = 1.0e-5) const;

private:
   //@{
   //! Referenced thread-local data
   GeoTrackView&     track_;
   ParticleTrackView& particle_;
   //@}
};

} // namespace celeritas

#include "InFieldPropagator.i.hh"
