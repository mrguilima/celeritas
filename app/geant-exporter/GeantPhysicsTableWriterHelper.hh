//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantPhysicsTableWriterHelper.hh
//! \brief Helper functions to safely convert physics tables enums
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <unordered_map>

#include <G4PhysicsVectorType.hh>
#include <G4ProcessType.hh>
#include <G4Material.hh>

#include "io/GeantTableType.hh"
#include "io/GeantProcessType.hh"
#include "io/GeantProcess.hh"
#include "io/GeantPhysicsVectorType.hh"
#include "io/GeantModel.hh"
#include "io/GeantMaterial.hh"
#include "base/Assert.hh"
#include "base/Types.hh"

using celeritas::GeantTableType;
using celeritas::GeantPhysicsVectorType;
using celeritas::GeantProcessType;
using celeritas::GeantProcess;
using celeritas::GeantModel;
using celeritas::GeantMaterialState;

namespace geant_exporter
{
//---------------------------------------------------------------------------//
/*!
 * Safely retrieve the correct table type enum from a given string.
 */
GeantTableType to_geant_table_type(const std::string& g4_table_type_name)
{
    const static std::unordered_map<std::string, GeantTableType> table_map = {
        // clang-format off
        {"not_defined",  GeantTableType::not_defined},
        {"DEDX",         GeantTableType::dedx},
        {"Ionisation",   GeantTableType::ionisation},
        {"Range",        GeantTableType::range},
        {"RangeSec",     GeantTableType::range_sec},
        {"InverseRange", GeantTableType::inverse_range},
        {"Lambda",       GeantTableType::lambda},
        {"LambdaPrim",   GeantTableType::lambda_prim},
        {"LambdaMod1",   GeantTableType::lambda_mod_1},
        {"LambdaMod2",   GeantTableType::lambda_mod_2},
        {"LambdaMod3",   GeantTableType::lambda_mod_3},
        {"LambdaMod4",   GeantTableType::lambda_mod_4},
        // clang-format on
    };
    auto iter = table_map.find(g4_table_type_name);
    CHECK(iter != table_map.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Safely switch from G4PhysicsVectorType to GeantPhysicsVectorType.
 * [See G4PhysicsVectorType.hh]
 */
GeantPhysicsVectorType
to_geant_physics_vector_type(const G4PhysicsVectorType g4_vector_type)
{
    switch (g4_vector_type)
    {
        case G4PhysicsVectorType::T_G4PhysicsVector:
            return GeantPhysicsVectorType::base;
        case G4PhysicsVectorType::T_G4PhysicsLinearVector:
            return GeantPhysicsVectorType::linear;
        case G4PhysicsVectorType::T_G4PhysicsLogVector:
            return GeantPhysicsVectorType::log;
        case G4PhysicsVectorType::T_G4PhysicsLnVector:
            return GeantPhysicsVectorType::ln;
        case G4PhysicsVectorType::T_G4PhysicsFreeVector:
            return GeantPhysicsVectorType::free;
        case G4PhysicsVectorType::T_G4PhysicsOrderedFreeVector:
            return GeantPhysicsVectorType::ordered_free;
        case G4PhysicsVectorType::T_G4LPhysicsFreeVector:
            return GeantPhysicsVectorType::low_energy_free;
    }
    CHECK(false);
}

//---------------------------------------------------------------------------//
/*!
 * Safely switch from G4PhysicsVectorType to GeantPhysicsVectorType.
 * [See G4PhysicsVectorType.hh]
 */
GeantProcessType to_geant_process_type(const G4ProcessType g4_process_type)
{
    switch (g4_process_type)
    {
        case G4ProcessType::fNotDefined:
            return GeantProcessType::not_defined;
        case G4ProcessType::fTransportation:
            return GeantProcessType::transportation;
        case G4ProcessType::fElectromagnetic:
            return GeantProcessType::electromagnetic;
        case G4ProcessType::fOptical:
            return GeantProcessType::optical;
        case G4ProcessType::fHadronic:
            return GeantProcessType::hadronic;
        case G4ProcessType::fPhotolepton_hadron:
            return GeantProcessType::photolepton_hadron;
        case G4ProcessType::fDecay:
            return GeantProcessType::decay;
        case G4ProcessType::fGeneral:
            return GeantProcessType::general;
        case G4ProcessType::fParameterisation:
            return GeantProcessType::parameterisation;
        case G4ProcessType::fUserDefined:
            return GeantProcessType::user_defined;
        case G4ProcessType::fParallel:
            return GeantProcessType::parallel;
        case G4ProcessType::fPhonon:
            return GeantProcessType::phonon;
        case G4ProcessType::fUCN:
            return GeantProcessType::ucn;
    }
    CHECK(false);
}

//---------------------------------------------------------------------------//
/*!
 * Safely retrieve the correct process enum from a given string.
 */
GeantProcess to_geant_process(const std::string& g4_process_name)
{
    const static std::unordered_map<std::string, GeantProcess> process_map = {
        // clang-format off
        {"not_defined",    GeantProcess::not_defined},
        {"ionIoni",        GeantProcess::ion_ioni},
        {"msc",            GeantProcess::msc},
        {"hIoni",          GeantProcess::h_ioni},
        {"hBrems",         GeantProcess::h_brems},
        {"hPairProd",      GeantProcess::h_pair_prod},
        {"CoulombScat",    GeantProcess::coulomb_scat},
        {"eIoni",          GeantProcess::e_ioni},
        {"eBrem",          GeantProcess::e_brem},
        {"phot",           GeantProcess::photoelectric},
        {"compt",          GeantProcess::compton},
        {"conv",           GeantProcess::conversion},
        {"Rayl",           GeantProcess::rayleigh},
        {"annihil",        GeantProcess::annihilation},
        {"muIoni",         GeantProcess::mu_ioni},
        {"muBrems",        GeantProcess::mu_brems},
        {"muPairProd",     GeantProcess::mu_pair_prod},
        {"Transportation", GeantProcess::transportation},
        // clang-format on
    };
    auto iter = process_map.find(g4_process_name);
    CHECK(iter != process_map.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Safely retrieve the correct model enum from a given string.
 */
GeantModel to_geant_model(const std::string& g4_model_name)
{
    const static std::unordered_map<std::string, GeantModel> model_map = {
        // clang-format off
        {"not_defined",         GeantModel::not_defined},
        {"BraggIon",            GeantModel::bragg_ion},
        {"BetheBloch",          GeantModel::bethe_bloch},
        {"UrbanMsc",            GeantModel::urban_msc},
        {"ICRU73QO",            GeantModel::icru_73_qo},
        {"WentzelVIUni",        GeantModel::wentzel_VI_uni},
        {"hBrem",               GeantModel::h_brem},
        {"hPairProd",           GeantModel::h_pair_prod},
        {"eCoulombScattering",  GeantModel::e_coulomb_scattering},
        {"Bragg",               GeantModel::bragg},
        {"MollerBhabha",        GeantModel::moller_bhabha},
        {"eBremSB",             GeantModel::e_brem_sb},
        {"eBremLPM",            GeantModel::e_brem_lpm},
        {"eplus2gg",            GeantModel::e_plus_to_gg},
        {"LivermorePhElectric", GeantModel::livermore_photoelectric},
        {"Klein-Nishina",       GeantModel::klein_nishina},
        {"BetheHeitlerLPM",     GeantModel::bethe_heitler_lpm},
        {"LivermoreRayleigh",   GeantModel::livermore_rayleigh},
        {"MuBetheBloch",        GeantModel::mu_bethe_bloch},
        {"MuBrem",              GeantModel::mu_brem},
        {"muPairProd",          GeantModel::mu_pair_prod},
        // clang-format on
    };
    auto iter = model_map.find(g4_model_name);
    CHECK(iter != model_map.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
} // namespace geant_exporter