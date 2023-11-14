//                                               -*- C -*-
/**
 *  @brief The header file of OpenTURNS for debug level
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_OTDEBUG_H
# define OPENTURNS_OTDEBUG_H

# include <stdlib.h> /* POSIX: for inclusion of <features.h> in GNU glibc systems */

/* BEGIN_C_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names.  Use END_C_DECLS at
   the end of C declarations. */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#ifdef __cplusplus
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS }
#else
# define BEGIN_C_DECLS /* empty */
# define END_C_DECLS /* empty */
#endif

# ifdef __GNUC__

#  ifdef SWIG
#   define DEPRECATED
#   define UNUSED
#   define NOTHROW
#  else /* not SWIG */
#   define DEPRECATED  __attribute__ ((deprecated))
#   define UNUSED      __attribute__ ((unused))
#   define NOTHROW     __attribute__ ((nothrow))
#  endif /* SWIG */

# else /* not __GNUC_ */

#  define DEPRECATED
#  define UNUSED
#  define NOTHROW

# endif /* __GNUC_ */

/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define OT_HELPER_DLL_IMPORT __declspec(dllimport)
#define OT_HELPER_DLL_EXPORT __declspec(dllexport)
#define OT_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OT_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define OT_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define OT_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define OT_HELPER_DLL_IMPORT
#define OT_HELPER_DLL_EXPORT
#define OT_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define OT_API and OT_LOCAL.
 * OT_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * OT_LOCAL is used for non-api symbols. */

#ifndef OT_STATIC /* defined if OT is compiled as a DLL */
#ifdef OT_DLL_EXPORTS /* defined if we are building the OT DLL (instead of using it) */
#define OT_API OT_HELPER_DLL_EXPORT
#else
#define OT_API OT_HELPER_DLL_IMPORT
#endif /* OT_DLL_EXPORTS */
#define OT_LOCAL OT_HELPER_DLL_LOCAL
#else /* OT_STATIC is defined: this means OT is a static lib. */
#define OT_API
#define OT_LOCAL
#endif /* !OT_STATIC */

#endif /* OPENTURNS_OTDEBUG_H */
