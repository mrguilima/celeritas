//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantGeometryMap.hh
//! \brief Store and map volume and material information
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
class GeantGeometryMap
{
  public:
    //@{
    //!
    // Should these be opaqueids?
    using mat_id = int;
    using vol_id = int;
    //@}

    // Construct/destruct with defaults
    GeantGeometryMap();
    ~GeantGeometryMap();

    // >>> READ
    // Find GeantMaterial given a material id
    GeantMaterial get_material(mat_id& material_id);
    // Find material id given volume id
    mat_id get_matid(vol_id& volume_id);
    // Find GeantVolume given volume id
    GeantVolume get_volume(vol_id& volume_id);

    // Only used in geant-exporter-cat. Not sure we need this...
    // Return a copy of private member volid_to_matid_
    std::map<vol_id, mat_id> volid_to_matid_map();

    // >>> WRITE
    // Add pair <mat_id, material> to the map
    void add_material(mat_id id, GeantMaterial material);
    // Add pair <vol_id, volume> to the map
    void add_volume(vol_id id, GeantVolume volume);
    // Add pair <vol_id, mat_id> to the map
    void link_volume_material(vol_id volid, mat_id matid);

  private:
    // Should we leave these public?
    std::map<mat_id, GeantMaterial> matid_to_material_;
    std::map<vol_id, GeantVolume>   volid_to_volume_;
    std::map<vol_id, mat_id>        volid_to_matid_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas