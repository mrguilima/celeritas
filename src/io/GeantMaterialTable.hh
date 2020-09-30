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

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Store material information. The material index is the position of said
 * material in the vector<GeantPhysicsVector> in the GeantPhysicsTable
 */
class GeantMaterialTable
{
  public:
    //@{
    //!
    using mat_id = int;
    //@}

    GeantMaterialTable()  = default;
    ~GeantMaterialTable() = default;

    void add(mat_id index, GeantMaterial material)
    {
        index_to_material_.insert(
            std::pair<mat_id, GeantMaterial>(index, material));
        name_to_index_.insert(
            std::pair<std::string, mat_id>(material.name, index));
    }

    mat_id find(const std::string& material_name)
    {
        auto iter = name_to_index_.find(material_name);
        REQUIRE(iter != name_to_index_.end());
        return iter->second;
    }

    GeantMaterial find(mat_id material_index)
    {
        auto iter = index_to_material_.find(material_index);
        REQUIRE(iter != index_to_material_.end());
        return iter->second;
    }

  private:
    std::map<std::string, mat_id>   name_to_index_;
    std::map<mat_id, GeantMaterial> index_to_material_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas