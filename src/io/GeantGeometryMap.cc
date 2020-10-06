//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantGeometryMap.cc
//---------------------------------------------------------------------------//
#include "GeantGeometryMap.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct/destruct with defaults
 */
GeantGeometryMap::GeantGeometryMap()  = default;
GeantGeometryMap::~GeantGeometryMap() = default;

//---------------------------------------------------------------------------//
/*!
 * Return the GeantMaterial associated with the mat_id
 */
GeantMaterial GeantGeometryMap::get_material(mat_id& material_id)
{
    auto iter = matid_to_material_.find(material_id);
    REQUIRE(iter != matid_to_material_.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Return the mat_id for a given vol_id
 */
GeantGeometryMap::mat_id GeantGeometryMap::get_matid(vol_id& volume_id)
{
    auto iter = volid_to_matid_.find(volume_id);
    REQUIRE(iter != volid_to_matid_.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Return the GeantVolume associated with the vol_id
 */
GeantVolume GeantGeometryMap::get_volume(vol_id& volume_id)
{
    auto iter = volid_to_volume_.find(volume_id);
    REQUIRE(iter != volid_to_volume_.end());
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Return a copy of private member volid_to_matid_
 */
std::map<GeantGeometryMap::vol_id, GeantGeometryMap::mat_id>
GeantGeometryMap::volid_to_matid_map()
{
    return volid_to_matid_;
}

//---------------------------------------------------------------------------//
/*!
 * Add pair <mat_id, GeantMaterial> to the matid_to_material_ map
 */
void GeantGeometryMap::add_material(mat_id id, GeantMaterial material)
{
    matid_to_material_.insert(std::pair<mat_id, GeantMaterial>(id, material));
}

//---------------------------------------------------------------------------//
/*!
 * Add pair <vol_id, volume> to the volid_to_volume_ map
 */
void GeantGeometryMap::add_volume(vol_id id, GeantVolume volume)
{
    volid_to_volume_.insert(std::pair<vol_id, GeantVolume>(id, volume));
}

//---------------------------------------------------------------------------//
/*!
 * Add pair <vol_id, mat_id> to the volid_to_matid_ map.
 * This links geometry and material information
 */
void GeantGeometryMap::link_volume_material(vol_id volid, mat_id matid)
{
    volid_to_matid_.insert(std::pair<vol_id, mat_id>(volid, matid));
}

//---------------------------------------------------------------------------//
} // namespace celeritas