//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file geant-exporter.cc
//! \brief Geant4 particle, XS tables, material, and volume data exporter app
//---------------------------------------------------------------------------//
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <FTFP_BERT.hh>
#include <G4VModularPhysicsList.hh>
#include <G4GenericPhysicsList.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4Material.hh>
#include <G4MaterialTable.hh>

#include <G4UnitsTable.hh>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "GeantPhysicsTableWriter.hh"
#include "io/GeantParticle.hh"
#include "io/GeantPhysicsTable.hh"
#include "io/GeantGeometryMap.hh"

using namespace geant_exporter;
using celeritas::GeantGeometryMap;
using celeritas::GeantParticle;
using std::cout;
using std::endl;

//---------------------------------------------------------------------------//
/*!
 * Write particle table data to ROOT.
 *
 * The ROOT file must be open before this call.
 */
void store_particles(TFile* root_file, G4ParticleTable* particle_table)
{
    REQUIRE(root_file);
    REQUIRE(particle_table);

    cout << "Exporting particles..." << endl;
    TTree tree_particles("particles", "particles");

    // Create temporary particle
    GeantParticle particle;
    tree_particles.Branch("GeantParticle", &particle);

    G4ParticleTable::G4PTblDicIterator* particle_iterator
        = G4ParticleTable::GetParticleTable()->GetIterator();
    particle_iterator->reset();

    while ((*particle_iterator)())
    {
        G4ParticleDefinition* g4_particle_def = particle_iterator->value();

        // Skip "dummy" particles: generic ion and geantino
        if (g4_particle_def->GetPDGEncoding() == 0)
            continue;

        particle.name      = g4_particle_def->GetParticleName();
        particle.pdg       = g4_particle_def->GetPDGEncoding();
        particle.mass      = g4_particle_def->GetPDGMass();
        particle.charge    = g4_particle_def->GetPDGCharge();
        particle.spin      = g4_particle_def->GetPDGSpin();
        particle.lifetime  = g4_particle_def->GetPDGLifeTime();
        particle.is_stable = g4_particle_def->GetPDGStable();

        if (!particle.is_stable)
        {
            // Convert lifetime of unstable particles to seconds
            particle.lifetime /= s;
        }

        tree_particles.Fill();

        cout << "  Added " << g4_particle_def->GetParticleName() << endl;
    }
    cout << endl;

    root_file->Write();
}

//---------------------------------------------------------------------------//
/*!
 * Write physics table data to ROOT.
 *
 * The ROOT file must be open before this call.
 */
void store_physics_tables(TFile* root_file, G4ParticleTable* particle_table)
{
    REQUIRE(root_file);
    REQUIRE(particle_table);

    // Start table writer
    GeantPhysicsTableWriter table_writer(root_file);

    cout << "Exporting physics tables..." << endl;

    G4ParticleTable::G4PTblDicIterator* particle_iterator
        = G4ParticleTable::GetParticleTable()->GetIterator();
    particle_iterator->reset();

    while ((*particle_iterator)())
    {
        G4ParticleDefinition* g4_particle_def = particle_iterator->value();

        // Skip "dummy" particles: generic ion and geantino
        if (g4_particle_def->GetPDGEncoding() == 0)
            continue;

        std::cout << "=============" << std::endl;
        std::cout << g4_particle_def->GetParticleName() << std::endl;
        std::cout << "=============" << std::endl;

        G4ProcessVector* process_list
            = (g4_particle_def->GetProcessManager())->GetProcessList();

        for (std::size_t j = 0; j < process_list->size(); j++)
        {
            G4VProcess* process = (*process_list)[j];
            table_writer.add_physics_tables(process, g4_particle_def);
        }
    }
    cout << endl;

    root_file->Write();
}

//---------------------------------------------------------------------------//
/*!
 * Recurring loop over all logical volumes.
 *
 * Function called by store_geometry(...)
 */
void loop_volumes(GeantGeometryMap& geometry, G4LogicalVolume* logical_volume)
{
    // Add current logical volume
    GeantVolume              volume;
    GeantGeometryMap::vol_id volume_id;
    GeantGeometryMap::mat_id material_id;

    volume.name = logical_volume->GetName();
    volume_id   = logical_volume->GetInstanceID();
    material_id = logical_volume->GetMaterialCutsCouple()->GetIndex();

    geometry.add_volume(volume_id, volume);
    geometry.link_volume_material(volume_id, material_id);

    // Check for daughter volumes
    if (logical_volume->GetNoDaughters() > 0)
    {
        // Repeat for every daughter volume
        for (size_type i = 0; i < logical_volume->GetNoDaughters(); i++)
        {
            auto daughter_vol
                = logical_volume->GetDaughter(i)->GetLogicalVolume();

            loop_volumes(geometry, daughter_vol);
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Write material table data to ROOT.
 *
 * The ROOT file must be open before this call.
 */
void store_geometry(TFile*                             root_file,
                    G4ProductionCutsTable*             g4production_cuts,
                    std::shared_ptr<G4VPhysicalVolume> world_volume)
{
    REQUIRE(root_file);
    REQUIRE(g4production_cuts);
    REQUIRE(world_volume);

    cout << "Exporting material and volume information..." << endl;

    TTree tree_materials("geometry", "geometry");

    // Create material table and ROOT branch
    GeantGeometryMap geometry;
    tree_materials.Branch("GeantGeometryMap", &geometry);

    // Loop over materials and elements
    for (size_type i = 0; i < g4production_cuts->GetTableSize(); i++)
    {
        // Fetch material and element list
        auto g4material_cuts = g4production_cuts->GetMaterialCutsCouple(i);
        auto g4material      = g4material_cuts->GetMaterial();
        auto g4elements      = g4material->GetElementVector();

        // Populate material information
        GeantMaterial material;
        material.name             = g4material->GetName();
        material.state            = (MaterialState)g4material->GetState();
        material.temperature      = g4material->GetTemperature();
        material.density          = g4material->GetDensity() / (g / cm3);
        material.electron_density = g4material->GetTotNbOfElectPerVolume()
                                    / (1. / cm3);
        material.atomic_density = g4material->GetTotNbOfAtomsPerVolume()
                                  / (1. / cm3);
        material.radiation_length   = g4material->GetRadlen() / (g / cm2);
        material.nuclear_int_length = g4material->GetNuclearInterLength()
                                      / (g / cm2);

        // Populate element information
        for (size_type j = 0; j < g4elements->size(); j++)
        {
            auto g4element = g4elements->at(j);

            GeantElement element;
            element.name                  = g4element->GetName();
            element.atomic_number         = g4element->GetZ();
            element.atomic_mass           = g4element->GetAtomicMassAmu();
            element.radiation_length_tsai = g4element->GetfRadTsai()
                                            / (g / cm2);
            element.coulomb_factor = g4element->GetfCoulomb();

            GeantGeometryMap::elem_id elid = g4element->GetIndex();
            real_type frac = g4material->GetFractionVector()[j];

            // Add element to the global list
            geometry.add_element(elid, element);

            // Connect global element to a given material
            material.elements.push_back(elid);
            material.fractions.insert(
                std::pair<GeantGeometryMap::elem_id, real_type>(elid, frac));
        }
        geometry.add_material(g4material_cuts->GetIndex(), material);
    }

    // Loop over volumes
    loop_volumes(geometry, world_volume->GetLogicalVolume());

    tree_materials.Fill();
    root_file->Write();
}

//---------------------------------------------------------------------------//
/*!
 * This application exports particle information, XS physics tables, material,
 * and volume information constructed by the physics list and geometry.
 *
 * The data is stored into a ROOT file.
 */
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        // Incorrect number of arguments: print help and exit
        cout << "Usage: " << argv[0] << " geometry.gdml output.root" << endl;
        return EXIT_FAILURE;
    }
    std::string gdml_input_filename  = argv[1];
    std::string root_output_filename = argv[2];

    // >>> Initialize Geant4

    G4RunManager run_manager;

    // Initialize the geometry
    auto detector = std::make_unique<DetectorConstruction>(gdml_input_filename);
    // Copy G4VPhysicalVolume for future work before releasing detector ptr
    auto world_phys_volume = detector->get_world_volume();
    run_manager.SetUserInitialization(detector.release());

    // Load the physics list

    // User-defined physics list (see PhysicsList.hh)
    // auto physics_list = std::make_unique<PhysicsList>();

    // EM Standard Physics
    auto physics_constructor = std::make_unique<std::vector<G4String>>();
    physics_constructor->push_back("G4EmStandardPhysics");
    auto physics_list = std::make_unique<G4GenericPhysicsList>(
        physics_constructor.release());

    // Full Physics
    // auto physics_list = std::make_unique<FTFP_BERT>();

    run_manager.SetUserInitialization(physics_list.release());

    // Run a single particle to generate the physics tables
    auto action_initialization = std::make_unique<ActionInitialization>();
    run_manager.SetUserInitialization(action_initialization.release());

    G4UImanager::GetUIpointer()->ApplyCommand("/run/initialize");
    run_manager.BeamOn(1);

    // >>> Export data

    // Create the ROOT output file
    cout << "Creating " << root_output_filename << "..." << endl;
    TFile root_output(root_output_filename.c_str(), "recreate");

    // Store particle information
    store_particles(&root_output, G4ParticleTable::GetParticleTable());

    // Store physics tables
    store_physics_tables(&root_output, G4ParticleTable::GetParticleTable());

    // Store material and volume information
    store_geometry(&root_output,
                   G4ProductionCutsTable::GetProductionCutsTable(),
                   world_phys_volume);

    root_output.Close();

    cout << root_output_filename << " done!" << endl;

    return EXIT_SUCCESS;
}
