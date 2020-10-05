//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantMaterialTable.cc
//---------------------------------------------------------------------------//
#include "GeantMaterialTable.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Return the GeantMaterial associated with the mat_id
 */
GeantMaterial GeantMaterialTable::get_material(mat_id& material_id)
{
    auto iter = matid_to_material_.find(material_id);
    REQUIRE(iter != matid_to_material_.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Return the mat_id for a given vol_id
 */
GeantMaterialTable::mat_id GeantMaterialTable::get_matid(vol_id& volume_id)
{
    auto iter = volid_to_matid_.find(volume_id);
    REQUIRE(iter != volid_to_matid_.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Return the GeantVolume associated with the vol_id
 */
GeantVolume GeantMaterialTable::get_volume(vol_id& volume_id)
{
    auto iter = volid_to_volume_.find(volume_id);
    REQUIRE(iter != volid_to_volume_.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Return the list of valid vol_ids
 */
std::vector<GeantMaterialTable::vol_id> GeantMaterialTable::vol_id_list()
{
    std::vector<vol_id>                vol_ids;
    std::map<vol_id, mat_id>::iterator iter;

    for (iter = volid_to_matid_.begin(); iter != volid_to_matid_.end(); iter++)
    {
        vol_ids.push_back(iter->first);
    }
    return vol_ids;
}

//---------------------------------------------------------------------------//
/*!
 * Return the list of valid mat_ids
 */
std::vector<GeantMaterialTable::mat_id> GeantMaterialTable::mat_id_list()
{
    std::vector<mat_id>                mat_ids;
    std::map<vol_id, mat_id>::iterator iter;

    for (iter = volid_to_matid_.begin(); iter != volid_to_matid_.end(); iter++)
    {
        mat_ids.push_back(iter->second);
    }
    return mat_ids;
}

//---------------------------------------------------------------------------//
/*!
 * Add pairs <mat_id, material> and <material.name, mat_id>
 */
void GeantMaterialTable::add_material(mat_id id, GeantMaterial material)
{
    matid_to_material_.insert(std::pair<mat_id, GeantMaterial>(id, material));
    name_to_matid_.insert(std::pair<std::string, mat_id>(material.name, id));
}

//---------------------------------------------------------------------------//
/*!
 * Add pairs <vol_id, volume> and <volume.name, vol_id>
 */
void GeantMaterialTable::add_volume(vol_id id, GeantVolume volume)
{
    volid_to_volume_.insert(std::pair<vol_id, GeantVolume>(id, volume));
    name_to_volid_.insert(std::pair<std::string, vol_id>(volume.name, id));
}

//---------------------------------------------------------------------------//
/*!
 * Link vol_id with mat_id
 */
void GeantMaterialTable::link_volume_material(vol_id& volid, mat_id& matid)
{
    volid_to_matid_.insert(std::pair<vol_id, mat_id>(volid, matid));
}

//---------------------------------------------------------------------------//
} // namespace celeritas