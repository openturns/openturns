//                                               -*- C++ -*-
/**
 *  @brief The header file that declares all general types
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OTTYPES_HXX
#define OPENTURNS_OTTYPES_HXX

#include <string>                // for std::string
#include <complex>               // for std::complex
#include <stdint.h>              // for u_int64_t

/**
 * @namespace  OpenTURNS
 * @brief      The namespace where all OpenTURNS symbols live.
 */

#define BEGIN_NAMESPACE_OPENTURNS namespace OT {
#define END_NAMESPACE_OPENTURNS   }

BEGIN_NAMESPACE_OPENTURNS

/**
 * Type Bool is an alias for the C++ standard bool type
 */
typedef bool Bool;

/**
 * Type String should store Unicode strings (for later internationalization)
 */
typedef std::string String;

/**
 * Type Id is an unique identifier number for PersistentObjects
 */
typedef unsigned long Id;

/**
 * Type Scalar is a floating point number representation
 */
typedef double Scalar;

/**
 * Type Complex is a complex floating point number representation
 */
typedef std::complex<Scalar> Complex;

/**
 * Type FileName stores file names and pathes
 */
typedef std::string FileName;

/**
 * Type UnsignedInteger stores unsigned integers.
 */
typedef unsigned long UnsignedInteger;

/**
 * Type SignedInteger stores signed integers.
 */
typedef long SignedInteger;

/**
 * Type Unsigned64BitsInteger stores unsigned values that needs 64 bits word length.
 */
typedef uint64_t Unsigned64BitsInteger;

/**
 * Type Signed64BitsInteger stores signed values that needs 64 bits word length.
 */
typedef int64_t Signed64BitsInteger;

/**
 * Type LibraryHandle stores dynamic library pointers
 */
typedef void * LibraryHandle;

/**
 * Type LibrarySymbol stores pointers library symbols
 */
typedef void * LibrarySymbol;

/**
 * The types for utility functions
 */
typedef void (*LogFunctionPointer) (const String &);


END_NAMESPACE_OPENTURNS

/**
 * An alias to namespace OT.
 * @sa         OT
 */
namespace OpenTURNS = OT;


#endif /* OPENTURNS_OTTYPES_HXX */
