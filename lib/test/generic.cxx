//                                               -*- C++ -*-
/**
 *  @brief This wrapper defines no function in order to switch to library internal functions
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
*                                  generic wrapper                                       *
*                                                                                        *
******************************************************************************************
*/

#define WRAPPERNAME generic

/*
******************************************************************************************
*                                                                                        *
*                                  generic function                                      *
*                                                                                        *
******************************************************************************************
*/


/* The createState function is optional */
/*FUNC_CREATESTATE( WRAPPERNAME , {} ) */

/* The deleteState function is optional */
/*FUNC_DELETESTATE( WRAPPERNAME , {} ) */




/* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
 * signatures of the wrapper functions. In particular, it hold the size of the input NumericalPoint
 * (inSize_) and of the output NumericalPoint (outSize_).
 * Those information are also used by the gradient and hessian functions to set the correct size
 * of the returned matrix and tensor.
 */
/*FUNC_INFO( WRAPPERNAME , {} )*/


/* Any function declared into the wrapper MUST declare three actual functions prefixed with
 * 'init_', 'exec_' and 'finalize_' followed by the name of the function, here 'compute_deviation'.
 *
 * The 'init_' function is only called once when the NumericalMathFunction object is created.
 * It allows the wrapper to set some internal state, read some external file, prepare the function
 * to run, etc. It takes only one argument, the internal state as created by the
 *
 * The 'exec_' function is intended to execute what the wrapper is done for: compute an mathematical
 * function or anything else. It takes the internal state pointer as its first argument, the input
 * NumericalPoint pointer as the second and the output NumericalPoint pointer as the third.
 *
 * The 'finalize_' function is only called once when the NumericalMathFunction object is destroyed.
 * It allows the wrapper to flush anything before unloading.
 */


/**
 * Initialization function
 * This function is called once just after the wrapper is loaded to initialize
 * it, ie create a temporary subdirectory (remember that the wrapper may be called
 * concurrently), store exchanged data in some internal storage, do some
 * pre-computational operations, etc.
 */
/*FUNC_INIT( WRAPPERNAME , {} )*/




/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns another vector.
 */
/* FUNC_EXEC( WRAPPERNAME , FUNC_EXEC_BODY_CALLING_COMMAND_IN_TEMP_DIR(0) ) */

/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
/*FUNC_FINALIZE( WRAPPERNAME , {} )*/


WRAPPER_END

END_C_DECLS
