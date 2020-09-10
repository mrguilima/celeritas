//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file GeantParticleLinkDef.h
//! \brief Defines which classes should be added to the ROOT dictionary
//---------------------------------------------------------------------------//
#ifdef __CINT__

// clang-format off
#pragma link C++ class celeritas::GeantParticle+;
#pragma link C++ class celeritas::GeantPhysicsTable+;
#pragma link C++ class celeritas::GeantPhysicsVector+;
#pragma link C++ class celeritas::GeantModel+;
#pragma link C++ class celeritas::GeantProcess+;
#pragma link C++ class celeritas::GeantTableType+;
// clang-format on

#endif
