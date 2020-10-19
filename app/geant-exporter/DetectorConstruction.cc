//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file DetectorConstruction.cc
//---------------------------------------------------------------------------//

#include <G4GDMLParser.hh>

#include "DetectorConstruction.hh"
#include "base/Assert.hh"

namespace geant_exporter
{
//---------------------------------------------------------------------------//
/*!
 * Construct with a given gdml input file.
 */
DetectorConstruction::DetectorConstruction(G4String gdmlInput)
{
    G4GDMLParser   gdml_parser;
    constexpr bool validate_gdml_schema = false;
    gdml_parser.Read(gdmlInput, validate_gdml_schema);
    phys_vol_world_.reset(gdml_parser.GetWorldVolume());
    ENSURE(phys_vol_world_);
}

//---------------------------------------------------------------------------//
DetectorConstruction::~DetectorConstruction() = default;

//---------------------------------------------------------------------------//
/*!
 * Return the loaded world volume.
 */
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    REQUIRE(phys_vol_world_);
    return phys_vol_world_.get();
}

//---------------------------------------------------------------------------//
/*!
 * Return the world physical volume shared pointer.
 */
std::shared_ptr<G4VPhysicalVolume> DetectorConstruction::get_world_volume()
{
    return std::shared_ptr<G4VPhysicalVolume>(phys_vol_world_);
}

//---------------------------------------------------------------------------//
} // namespace geant_exporter
