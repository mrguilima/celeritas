//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file PhysicsList.cc
//---------------------------------------------------------------------------//
#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4ComptonScattering.hh>
#include <G4ProcessManager.hh>
#include <G4SystemOfUnits.hh>

#include "PhysicsList.hh"

namespace geant_exporter
{
//---------------------------------------------------------------------------//
/*!
 * Constructor and default destructor
 */
PhysicsList::PhysicsList() : G4VUserPhysicsList() {}

PhysicsList::~PhysicsList() = default;

//---------------------------------------------------------------------------//
/*!
 * Build list of available particles
 */
void PhysicsList::ConstructParticle()
{
    G4Gamma::GammaDefinition();
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
}

//---------------------------------------------------------------------------//
/*!
 * Build list of available processes and models
 */
void PhysicsList::ConstructProcess()
{
    AddTransportation();
    auto klein_nishina_process = std::make_unique<G4ComptonScattering>();

    G4ProcessManager* process_manager = G4Gamma::Gamma()->GetProcessManager();
    process_manager->AddDiscreteProcess(klein_nishina_process.release());
}

//---------------------------------------------------------------------------//
} // namespace geant_exporter
