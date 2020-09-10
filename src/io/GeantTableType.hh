//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantTableType.hh
//! \brief Enumerator for the physics table types
//---------------------------------------------------------------------------//
#pragma once

#include <string>

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * This enum does not exists in Geant4. It was created to safely access the
 * many physics tables imported from it.
 * Geant4 has table type names hardcoded as strings. This class provides an
 * interface to select an enum from a string, while working as an enum:
 *
 * \code
 *  GeantTableType table("Lambda")
 *  if (table == GeantTableType::Lambda) { ... }
 * \endcode
 *
 * Main reason for this wrapper is to retrieve Geant4 physics tables. Otherwise
 * it could have been a simple enum.
 *
 * FOR REVIEW: The enum keeps the Geant4 names, not following Celeritas code
 * standards.
 */
class GeantTableType
{
  public:
    enum Value
    {
        // Geant4 table types
        not_defined,
        DEDX,
        Ionisation,
        Range,
        RangeSec,
        InverseRange,
        Lambda,
        LambdaPrim,
        LambdaMod1,
        LambdaMod2,
        LambdaMod3,
        LambdaMod4
    };

  public:
    // Constructors
    GeantTableType() : value_(Value::not_defined) {}
    GeantTableType(std::string table_name)
    {
        GeantTableType table;
        table(table_name);
        this->value_ = table.value_;
    }
    GeantTableType(Value table) : value_(table) {}

    // Default destructor
    ~GeantTableType() = default;

    // Fetch enum value
    Value get() { return this->value_; }

    // >>> OPERATORS
         operator Value() const { return value_; }
    void operator()(std::string table_name)
    {
        // clang-format off
        if (table_name == "not_defined")  this->value_ = not_defined;
        if (table_name == "DEDX")         this->value_ = DEDX;
        if (table_name == "Ionisation")   this->value_ = Ionisation;
        if (table_name == "Range")        this->value_ = Range;
        if (table_name == "RangeSec")     this->value_ = RangeSec;
        if (table_name == "InverseRange") this->value_ = InverseRange;
        if (table_name == "Lambda")       this->value_ = Lambda;
        if (table_name == "LambdaPrim")   this->value_ = LambdaPrim;
        if (table_name == "LambdaMod1")   this->value_ = LambdaMod1;
        if (table_name == "LambdaMod2")   this->value_ = LambdaMod2;
        if (table_name == "LambdaMod3")   this->value_ = LambdaMod3;
        if (table_name == "LambdaMod4")   this->value_ = LambdaMod4;
        // clang-format on
    }

  protected:
    Value value_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas