//----------------------------------*-C++-*----------------------------------//
// Copyright 2022-2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/global/ActionRegistryOutput.cc
//---------------------------------------------------------------------------//
#include "ActionRegistryOutput.hh"

#include <type_traits>
#include <utility>

#include "celeritas_config.h"
#include "corecel/cont/Range.hh"
#include "corecel/io/JsonPimpl.hh"

#include "ActionInterface.hh"
#include "ActionRegistry.hh"  // IWYU pragma: keep
#if CELERITAS_USE_JSON
#    include <nlohmann/json.hpp>
#endif

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Construct from a shared action manager.
 */
ActionRegistryOutput::ActionRegistryOutput(SPConstActionRegistry actions)
    : actions_(std::move(actions))
{
    CELER_EXPECT(actions_);
}

//---------------------------------------------------------------------------//
/*!
 * Write output to the given JSON object.
 */
void ActionRegistryOutput::output(JsonPimpl* j) const
{
#if CELERITAS_USE_JSON
    auto obj = nlohmann::json::array();
    for (auto id : range(ActionId{actions_->num_actions()}))
    {
        nlohmann::json entry{
            {"label", actions_->id_to_label(id)},
        };

        ActionInterface const& action = *actions_->action(id);
        auto&& desc = action.description();
        if (!desc.empty())
        {
            entry["description"] = std::move(desc);
        }
        obj.push_back(entry);
    }
    j->obj = std::move(obj);
#else
    (void)sizeof(j);
#endif
}

//---------------------------------------------------------------------------//
}  // namespace celeritas
