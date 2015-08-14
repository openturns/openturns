//                                               -*- C++ -*-
/**
 *  @brief This file defines a funtion to bind OpenTURNS functions to the wrapper
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* IMPORTANT NOTE
 * ==============
 *
 * This file defines a function BIND_METHODS_FUNCTION whose actual
 * name is set in WrapperMacros.h as __wrapper_bind_methods.
 *
 * This function is embedded in a static library called libOTbind.a
 * located in the standard OpenTURNS installation directory. This
 * library, and the function, have to be linked with every wrapper for
 * normal operation. This is MANDATORY.
 *
 * Why do we need such a function ?
 * The reason comes from incompatible design choices between OpenTURNS
 * and Python.
 * OpenTURNS needs to dlopen libraries (aka wrappers) having
 * unresolved symbols, those ones being available in libOT.so (aka the
 * main OpenTURNS library). This is some kind of callback. In order to
 * be effective, libOT.so has to export its symbols to every
 * subsequent library it dlopens. This happens in two situations:
 *  a. when the library is linked with an executable, in which case
 *     the linker automatically sets the RTLD_GLOBAL flag (man
 *     dlopen(3)).
 *  b. when we manually dlopen libOT.so with this RTLS_GLOBAL flag.
 *
 * Citation from http://doc.python.org/:
 * "Portability therefore requires not to make any assumptions about
 * symbol visibility."
 *
 * But Python has made some different choice and its modules are
 * loaded with the RTLD_LOCAL flag (with is the opposite of
 * RTLD_GLOBAL) and the libOT.so can't export its symbols anymore to
 * the wrappers. So these latters can't bind their unresolved symbols
 * and crash at load time.
 *
 * Different implementations and workaround have been experimented and
 * we won't tell about them here.
 *
 * This last implementation is the one proposed by Python (see
 * http://docs.python.org/extending/extending.html#providing-a-c-api-for-an-extension-module)
 * as a Capsule or something like that (we don't actually use a
 * capsule that is too Python specific).
 *
 * The main idea is the following:
 *  1. libOT.so (in WrapperObject.cxx) stores in a static array the
 *     list of function pointers that need to be resolved by the
 *     wrapper.
 *  2. This array is passed to the BIND_METHODS_FUNCTION stores in the
 *     wrapper (remember it is statically linked to it).
 *  3. This function scans the array and manually sets the function
 *     pointers in the wrapper, resolving the symbols as the linker
 *     do.
 * For this to run smoothly, the scan of the array and the resolution
 * of symbols are embedded in C macros defined in WrapperCommon_macros.h
 */

#include "OTconfig.hxx" // Only needed for test wrappers
#include "OTdebug.h"
#include "WrapperInterface.h"
#include "WrapperCommon_macros.h"
#include "WrapperCommon_static.h"
#include "WrapperMacros.h"

OT_API RETURNCODE BIND_METHODS_FUNCTION_HIDDEN ( METHODS methods[] )
{
  static int first = 1;

  if (first) {
    if (methods == 0) return WRAPPER_BIND_ERROR;
    first = 0;

#undef BIND_ACTION
#define BIND_ACTION(rtype, name, args) name = (rtype (*) args) *methods++;
    BIND_METHODS;
  }

  return WRAPPER_OK;
}

#undef BIND_ACTION
#define BIND_ACTION(rtype, name, args) rtype (* name) args = 0;
BIND_METHODS;
