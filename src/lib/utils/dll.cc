// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (c) 2012 Association Prologin <association@prologin.org>
#include "dll.hh"
#include "log.hh"

#include <dlfcn.h>

namespace utils
{

DLLError::DLLError()
    : std::runtime_error(dlerror())
{
}

DLL::DLL(const std::string& filename)
    : filename_(filename)
    , handle_(dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL))
{
    CHECK_EXC(DLLError, handle_ != nullptr);
    if (handle_ == nullptr)
        WARN("Cannot open shared library: %d", filename.c_str());
}

DLL::~DLL()
{
    dlclose(handle_);
}

void* DLL::get_untyped(const std::string& sym)
{
    DEBUG("getting symbol %s in %s", sym.c_str(), filename_.c_str());

    void* sym_value = dlsym(handle_, sym.c_str());
    CHECK_EXC(DLLError, sym_value != nullptr);
    return sym_value;
}

} // namespace utils
