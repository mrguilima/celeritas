//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantProcess.hh
//! \brief "Enumerator class" for the physics processes
//---------------------------------------------------------------------------//
#pragma once

#include <string>

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * This enum does not exists in Geant4. It was created to safely access the
 * many physics tables imported from it.
 * Geant4 only provides the process names as strings. This class provides an
 * interface to select an enum from a string, while still working as an enum:
 *
 * \code
 *  GeantProcess my_process("msc")
 *  if (my_process.get() == GeantProcess::msc) { ... }
 * \endcode
 *
 * FOR REVIEW: The enum keeps the Geant4 names, not following Celeritas code
 * standards.
 */
class GeantProcess
{
  public:
    enum Value
    {
        // EM process list
        not_defined,
        ionIoni,
        msc,
        hIoni,
        hBrems,
        hPairProd,
        CoulombScat,
        eIoni,
        eBrem,
        phot,
        compt,
        conv,
        Rayl,
        muIoni,
        muBrems,
        muPairProd
    };

  public:
    // Constructors
    GeantProcess() : value_(Value::not_defined) {}
    GeantProcess(std::string process_name)
    {
        GeantProcess p;
        p(process_name);
        this->value_ = p.value_;
    }
    GeantProcess(Value process) : value_(process) {}

    // Default destructor
    ~GeantProcess() = default;

    // Fetch enum value
    Value get() { return this->value_; }

    // >>> OPERATORS
         operator Value() const { return value_; }
    void operator()(std::string process_name)
    {
        // clang-format off
        if (process_name == "not_defined") this->value_ = not_defined;
        if (process_name == "ionIoni")     this->value_ = ionIoni;
        if (process_name == "msc")         this->value_ = msc;
        if (process_name == "hIoni")       this->value_ = hIoni;
        if (process_name == "hBrems")      this->value_ = hBrems;
        if (process_name == "hPairProd")   this->value_ = hPairProd;
        if (process_name == "CoulombScat") this->value_ = CoulombScat;
        if (process_name == "eIoni")       this->value_ = eIoni;
        if (process_name == "eBrem")       this->value_ = eBrem;
        if (process_name == "phot")        this->value_ = phot;
        if (process_name == "compt")       this->value_ = compt;
        if (process_name == "conv")        this->value_ = conv;
        if (process_name == "Rayl")        this->value_ = Rayl;
        if (process_name == "muIoni")      this->value_ = muIoni;
        if (process_name == "muBrems")     this->value_ = muBrems;
        if (process_name == "muPairProd")  this->value_ = muPairProd;
        // clang-format on
    }

  protected:
    Value value_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas