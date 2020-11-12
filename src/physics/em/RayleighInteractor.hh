//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file RayleighInteractor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Macros.hh"
#include "base/Types.hh"
#include "physics/base/Interaction.hh"
#include "physics/base/ParticleTrackView.hh"
#include "physics/base/SecondaryAllocatorView.hh"
#include "physics/base/Secondary.hh"
#include "physics/base/Units.hh"
#include "physics/material/MaterialTrackView.hh"
#include "physics/material/ElementSelector.hh"
#include "RayleighInteractorPointers.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Livermore model for the Rayleigh scattering of photons.
 *
 * Additional description
 *
 * \note This performs the same sampling routine as in Geant4's
 * G4LivermoreRayleigh class, as documented in section XXX of the Geant4
 * Physics Reference (release 10.6).
 */
class RayleighInteractor
{
    // Example functor for calculating cross section from actual atomic
    // properties and particle state
    struct RayleighMicroXs
    {
        RayleighMicroXs(const MaterialParamsPointers& mats,
                        units::MevEnergy              energy)
            : mats_(mats), inv_energy_(1 / energy.value())
        {
        }

        real_type operator()(ElementDefId el_id) const
        {
            REQUIRE(el_id);
            ElementView el(mats_, el_id);
            return el.cbrt_z() * inv_energy_;
        }

        const MaterialParamsPointers& mats_;
        real_type                     inv_energy_;
    };

  public:
    // Construct from shared and state data
    inline CELER_FUNCTION
    RayleighInteractor(const RayleighInteractorPointers& shared,
                       const ParticleTrackView&          particle,
                       const Real3&                      inc_direction,
                       SecondaryAllocatorView&           allocate,
                       const MaterialTrackView&          material);

    // Sample an interaction with the given RNG
    template<class Engine>
    inline CELER_FUNCTION Interaction operator()(Engine& rng);

    // >>> COMMON PROPERTIES

    //! Minimum incident energy for this model to be valid
    static CELER_CONSTEXPR_FUNCTION units::MevEnergy min_incident_energy()
    {
        return units::MevEnergy{0.000010}; // 10eV
    }

    //! Maximum incident energy for this model to be valid
    static CELER_CONSTEXPR_FUNCTION units::MevEnergy max_incident_energy()
    {
        return units::MevEnergy{1000.}; // 1 GeV
    }

  private:
    //! shared constant physics properties
    const RayleighInteractorPointers& shared_;

    //! Incident gamma energy
    const units::MevEnergy inc_energy_;

    //! Incident direction
    const Real3& inc_direction_;

    //! Allocate space for one or more secondary particles
    SecondaryAllocatorView& allocate_;

    //! Material
    const MaterialTrackView& material_;

    //! Atom selection utility
    // ElementSelector select_elem_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas

#include "RayleighInteractor.i.hh"
