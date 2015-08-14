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
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <pthread.h>
#include "WrapperInterface.h"
#include "WrapperMacros.h"

/* TODO : Add error treatment when something goes wrong into one wrapper function. We CAN'T use exceptions */

/*
 * Here is some functions absolutely internal to the wrapper. They are not exported or seen
 * by the platform, so you can do anything you want.
 */

namespace WrapperInternals
{

static pthread_mutex_t Lock = PTHREAD_MUTEX_INITIALIZER;

struct internalState
{
  long numberOfCalls;
};

void internalStateInitialization(struct internalState * p_internalState)
{
  pthread_mutex_lock( &Lock );
  if (p_internalState) p_internalState->numberOfCalls = 0;
  pthread_mutex_unlock( &Lock );
}

void internalStateIncrement(struct internalState * p_internalState)
{
  pthread_mutex_lock( &Lock );
  if (p_internalState) p_internalState->numberOfCalls++;
  pthread_mutex_unlock( &Lock );
}

long internalStateGetNumberOfCalls(struct internalState * p_internalState)
{
  long calls;
  pthread_mutex_lock( &Lock );
  calls = (p_internalState ? p_internalState->numberOfCalls : -1);
  pthread_mutex_unlock( &Lock );
  return calls;
}


} /* namespace WrapperInternals */



/*
 *  This is the declaration of function named 'fonction1' into the wrapper.
 *  All functions MUST be prefixed by 'func_' because it is a function (not
 *  a gradient or an hessian).
 */


BEGIN_C_DECLS

WRAPPER_BEGIN

/* The state is a memory space allocated to a NumericalMathFunction object when it is constructed
 * so that it can save and remember its internal state through the calls to the wrapper. It is
 * useful because the wrapper is shared among all NumericalMathFunction object through the Library
 * loading mechanism, and because the functions declared into the wrapper can be called
 * concurently (multithreading). The state is destroyed when the Numerical MathFunctionObject is
 * destroyed.
 * The state is not used by the NumericalMathFunction object. This one only takes a pointer to the
 * memory space allocated by the wrapper, and ensures that it is given as the first argument
 * of each wrapper function, so that each function can save and reload ths state.
 * Because the NumericalMathFunction object only sees the state as an obscure pointer (void *),
 * the wrapper is responsible for allocating and deallocating the memory.
 *
 * WARNING : the state pointer CAN be NULL so that each function must check its value before
 *           dereferening it !
 *
 * The state creation is done by a function prefixed by 'createState_' and followed by the name
 * of the function, here 'fonction1'. This function takes no argument and returns a obscure
 * pointer to the internal state memory space, or any other thing that can only be interpreted
 * inside the wrapper functions. This pointer MAY be NULL.
 *
 * The state deletion is done by a function prefixed by 'deleteState_' and followed by the name
 * of the function, here 'fonction1'. This function takes the state pointer as the only argument.
 */


/* State creation function */
enum WrapperErrorCode func_createState_fonction1(void ** p_p_state, const struct WrapperExchangedData * p_exchangedData)
{
  *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));
  internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));
  return WRAPPER_OK;
}

/* State deletion function */
enum WrapperErrorCode func_deleteState_fonction1(void * p_state)
{
  free(p_state);
  return WRAPPER_OK;
}





/* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
 * signatures of the wrapper functions. In particular, it hold the size of the input NumericalPoint
 * (inSize_) and of the output NumericalPoint (outSize_).
 * Those information are also used by the gradient and hessian functions to set the correct size
 * of the returned matrix and tensor.
 */


/* Wrapper information */
static struct WrapperInformation info_fonction1 = {/* inSize_  = */ 2,
         /* outSize_ = */ 3
};

enum WrapperErrorCode func_getInfo_fonction1(void * p_state, struct WrapperInformation * p_info)
{
  *p_info = info_fonction1;
  return WRAPPER_OK;
}





/* Any function declared into the wrapper MUST declare three actual functions prefixed with
 * 'init_', 'exec_' and 'finalize_' folowed by the name of the function, here 'fonction1'.
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
 * it, ie create a temparary subdirectory (remember that the wrapper may be called
 * concurrently), store exchanged data in some internal repository, do some
 * pre-computational operation, etc.
 */
enum WrapperErrorCode func_init_fonction1(void * p_state)
{
  return WRAPPER_OK;
}




/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns another vector.
 */
enum WrapperErrorCode func_exec_fonction1(void * p_state, const struct point * inPoint, struct point * outPoint)
{
  internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));

  /* The real computation is here */
  double x = 0.;
  double y = 0.;

  if (inPoint->size_ == 2)
  {
    x = inPoint->data_[0];
    y = inPoint->data_[1];
  }

  double modulus = sqrt(x * x + y * y);
  double cosinus = (modulus != 0.) ? x / modulus : 0.;
  double sinus   = (modulus != 0.) ? y / modulus : 0.;

  if (outPoint->size_ == 3)
  {
    outPoint->data_[0] = modulus;
    outPoint->data_[1] = cosinus;
    outPoint->data_[2] = sinus;
  }

  return WRAPPER_OK;
}




/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
enum WrapperErrorCode func_finalize_fonction1(void * p_state)
{
  return WRAPPER_OK;
}

WRAPPER_END

END_C_DECLS
