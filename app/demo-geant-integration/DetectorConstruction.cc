//----------------------------------*-C++-*----------------------------------//
// Copyright 2022-2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file demo-geant-integration/DetectorConstruction.cc
//---------------------------------------------------------------------------//
#include "DetectorConstruction.hh"

#include <map>
#include <G4Exception.hh>
#include <G4GDMLAuxStructType.hh>
#include <G4GDMLParser.hh>
#include <G4LogicalVolume.hh>
#include <G4SDManager.hh>
#include <G4VPhysicalVolume.hh>

#include "corecel/io/Logger.hh"
#include "accel/SetupOptions.hh"

#include "GlobalSetup.hh"
#include "SensitiveDetector.hh"

namespace demo_geant
{
//---------------------------------------------------------------------------//
/*!
 * Set up Celeritas SD options during construction.
 *
 * This should be done only during the main/serial thread.
 */
DetectorConstruction::DetectorConstruction()
{
    auto& sd = demo_geant::GlobalSetup::Instance()->GetSDSetupOptions();

    // Use Celeritas "hit processor" to call back to Geant4 SDs.
    sd.enabled = true;

    // Only call back for nonzero energy depositions: this is currently a
    // global option for all detectors, so if any SDs extract data from tracks
    // with no local energy deposition over the step, it must be set to false.
    sd.ignore_zero_deposition = true;

    // Using the pre-step point, reconstruct the G4 touchable handle.
    sd.locate_touchable = true;

    // Since at least one SD uses the pre-step time, export it from Celeritas.
    sd.pre.global_time = true;
}

//---------------------------------------------------------------------------//
/*!
 * Load geometry and sensitive detector volumes.
 */
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    CELER_LOG_LOCAL(status) << "Loading detector geometry";

    G4GDMLParser gdml_parser;
    gdml_parser.SetStripFlag(false);

    std::string const& filename = GlobalSetup::Instance()->GetGeometryFile();
    if (filename.empty())
    {
        G4Exception("DetectorConstruction::Construct()",
                    "",
                    FatalException,
                    "No GDML file was specified with setGeometryFile");
    }
    constexpr bool validate_gdml_schema = false;
    gdml_parser.Read(filename, validate_gdml_schema);

    // Find sensitive detectors
    for (auto const& lv_vecaux : *gdml_parser.GetAuxMap())
    {
        for (G4GDMLAuxStructType const& aux : lv_vecaux.second)
        {
            if (aux.type == "SensDet")
            {
                detectors_.emplace_back(lv_vecaux.first, aux.value);
            }
        }
    }

    // Claim ownership of world volume and pass it to the caller
    return gdml_parser.GetWorldVolume();
}

//---------------------------------------------------------------------------//
void DetectorConstruction::ConstructSDandField()
{
    CELER_LOG_LOCAL(debug) << "Loading sensitive detectors";

    G4SDManager* sd_manager = G4SDManager::GetSDMpointer();

    for (auto& lv_name : detectors_)
    {
        auto detector = std::make_unique<SensitiveDetector>(lv_name.second);
        lv_name.first->SetSensitiveDetector(detector.get());
        sd_manager->AddNewDetector(detector.release());
    }
}

//---------------------------------------------------------------------------//
}  // namespace demo_geant
