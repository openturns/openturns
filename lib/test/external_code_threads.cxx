//                                               -*- C++ -*-
/**
 *  @brief
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
#include "OTconfig.hxx" // Only needed for test wrappers
#include "WrapperInterface.h"
#include "WrapperCommon.h"
#include "WrapperMacros.h"


/*
 *  This is the declaration of function named 'smallest' into the wrapper.
 */


BEGIN_C_DECLS

WRAPPER_BEGIN

/*
******************************************************************************************
*                                                                                        *
*                                  smallest function                                     *
*                                                                                        *
******************************************************************************************
*/

#define WRAPPERNAME external_code

/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns another vector.
 */
#define NUM_LOOPS 1000000
FUNC_EXEC( WRAPPERNAME ,
{
  const long inSize  = getNumberOfVariables(p_exchangedData, WRAPPER_IN);
  const long outSize = getNumberOfVariables(p_exchangedData, WRAPPER_OUT);

  // First initialize the output point to the null vector
  for (long i = 0; i < outSize; i++) outPoint->data_[i] = 0.0;

  // Search the fixed point of the contraction by a fixed number of iterations
  for (long i = 0; i < NUM_LOOPS; i++)
    outPoint->data_[i % outSize] = inPoint->data_[i % inSize] + outPoint->data_[(i + 1) % outSize];
} )


WRAPPER_END

END_C_DECLS
