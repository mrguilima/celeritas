//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantImporter.test.cc
//---------------------------------------------------------------------------//
#include "io/GeantImporter.hh"
#include "io/GeantTableType.hh"
#include "io/GeantProcessType.hh"
#include "io/GeantProcess.hh"
#include "io/GeantModel.hh"
#include "physics/base/ParticleMd.hh"
#include "base/Types.hh"

#include "gtest/Main.hh"
#include "gtest/Test.hh"

using celeritas::real_type;

using celeritas::GeantImporter;
// Particles
using celeritas::GeantParticle;
using celeritas::ParticleDef;
using celeritas::ParticleDefId;
using celeritas::ParticleParams;
using celeritas::PDGNumber;
// Tables
using celeritas::GeantModel;
using celeritas::GeantPhysicsTable;
using celeritas::GeantPhysicsVectorType;
using celeritas::GeantProcess;
using celeritas::GeantProcessType;
using celeritas::GeantTableType;
// Materials and volumes
using celeritas::GeantGeometryMap;
using celeritas::GeantMaterial;
using celeritas::GeantVolume;

//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class GeantImporterTest : public celeritas::Test
{
  protected:
    void SetUp() override
    {
        root_filename_ = this->test_data_path("io", "geant-exporter-data.root");
    }

    std::string root_filename_;
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//
TEST_F(GeantImporterTest, import_particles)
{
    GeantImporter import(root_filename_.c_str());
    auto          data = import();

    EXPECT_EQ(19, data.particle_params->size());

    EXPECT_GE(data.particle_params->find(PDGNumber(11)).get(), 0);
    ParticleDefId electron_id = data.particle_params->find(PDGNumber(11));
    ParticleDef   electron    = data.particle_params->get(electron_id);

    EXPECT_SOFT_EQ(0.510998910, electron.mass.value());
    EXPECT_EQ(-1, electron.charge.value());
    EXPECT_EQ(0, electron.decay_constant);

    std::vector<std::string> loaded_names;
    std::vector<int>         loaded_pdgs;
    for (const auto& md : data.particle_params->md())
    {
        loaded_names.push_back(md.name);
        loaded_pdgs.push_back(md.pdg_code.get());
    }

    // clang-format off
    const std::string expected_loaded_names[] = {"gamma", "e+", "e-", "mu+",
        "mu-", "pi-", "pi+", "kaon-", "kaon+", "anti_proton", "proton",
        "anti_deuteron", "deuteron", "anti_He3", "He3", "anti_triton",
        "triton", "anti_alpha", "alpha"};
    const int expected_loaded_pdgs[] = {22, -11, 11, -13, 13, -211, 211, -321,
        321, -2212, 2212, -1000010020, 1000010020, -1000020030, 1000020030,
        -1000010030, 1000010030, -1000020040, 1000020040};
    // clang-format on

    EXPECT_VEC_EQ(expected_loaded_names, loaded_names);
    EXPECT_VEC_EQ(expected_loaded_pdgs, loaded_pdgs);
}

//---------------------------------------------------------------------------//
TEST_F(GeantImporterTest, import_tables)
{
    GeantImporter import(root_filename_.c_str());
    auto          data = import();

    EXPECT_GE(data.physics_tables->size(), 0);

    bool lambda_kn_gamma_table = false;

    for (auto table : *data.physics_tables)
    {
        EXPECT_GE(table.physics_vectors.size(), 0);

        if (table.particle == PDGNumber{celeritas::pdg::gamma()}
            && table.table_type == GeantTableType::Lambda
            && table.process == GeantProcess::compt
            && table.model == GeantModel::KleinNishina)
        {
            lambda_kn_gamma_table = true;
            break;
        }
    }
    EXPECT_TRUE(lambda_kn_gamma_table);
}

//---------------------------------------------------------------------------//
TEST_F(GeantImporterTest, import_geometry)
{
    GeantImporter import(root_filename_.c_str());
    auto          data = import();

    auto map = data.geometry->volid_to_matid_map();
    EXPECT_EQ(map.size(), 4257);

    // Fetch a given GeantVolume provided a vol_id
    GeantGeometryMap::vol_id volid  = 10;
    GeantVolume              volume = data.geometry->get_volume(volid);
    EXPECT_EQ(volume.name, "TrackerPatchPannel");

    // Fetch respective mat_id and GeantMaterial from the given vol_id
    GeantGeometryMap::mat_id matid    = data.geometry->get_matid(volid);
    GeantMaterial            material = data.geometry->get_material(matid);

    // Material
    EXPECT_EQ(matid, 31);
    EXPECT_EQ(material.name, "Air");
    EXPECT_SOFT_EQ(material.density, 0.00121399936124299);
    EXPECT_EQ(material.elements.size(), 4);

    // Elements within material
    std::string elements_name[4] = {"N", "O", "Ar", "H"};
    real_type   fraction[4]      = {0.7494, 0.2369, 0.0129, 0.0008};
    int         z_number[4]      = {7, 8, 18, 1};
    real_type   atomic_mass[4]
        = {14.00676896, 15.999390411, 39.94769335110001, 1.007940752665138};

    for (size_t i = 0; i < 4; i++)
    {
        auto elid         = material.elements.at(i);
        auto element      = data.geometry->get_element(elid);
        auto fraction_map = material.fractions.find(elid);

        EXPECT_EQ(element.name, elements_name[i]);
        EXPECT_EQ(element.atomic_number, z_number[i]);
        EXPECT_SOFT_EQ(element.atomic_mass, atomic_mass[i]);
        EXPECT_SOFT_EQ(fraction_map->second, fraction[i]);
    }
}