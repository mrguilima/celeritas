//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantGeometryMap.hh
//! \brief Store and link volume with material information
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
 * Store material, element, and volume information.
 *
 * - The material id maps materials in the global material map. It also
 *   represents the position of the material in the vector<GeantPhysicsVector>
 *   of the GeantPhysicsTable.
 * - The element id maps elements in the global element map.
 * - The volume id maps volumes in the global volume map.
 * - Volume id and material id are linked by a map, so that given a volume id
 *   one can retrieve full material/element information, including XS data.
 */
class GeantGeometryMap
{
  public:
    //@{
    //!
    using mat_id  = int;
    using vol_id  = int;
    using elem_id = int;
    //@}

    // Construct/destruct with defaults
    GeantGeometryMap();
    ~GeantGeometryMap();

    // >>> READ

    // Find material id given volume id
    mat_id get_matid(vol_id& volume_id);
    // Find GeantVolume given volume id
    GeantVolume get_volume(vol_id& volume_id);
    // Find GeantMaterial given a material id
    GeantMaterial get_material(mat_id& material_id);
    // Find GeantElement given element id
    GeantElement get_element(elem_id& element_id);

    // Only used in geant-exporter-cat. Not sure we need this...
    // Return a copy of private member volid_to_matid_
    std::map<vol_id, mat_id> volid_to_matid_map();

    // >>> WRITE (only used by geant-exporter app)

    // Add pair <mat_id, material> to the map
    void add_material(mat_id id, GeantMaterial material);
    // Add pair <vol_id, volume> to the map
    void add_volume(vol_id id, GeantVolume volume);
    // Add pair <elem_id, element> to the map
    void add_element(elem_id id, GeantElement element);
    // Add pair <vol_id, mat_id> to the map
    void link_volume_material(vol_id volid, mat_id matid);

  private:
    // Should we leave these public?
    // Global maps
    std::map<mat_id, GeantMaterial> matid_to_material_;
    std::map<vol_id, GeantVolume>   volid_to_volume_;
    std::map<elem_id, GeantElement> elemid_to_element_;
    // Link between volume and material
    std::map<vol_id, mat_id> volid_to_matid_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas