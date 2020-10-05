//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file geant-exporter.cc
//! \brief Geant4 particle definition and physics tables exporter app
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

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "GeantPhysicsTableWriter.hh"
#include "io/GeantParticle.hh"
#include "io/GeantPhysicsTable.hh"
#include "io/GeantMaterialTable.hh"
#include "io/GeantVolume.hh"

using celeritas::GeantMaterialTable;
using celeritas::GeantParticle;
using namespace geant_exporter;
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
 * Function called by store_material_table(...)
 */
void loop_volumes(GeantMaterialTable& material_table,
                  G4LogicalVolume*    logical_volume)
{
    // Loop over logical volumes
    for (size_type i = 0; i < logical_volume->GetNoDaughters(); i++)
    {
        GeantVolume                volume;
        GeantMaterialTable::vol_id volume_id;
        GeantMaterialTable::mat_id material_id;

        volume.name = logical_volume->GetName();
        volume_id   = logical_volume->GetInstanceID();
        material_id = logical_volume->GetMaterialCutsCouple()->GetIndex();

        material_table.add_volume(volume_id, volume);
        material_table.link_volume_material(volume_id, material_id);

        // REPEAT
        // Recurrent loop to read all logical volumes found in the gdml
        auto daughter_vol = logical_volume->GetDaughter(i)->GetLogicalVolume();
        if (daughter_vol->GetNoDaughters() > 0)
        {
            loop_volumes(material_table, daughter_vol);
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Write material table data to ROOT.
 *
 * The ROOT file must be open before this call.
 *
 * TEMP NOTES:
 * - ProductionCutsTable size matches the physics tables sizes
 * - MaterialTable size does not
 *   - For cms, physics table size = 444 and material table size = 385
 * - Do we need any cuts?
 */
void store_material_table(TFile*                             root_file,
                          G4ProductionCutsTable*             g4production_cuts,
                          std::shared_ptr<G4VPhysicalVolume> world_volume)
{
    REQUIRE(root_file);
    REQUIRE(g4production_cuts);
    REQUIRE(world_volume);

    cout << "Exporting material and volume information..." << endl;

    TTree tree_materials("materials", "materials");

    // Create material table and ROOT branch
    GeantMaterialTable geant_material_table;
    tree_materials.Branch("GeantMaterialTable", &geant_material_table);

    // Loop over materials and elements
    for (size_type i = 0; i < g4production_cuts->GetTableSize(); i++)
    {
        GeantMaterial material;
        auto g4material_cuts = g4production_cuts->GetMaterialCutsCouple(i);
        auto g4material      = g4material_cuts->GetMaterial();

        material.name    = g4material->GetName();
        material.density = g4material->GetDensity() / (g / cm3);

        auto g4elements = g4material->GetElementVector();

        for (size_type j = 0; j < g4elements->size(); j++)
        {
            GeantElement element;
            element.name        = g4elements->at(j)->GetName();
            element.fraction    = g4material->GetFractionVector()[j];
            element.z           = g4elements->at(j)->GetZ();
            element.atomic_mass = g4elements->at(j)->GetAtomicMassAmu();

            material.elements.push_back(element);
        }

        geant_material_table.add_material(g4material_cuts->GetIndex(),
                                          material);
    }

    // Loop over volumes
    loop_volumes(geant_material_table, world_volume->GetLogicalVolume());

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
    store_material_table(&root_output,
                         G4ProductionCutsTable::GetProductionCutsTable(),
                         world_phys_volume);

    root_output.Close();

    cout << root_output_filename << " done!" << endl;
    return EXIT_SUCCESS;
}
