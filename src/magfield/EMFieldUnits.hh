//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file base/EMFieldUnits.hh
//---------------------------------------------------------------------------//
/**
 * @brief   System of units for EM-field and related physics
 *
 * Units here were extracted from Geant4 system of units, and adapted for Celeritas.
 * Authors of the original version: M. Maire, S. Giani
 *
 * The basic units are :
 * \li  giga electron volt      (GeV)
 * \li  positron charge         (eplus)
 * \li  Volt, Tesla, Gauss
 *
 */
#pragma once

namespace celeritas {
namespace units {
  /*
  //
// (kPi is used here (degree) so we declare here and not among the constants.)
//
static constexpr double kPi       = 3.14159265358979323846;
static constexpr double kTwoPi    = 2. * kPi;
static constexpr double kHalfPi   = kPi / 2.;
static constexpr double kPiSquare = kPi * kPi;

//
// Length [L]
//
static constexpr double centimeter  = 1.;
static constexpr double centimeter2 = centimeter * centimeter;
static constexpr double centimeter3 = centimeter * centimeter * centimeter;

static constexpr double millimeter  = 0.1 * centimeter;
static constexpr double millimeter2 = millimeter * millimeter;
static constexpr double millimeter3 = millimeter * millimeter * millimeter;

static constexpr double meter  = 100. * centimeter;
static constexpr double meter2 = meter * meter;
static constexpr double meter3 = meter * meter * meter;

static constexpr double kilometer  = 1000. * meter;
static constexpr double kilometer2 = kilometer * kilometer;
static constexpr double kilometer3 = kilometer * kilometer * kilometer;

static constexpr double parsec = 3.0856775807e+16 * meter;

static constexpr double micrometer = 1.e-6 * meter;
static constexpr double nanometer  = 1.e-9 * meter;
static constexpr double angstrom   = 1.e-10 * meter;
static constexpr double fermi      = 1.e-15 * meter;

static constexpr double barn      = 1.e-28 * meter2;
static constexpr double millibarn = 1.e-3 * barn;
static constexpr double microbarn = 1.e-6 * barn;
static constexpr double nanobarn  = 1.e-9 * barn;
static constexpr double picobarn  = 1.e-12 * barn;

// symbols
static constexpr double nm = nanometer;
static constexpr double um = micrometer;

static constexpr double mm  = millimeter;
static constexpr double mm2 = millimeter2;
static constexpr double mm3 = millimeter3;

static constexpr double cm  = centimeter;
static constexpr double cm2 = centimeter2;
static constexpr double cm3 = centimeter3;

static constexpr double liter = 1.e+3 * cm3;
static constexpr double L     = liter;
static constexpr double dL    = 1.e-1 * liter;
static constexpr double cL    = 1.e-2 * liter;
static constexpr double mL    = 1.e-3 * liter;

static constexpr double m  = meter;
static constexpr double m2 = meter2;
static constexpr double m3 = meter3;

static constexpr double km  = kilometer;
static constexpr double km2 = kilometer2;
static constexpr double km3 = kilometer3;

static constexpr double pc = parsec;

//
// Angle
//
static constexpr double radian      = 1.;
static constexpr double milliradian = 1.e-3 * radian;
static constexpr double degree      = (kPi / 180.0) * radian;

static constexpr double steradian = 1.;

// symbols
static constexpr double rad  = radian;
static constexpr double mrad = milliradian;
static constexpr double sr   = steradian;
static constexpr double deg  = degree;

//
// Time [T]
//
static constexpr double second      = 1.;
static constexpr double millisecond = 1.e-3 * second;
static constexpr double microsecond = 1.e-6 * second;
static constexpr double nanosecond  = 1.e-9 * second;
static constexpr double picosecond  = 1.e-12 * second;

static constexpr double hertz     = 1. / second;
static constexpr double kilohertz = 1.e+3 * hertz;
static constexpr double megahertz = 1.e+6 * hertz;

// symbols
static constexpr double s  = second;
static constexpr double ms = millisecond;
static constexpr double ns = nanosecond;
  */


//
// Electric charge [Q]
//
static constexpr real_type eplus   = 1.;              // positron charge
static constexpr real_type e_SI    = 1.602176487e-19; // positron charge in coulomb
static constexpr real_type coulomb = eplus / e_SI;    // coulomb = 6.24150 e+18 * eplus

  /*
//
// Energy [E]
//
static constexpr real_type gigaelectronvolt = 1.;
static constexpr real_type megaelectronvolt = 1.e-3 * gigaelectronvolt;
static constexpr real_type kiloelectronvolt = 1.e-6 * gigaelectronvolt;
static constexpr real_type electronvolt     = 1.e-9 * gigaelectronvolt;
static constexpr real_type teraelectronvolt = 1.e+3 * gigaelectronvolt;
static constexpr real_type petaelectronvolt = 1.e+6 * gigaelectronvolt;

static constexpr real_type joule = electronvolt / e_SI; // joule = 6.24150 e+9 * GeV
  */

// symbols
static constexpr real_type MeV = mega_electron_volt;
static constexpr real_type eV  = 1.0e-6 * MeV;
static constexpr real_type keV = 1.0e-3 * MeV;
static constexpr real_type GeV = 1.0e+3 * MeV;
static constexpr real_type TeV = 1.0e+6 * MeV;
static constexpr real_type PeV = 1.0e+9 * MeV;

  /*
//
// Mass [E][T^2][L^-2]
//
static constexpr real_type kilogram  = joule * second * second / (meter * meter);
static constexpr real_type gram      = 1.e-3 * kilogram;
static constexpr real_type milligram = 1.e-3 * gram;

// symbols
static constexpr real_type kg = kilogram;
static constexpr real_type g  = gram;
static constexpr real_type mg = milligram;

//
// Power [E][T^-1]
//
static constexpr real_type watt = joule / second; // watt = 6.24150 e+9 * GeV/s

//
// Force [E][L^-1]
//
static constexpr real_type newton = joule / meter; // newton = 6.24150 e+7 * GeV/cm

//
// Pressure [E][L^-3]
//
static constexpr real_type pascal     = newton / m2;     // pascal = 6.24150 e+3 * GeV/cm3
static constexpr real_type bar        = 1.e+5 * pascal;  // bar    = 6.24150 e+8 * GeV/cm3
static constexpr real_type atmosphere = 101325 * pascal; // atm    = 6.32420 e+8 * GeV/cm3
  */

//
// Electric current [Q][T^-1]
//
static constexpr real_type ampere      = coulomb / second; // ampere = 6.24150 e+18 * eplus/s
static constexpr real_type milliampere = 1.e-3 * ampere;
static constexpr real_type microampere = 1.e-6 * ampere;
static constexpr real_type nanoampere  = 1.e-9 * ampere;

//
// Electric potential [E][Q^-1]
//
static constexpr real_type gigavolt = GeV / eplus;
static constexpr real_type megavolt = 1.e-3 * gigavolt;
static constexpr real_type kilovolt = 1.e-6 * gigavolt;
static constexpr real_type volt     = 1.e-9 * gigavolt;

//
// Electric resistance [E][T][Q^-2]
//
static constexpr real_type ohm = volt / ampere; // ohm = 1.60217e-18*(GeV/eplus)/(eplus/s)

//
// Electric capacitance [Q^2][E^-1]
//
static constexpr real_type farad = coulomb / volt; // farad = 6.24150e+27 * eplus/gigavolt
static constexpr real_type millifarad = 1.e-3 * farad;
static constexpr real_type microfarad = 1.e-6 * farad;
static constexpr real_type nanofarad  = 1.e-9 * farad;
static constexpr real_type picofarad  = 1.e-12 * farad;

//
// Magnetic Flux [T][E][Q^-1]
//
static constexpr real_type weber = volt * second; // weber = 1.e-9*gigavolt*s

//
// Magnetic Field [T][E][Q^-1][L^-2]
//
  static constexpr real_type tesla = volt * second / (meter * meter); // tesla =1.e-13*gigavolt*s/cm2
static constexpr real_type gauss     = 1.e-4 * tesla;
static constexpr real_type kilogauss = 1.e-1 * tesla;

//
// Miscellaneous
//
static constexpr real_type perCent     = 0.01;
static constexpr real_type perThousand = 0.001;
static constexpr real_type perMillion  = 0.000001;

} // namespace units
} // namespace celeritas
