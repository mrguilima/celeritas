//----------------------------------*-C++-*----------------------------------//
// Copyright 2022-2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file demo-loop/LDemoIO.root.cc
//---------------------------------------------------------------------------//
#include "LDemoIO.hh"

#include <string>
#include <vector>
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>

#include "celeritas/global/ActionRegistry.hh"

namespace demo_loop
{
//---------------------------------------------------------------------------//
/*!
 * Store input information to the ROOT MC truth output file.
 */
void to_root(celeritas::RootFileManager& root_manager, LDemoArgs const& cargs)
{
    CELER_EXPECT(cargs);

    auto& args = const_cast<LDemoArgs&>(cargs);
    auto tree_input = root_manager.make_tree("input", "input");

    // Problem definition
    tree_input->Branch("geometry_filename", &args.geometry_filename);
    tree_input->Branch("physics_filename", &args.physics_filename);
    tree_input->Branch("hepmc3_filename", &args.hepmc3_filename);

    // Control
    tree_input->Branch("seed", &args.seed);
    tree_input->Branch("max_num_tracks", &args.max_num_tracks);
    tree_input->Branch("max_steps", &args.max_steps);
    tree_input->Branch("initializer_capacity", &args.initializer_capacity);
    tree_input->Branch("max_events", &args.max_events);
    tree_input->Branch("secondary_stack_factor", &args.secondary_stack_factor);
    tree_input->Branch("enable_diagnostics", &args.enable_diagnostics);
    tree_input->Branch("use_device", &args.use_device);
    tree_input->Branch("sync", &args.sync);
    tree_input->Branch("step_limiter", &args.step_limiter);

    // Options for physics processes and models
    tree_input->Branch("combined_brem", &args.brem_combined);

    // TODO Add magnetic field information?

    // Fill tree (writing happens at destruction)
    tree_input->Fill();
}

//---------------------------------------------------------------------------//
/*!
 * Store CoreParams data to the ROOT MC truth output file.
 *
 * \note
 * Currently only storing the action labels so their IDs can be identified. If
 * other parameters are needed for future debugging/analyses, this function can
 * easily be expanded.
 */
void to_root(celeritas::RootFileManager& root_manager,
             celeritas::CoreParams const& core_params)
{
    auto const& action_reg = *core_params.action_reg();

    // Initialize CoreParams TTree
    auto tree_params = root_manager.make_tree("core_params", "core_params");

    // Store labels
    std::vector<std::string> action_labels;
    action_labels.resize(action_reg.num_actions());
    for (auto const id : celeritas::range(action_reg.num_actions()))
    {
        action_labels[id] = action_reg.id_to_label(celeritas::ActionId{id});
    }

    // Set up action labels branch, fill the TTree and write it
    /*
     * The decision to store a vector instead of making a tree entry for
     * each label is to simplify the reading of the information. Calling
     * action_labels->at(action_id) after loading the first (and only) tree
     * entry is much simpler than:
     * tree->GetEntry(action_id);
     * tree->GetLeaf("action_label")->GetValue();
     */
    tree_params->Branch("action_labels", &action_labels);
    tree_params->Fill();  // Writing happens at destruction
}

//---------------------------------------------------------------------------//
}  // namespace demo_loop
