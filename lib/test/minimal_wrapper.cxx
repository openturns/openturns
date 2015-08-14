//                                               -*- C++ -*-
/**
 *  @brief This is an example of the smallest definition of a wrapper
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

#define WRAPPERNAME smallest

/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns another vector.
 */
FUNC_EXEC( WRAPPERNAME ,
{
  /* The real computation is here */
  double & x1 = inPoint->data_[0];
  double & x2 = inPoint->data_[1];

  double & y  = outPoint->data_[0];

  y = x1 * x2 + x1 + x2;
} )

WRAPPER_END

END_C_DECLS
