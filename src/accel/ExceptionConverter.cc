//----------------------------------*-C++-*----------------------------------//
// Copyright 2022-2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file accel/ExceptionConverter.cc
//---------------------------------------------------------------------------//
#include "ExceptionConverter.hh"

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <G4Exception.hh>

#include "celeritas_config.h"
#include "corecel/Assert.hh"
#include "corecel/Macros.hh"
#include "corecel/io/Logger.hh"
#include "corecel/sys/Environment.hh"

namespace celeritas
{
namespace
{
//---------------------------------------------------------------------------//
bool determine_strip()
{
    if (!celeritas::getenv("CELER_STRIP_SOURCEDIR").empty())
    {
        return true;
    }
    return static_cast<bool>(CELERITAS_DEBUG);
}

//---------------------------------------------------------------------------//
//! Try removing up to and including the filename from the reported path.
std::string strip_source_dir(std::string const& filename)
{
    static bool const do_strip = determine_strip();
    if (!do_strip)
    {
        // Don't strip in debug mode
        return filename;
    }

    std::string::size_type max_pos = 0;
    for (const std::string path : {"src/", "app/", "test/"})
    {
        auto pos = filename.rfind(path);

        if (pos != std::string::npos)
        {
            pos += path.size() - 1;
            max_pos = std::max(max_pos, pos);
        }
    }
    if (max_pos == 0)
    {
        // No telling where the filename is from...
        return filename;
    }

    return filename.substr(max_pos + 1);
}

//---------------------------------------------------------------------------//
}  // namespace

//---------------------------------------------------------------------------//
/*!
 * Capture the current exception and convert it to a G4Exception call.
 */
void ExceptionConverter::operator()(std::exception_ptr eptr) const
{
    try
    {
        std::rethrow_exception(eptr);
    }
    catch (RichContextException const& e)
    {
        CELER_LOG_LOCAL(critical)
            << "The following error is from: " << e.what();
        try
        {
            std::rethrow_if_nested(e);
        }
        catch (...)
        {
            return (*this)(std::current_exception());
        }
    }
    catch (RuntimeError const& e)
    {
        // Translate a runtime error into a G4Exception call
        std::ostringstream where;
        if (e.details().file)
        {
            where << strip_source_dir(e.details().file);
        }
        if (e.details().line != 0)
        {
            where << ':' << e.details().line;
        }
        G4Exception(where.str().c_str(),
                    err_code_,
                    FatalException,
                    e.details().what.c_str());
    }
    catch (DebugError const& e)
    {
        // Translate a *debug* error
        std::ostringstream where;
        where << strip_source_dir(e.details().file) << ':' << e.details().line;
        std::ostringstream what;
        what << to_cstring(e.details().which) << ": " << e.details().condition;
        G4Exception(
            where.str().c_str(), err_code_, FatalException, what.str().c_str());
    }
    catch (std::runtime_error const& e)
    {
        this->convert_device_exceptions(std::current_exception());
    }
    // (Any other errors will be rethrown and abort the program.)
}

#if !CELER_USE_DEVICE
//---------------------------------------------------------------------------//
/*!
 * No other exceptions are caught when device code is disabled.
 *
 * See ExceptionConverter.cu for the CUDA implementation of this.
 */
inline void
ExceptionConverter::convert_device_exceptions(std::exception_ptr eptr) const
{
    std::rethrow_exception(eptr);
}

#endif
//---------------------------------------------------------------------------//
}  // namespace celeritas
