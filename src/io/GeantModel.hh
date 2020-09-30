//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantModel.hh
//! \brief Enumerator for the available physics models
//---------------------------------------------------------------------------//
#pragma once

#include <string>

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * This enum does not exists in Geant4. It was created to safely access the
 * many physics tables imported from it.
 * Geant4 has model names hardcoded as strings. This class provides an
 * interface to select an enum from a string, while still working as an enum:
 *
 * \code
 *  GeantModel model("KleinNishina")
 *  if (model.get() == GeantModel::KleinNishina) { ... }
 * \endcode
 *
 * Only reason for this wrapper is to retrieve Geant4 physics tables. Otherwise
 * it would have been a simple enum.
 *
 * FOR REVIEW: The enum keeps the Geant4 names, not following Celeritas code
 * standards.
 */
class GeantModel
{
  public:
    enum Value
    {
        // EM model list
        not_defined,
        BraggIon,
        BetheBloch,
        UrbanMsc,
        ICRU73QO,
        WentzelVIUni,
        hBrem,
        hPairProd,
        eCoulombScattering,
        Bragg,
        MollerBhabha,
        eBremSB,
        eBremLPM,
        LivermorePhElectric,
        KleinNishina,
        BetheHeitlerLPM,
        LivermoreRayleigh,
        MuBetheBloch,
        MuBrem,
        muPairProd
    };

  public:
    // Constructors
    GeantModel() : value_(Value::not_defined) {}
    GeantModel(std::string model_name)
    {
        GeantModel model;
        model(model_name);
        this->value_ = model.value_;
    }
    GeantModel(Value model) : value_(model) {}

    // Default destructor
    ~GeantModel() = default;

    // Fetch enum value
    Value get() { return this->value_; }

    // >>> OPERATORS
         operator Value() const { return value_; }
    void operator()(std::string model_name)
    {
        // clang-format off
        if (model_name == "not_defined")         this->value_ = not_defined;
        if (model_name == "BraggIon")            this->value_ = BraggIon;
        if (model_name == "BetheBloch")          this->value_ = BetheBloch;
        if (model_name == "UrbanMsc")            this->value_ = UrbanMsc;
        if (model_name == "ICRU73QO")            this->value_ = ICRU73QO;
        if (model_name == "WentzelVIUni")        this->value_ = WentzelVIUni;
        if (model_name == "hBrem")               this->value_ = hBrem;
        if (model_name == "hPairProd")           this->value_ = hPairProd;
        if (model_name == "eCoulombScattering")  this->value_ =
                                                    eCoulombScattering;
        if (model_name == "Bragg")               this->value_ = Bragg;
        if (model_name == "MollerBhabha")        this->value_ = MollerBhabha;
        if (model_name == "eBremSB")             this->value_ = eBremSB;
        if (model_name == "eBremLPM")            this->value_ = eBremLPM;
        if (model_name == "LivermorePhElectric") this->value_ =
                                                    LivermorePhElectric;
        if (model_name == "Klein-Nishina")       this->value_ = KleinNishina;
        if (model_name == "BetheHeitlerLPM")     this->value_ = BetheHeitlerLPM;
        if (model_name == "LivermoreRayleigh")   this->value_ =
                                                    LivermoreRayleigh;
        if (model_name == "MuBetheBloch")        this->value_ = MuBetheBloch;
        if (model_name == "MuBrem")              this->value_ = MuBrem;
        if (model_name == "muPairProd")          this->value_ = muPairProd;
        // clang-format on
    }

  protected:
    Value value_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas