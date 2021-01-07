//                                               -*- C++ -*-
/**
 *  @brief The internal header file of OpenTURNS
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

/**
 * @mainpage OpenTURNS API documentation
 *
 * This documentation is intented to be used by experienced programmers.
 * It shows the API of almost all the objects defined by the C++ core library
 * of OpenTURNS.
 *
 * There may be undocumented features or objects. This is a mistake. You can help
 * us improve the software by noticing us of these caveats or by documenting them yourself
 * as a friendly user.
 *
 * As visible either in the directory structure or in the namespace naming,
 * the library is divided in two main pieces:
 *  - the Base part
 *  - the Uncertainty part
 *
 * \section Base
 * The Base part of the library provides containers and algorithms of common usage, such
 * as multidimensional points (\link OT::Point Point\endlink),
 * aka vectors, samples
 * (\link OT::Sample Sample\endlink)
 * or functions (\link OT::Function Function\endlink).
 * Every concept in OpenTURNS is multidimensional unless clearly expressed or documented.
 *
 * \section Uncertainty
 * The Uncertainty part of the library provides objects and algorithms designed to achieve
 * probabilistic analyses. Here you will find things like distributions (\link OT::Distribution
 * Distribution\endlink or \link OT::Normal Normal\endlink),
 * random vectors (\link OT::RandomVector RandomVector\endlink), algorithms
 * (\link OT::FORM FORM\endlink or \link OT::MonteCarlo
 * MonteCarlo\endlink) or statistical tests (\link OT::FittingTest FittingTest\endlink).
 *
 * As you will probably see, a particular attention was paid to precisely name classes and objects
 * according to their usage. We hope this will help you enter the library.
 *
 * The OpenTURNS core team.
 */
#ifndef OPENTURNS_OTPRIVATE_HXX
#define OPENTURNS_OTPRIVATE_HXX

/*
 * This include defines the debug level in all source code.
 */
#include "openturns/OTdebug.h"

/*
 * The OTtypes.hxx header file contains all type declarations used by
 * the OpenTURNS platform.
 */
#include "openturns/OTtypes.hxx"

/*
 * The OTmemory.hxx header file declares some debugging memory functions
 */
#include "openturns/OTmemory.hxx"

/*
 * Assertions are needed to ensure invariant respect
 */
#include <cassert>

/*
 * Some templates to ease C memory allocation
 */
#include "openturns/OTconfig.hxx"

#ifdef OPENTURNS_HAVE_STDLIB_H
#include <stdlib.h>
#endif

template <class T>
T * newC(size_t count = 1)
{
  T * ptr = static_cast<T *>( calloc( count, sizeof(T) ) );
  return ptr;
}

template <class T>
void deleteC(const T * ptr)
{
  free( const_cast<T *>( ptr ) );
}

#endif /* OPENTURNS_OTPRIVATE_HXX */
