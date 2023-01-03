//                                               -*- C++ -*-
/**
 *  @brief The header file that declares de-/allocation operators
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OTMEMORY_HXX
#define OPENTURNS_OTMEMORY_HXX

#ifdef DEBUG_MEMORY
#ifndef SWIG

#include <new>

// These static functions are overloaded for debugging purpose
extern void * operator new (size_t sz);
extern void operator delete (void * ptr);
extern void * operator new [] (size_t sz);
extern void operator delete [] (void * ptr);

#endif /* SWIG */
#endif /* DEBUG_MEMORY */

#endif /* OPENTURNS_OTMEMORY_HXX */
