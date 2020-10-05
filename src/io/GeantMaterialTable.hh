//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantMaterialTable.hh
//! \brief Store material index information for the GeantPhysicsTable
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <map>

#include "GeantMaterial.hh"
#include "GeantVolume.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store material and volume information. The material id represents the
 * position of said material in the vector<GeantPhysicsVector> of the
 * GeantPhysicsTable. The volume id links a geometry volume with its respective
 * material id for lookup.
 */
class GeantMaterialTable
{
  public:
    //@{
    //!
    // Should these be opaqueids?
    using mat_id = int;
    using vol_id = int;
    //@}

    GeantMaterialTable()  = default;
    ~GeantMaterialTable() = default;

    // >>> READ
    // Find GeantMaterial given a material id
    GeantMaterial get_material(mat_id& material_id);
    // Find material id given volume id
    mat_id get_matid(vol_id& volume_id);
    // Find GeantVolume given volume id
    GeantVolume get_volume(vol_id& volume_id);

    // Return list of volume ids
    std::vector<vol_id> vol_id_list();
    // Return list of material ids
    std::vector<mat_id> mat_id_list();

    // >>> WRITE (used only for export/import)
    // Add pairs <mat_id, material> and <material.name, mat_id>
    void add_material(mat_id id, GeantMaterial material);
    // Add pairs <vol_id, volume> and <volume.name, vol_id>
    void add_volume(vol_id id, GeantVolume volume);
    // Link a given volume id with a given material id
    void link_volume_material(vol_id& volid, mat_id& matid);

  private:
    // Material
    std::map<std::string, mat_id>   name_to_matid_;
    std::map<mat_id, GeantMaterial> matid_to_material_;
    // Volume
    std::map<std::string, vol_id> name_to_volid_;
    std::map<vol_id, GeantVolume> volid_to_volume_;
    // Volume to material
    std::map<vol_id, mat_id> volid_to_matid_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas