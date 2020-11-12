//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file RayleighInteractor.i.hh
//---------------------------------------------------------------------------//

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct with shared and state data.
 */
CELER_FUNCTION
RayleighInteractor::RayleighInteractor(const RayleighInteractorPointers& shared,
                                       const ParticleTrackView& particle,
                                       const Real3&             inc_direction,
                                       SecondaryAllocatorView&  allocate,
                                       const MaterialTrackView& material)
    : shared_(shared)
    , inc_energy_(particle.energy().value())
    , inc_direction_(inc_direction)
    , allocate_(allocate)
    , material_(material)
{
    REQUIRE(inc_energy_ >= this->min_incident_energy()
            && inc_energy_ <= this->max_incident_energy());
    REQUIRE(particle.def_id() == shared_.gamma_id);
}

//---------------------------------------------------------------------------//
//! Sample outgoing photon direction using the G4LivermoreRayleighModel model.
template<class Engine>
CELER_FUNCTION Interaction RayleighInteractor::operator()(Engine& rng)
{
    printf("RayleighInteractor::operator() called!\n");

    const real_type incph_energy = inc_energy_.value();

    // Allocate space for secondary XXX (electron, multiple particles, ...)
    // Secondary* secondaries = this->allocate_(0); // XXX
    // if (secondaries == nullptr)
    // {
    //     // Failed to allocate space for a secondary
    //     return Interaction::from_failure();
    // }

    // TODO: proper initialization - how to get micro_xs?
    auto                   matview = material_.material_view();
    std::vector<real_type> storage(matview.max_element_components(), -1);
    // TODO: there is no MaterialParamsPointers to initialize RayleighMicroXs ?
    ElementSelector select_el(
        matview, RayleighMicroXs{host_mats, energy}, make_span(storage));

    select_elem_
        = {material_.material_view(), RayleighMicroXs{}, make_span(storage)};

    //.. Randomly select one element in the current material
    // const G4Element* elm = SelectRandomAtom(couple,particle,photonEnergy0);
    auto atomDef = ElementDef{select_elem_(rng)};
    int  Z       = atomDef.atomic_number();

    // Sample the angle of the scattered photon
    // G4ThreeVector photonDirection =
    //  GetAngularDistribution()->SampleDirection(aDynamicGamma, photonEnergy0,
    //                                               Z, couple->GetMaterial());
    // fParticleChange->ProposeMomentumDirection(photonDirection);
    //===

    // /** this code was commented out in Geant4
    // // absorption of low-energy gamma
    // if (photonEnergy0 <= lowEnergyLimit)
    //   {
    //     fParticleChange->ProposeTrackStatus(fStopAndKill);
    //     fParticleChange->SetProposedKineticEnergy(0.);
    //     fParticleChange->ProposeLocalEnergyDeposit(photonEnergy0);
    //     return ;
    //   }
    // */

    // Construct interaction for change to primary (incident) particle
    Interaction result;
    result.action      = Action::scattered;                     // XXX
    result.energy      = units::MevEnergy{inc_energy_.value()}; // XXX
    result.direction   = inc_direction_;
    // result.secondaries = {secondaries, 1}; // XXX
    result.secondaries = {secondaries, 0}; // XXX

    // // Save outgoing secondary data
    // secondaries[0].def_id    = shared_.electron_id; // XXX
    // secondaries[0].energy    = units::MevEnergy{0}; // XXX
    // secondaries[0].direction = {0, 0, 0};           // XXX

    // Cutoff for secondary production happens *after* the interaction code.
    return result;
}

//---------------------------------------------------------------------------//
} // namespace celeritas
