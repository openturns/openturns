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
#include <cstring>
#include "OTconfig.hxx" // Only needed for test wrappers
#include "WrapperCommon.h"
#include "WrapperMacros.h"

/*
 * Here is some functions absolutely internal to the wrapper. They are not exported or seen
 * by the platform, so you can do anything you want.
 */

/*
 * In this example we decided to:
 *  - count the number of times we call the function, the gradient or the hessian
 *  - save the data sent by the OpenTURNS platform to the internal state (though there is no need for that)
 */

namespace WrapperInternals
{

struct internalState
{
  long numberOfCalls;
  char * currentWorkingDirectory;
  struct WrapperInformation info;
  struct WrapperExchangedData * p_exchangedData;
};

#ifdef FIND_EXCHANGED_DATA_IN_INTERNAL_STATE_DEF
#undef FIND_EXCHANGED_DATA_IN_INTERNAL_STATE_DEF
#define FIND_EXCHANGED_DATA_IN_INTERNAL_STATE_DEF( pointer ) CAST(struct WrapperExchangedData *, CAST(WrapperInternals::internalState *, pointer)->p_exchangedData)
#endif

static inline
void internalStateIncrement(struct internalState * p_internalState)
{
  if (p_internalState) p_internalState->numberOfCalls++;
}

// static inline
// long internalStateGetNumberOfCalls(struct internalState * p_internalState)
// {
//   return (p_internalState ? p_internalState->numberOfCalls : -1);
// }

// static inline
// void internalStatePrint(const char * functionName, struct internalState * p_internalState)
// {
// }

static inline
void commonInternalStateCreation(void ** p_p_state, const struct WrapperExchangedData * p_exchangedData, void * p_error)
{
  *p_p_state = new WrapperInternals::internalState;

  struct WrapperInternals::internalState * p_internalState = static_cast<struct WrapperInternals::internalState *>(*p_p_state);

  copyWrapperExchangedData(& (p_internalState->p_exchangedData), p_exchangedData, p_error);
  p_internalState->numberOfCalls           = 0;
  p_internalState->info.inSize_            = getNumberOfVariables(p_internalState->p_exchangedData, WRAPPER_IN);
  p_internalState->info.outSize_           = getNumberOfVariables(p_internalState->p_exchangedData, WRAPPER_OUT);
  p_internalState->currentWorkingDirectory = getCurrentWorkingDirectory(p_error);
}

static inline
void commonInternalStateDestruction(void * p_state, void * p_error)
{
  struct WrapperInternals::internalState * p_internalState = static_cast<struct WrapperInternals::internalState *>(p_state);
  free(p_internalState->currentWorkingDirectory);
  freeWrapperExchangedData(p_internalState->p_exchangedData);
  delete p_internalState;
}

static inline
void commonGetInfo(void * p_state, struct WrapperInformation * p_info, void * p_error)
{
  *p_info = static_cast<struct WrapperInternals::internalState *>(p_state)->info;
}

} /* namespace WrapperInternals */



/*
 *  This is the declaration of function named 'compute_deviation' into the wrapper.
 */


BEGIN_C_DECLS

WRAPPER_BEGIN
/*
******************************************************************************************
*                                                                                        *
*                             compute_deviation function                                 *
*                                                                                        *
******************************************************************************************
*/

/* The name of the wrapper's functions is defined in WRAPPERNAME macro */
#define WRAPPERNAME compute_deviation


/* The createState function is optional */
FUNC_CREATESTATE( WRAPPERNAME ,
{
  WrapperInternals::commonInternalStateCreation(p_p_state, p_exchangedData, p_error);
} )

/* The deleteState function is optional */
FUNC_DELETESTATE( WRAPPERNAME ,
{
  WrapperInternals::commonInternalStateDestruction(p_state, p_error);
} )





/* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
 * signatures of the wrapper functions. In particular, it hold the size of the input NumericalPoint
 * (inSize_) and of the output NumericalPoint (outSize_).
 * Those information are also used by the gradient and hessian functions to set the correct size
 * of the returned matrix and tensor.
 */
FUNC_INFO( WRAPPERNAME ,
{
  WrapperInternals::commonGetInfo(p_state, p_info, p_error);
} )


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
FUNC_INIT( WRAPPERNAME , {} )


// These commands should be inserted in FUNC_EXEC argument
// just below.  But Microsoft compiler (at least Visual Studio 2010)
// chokes on '#ifdef WIN32' because of the hash sign.
#ifdef WIN32
#define TEST_SET_PATH_ON_WINDOWS \
      char * cmd = p_exchangedData->parameters_->command_; \
      char * autotest_path = getenv("AUTOTEST_PATH"); \
      if (autotest_path != NULL) \
      { \
        char * abs_cmd = (char *) calloc(strlen(autotest_path) + strlen(cmd) + 60, sizeof(char)); \
        sprintf(abs_cmd, "set PATH=%s;%%PATH%% & %s > NUL", autotest_path, cmd); \
        free(cmd); \
        p_exchangedData->parameters_->command_ = abs_cmd; \
      }
#else
#define TEST_SET_PATH_ON_WINDOWS
#endif


/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns another vector.
 */
FUNC_EXEC( WRAPPERNAME,
{
  struct WrapperInternals::internalState * p_internalState = static_cast<struct WrapperInternals::internalState *>(p_state);

  internalStateIncrement(p_internalState);
  //                internalStatePrint("func_exec_compute_deviation", p_internalState);

  if ( (inPoint->size_  != p_internalState->info.inSize_) ||
  (outPoint->size_ != p_internalState->info.outSize_) ) return WRAPPER_EXECUTION_ERROR;

  double & E = inPoint->data_[0];
  double & F = inPoint->data_[1];
  double & L = inPoint->data_[2];
  double & I = inPoint->data_[3];

  double & d = outPoint->data_[0];

  if ((E == 0.0) || (I == 0.0))
  {
    setError( p_error, "Neither E nor I should be zero. Got E=%g and I=%g", E, I );
    return WRAPPER_EXECUTION_ERROR;
  }

  switch (p_internalState->p_exchangedData->parameters_->mode_)
  {

    case WRAPPER_STATICLINK:
      /* The real computation is here */
      d = -( F * L * L * L ) / ( 3. * E * I );
      break;

    case WRAPPER_DYNAMICLINK:
      setError( p_error, "Wrapper doesn't support dynamic linkage" );
      return WRAPPER_USAGE_ERROR;
      break;

    case WRAPPER_FORK:
    {
      /* We build a temporary directory in which we will work */
      char * temporaryDirectory = createTemporaryDirectory("openturnsWorkingDirectory", p_internalState->p_exchangedData, p_error);

      /* We create the input files for the external code in the temporary directory */
      if (createInputFiles(temporaryDirectory, p_internalState->p_exchangedData, inPoint, p_error)) return WRAPPER_EXECUTION_ERROR;

      // Autotest only set the PATH for Linux, so we have to manually set the windows PATH.
      TEST_SET_PATH_ON_WINDOWS

      /* The real computation is here */
      long rc = runInsulatedCommand( temporaryDirectory, p_exchangedData, inPoint, p_error );

      /* Read the output values */
      if ( rc == 0 )
        if (readOutputFiles( temporaryDirectory, p_exchangedData, outPoint, p_error ))
        {
          const char * msg = getError( p_error );
          setError( p_error, "Can't read output files. Reason: %s", msg );
          free( (void *) msg );
          return WRAPPER_EXECUTION_ERROR;
        }

      /* We kill the temporary directory */
      deleteTemporaryDirectory(temporaryDirectory, rc, p_error);
    }
    break;

    default:
      setError( p_error, "Invalid mode for wrapper" );
      return WRAPPER_USAGE_ERROR;
  }

} )



/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
FUNC_FINALIZE( WRAPPERNAME , {} )





/*
******************************************************************************************
*                                                                                        *
*                             compute_deviation gradient                                 *
*                                                                                        *
******************************************************************************************
*/


/* The createState function is optional */
GRAD_CREATESTATE( WRAPPERNAME ,
{
  WrapperInternals::commonInternalStateCreation(p_p_state, p_exchangedData, p_error);
} )

/* The deleteState function is optional */
GRAD_DELETESTATE( WRAPPERNAME ,
{
  WrapperInternals::commonInternalStateDestruction(p_state, p_error);
} )





/* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
 * signatures of the wrapper functions. In particular, it hold the size of the input NumericalPoint
 * (inSize_) and of the output NumericalPoint (outSize_).
 * Those information are also used by the gradient and hessian functions to set the correct size
 * of the returned matrix and tensor.
 */
GRAD_INFO( WRAPPERNAME ,
{
  WrapperInternals::commonGetInfo(p_state, p_info, p_error);
} )


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
 * it, ie create a temparary subdirectory (remember that the wrapper may be called
 * concurrently), store exchanged data in some internal storage, do some
 * pre-computational operations, etc.
 */
GRAD_INIT( WRAPPERNAME , {} )




/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns a gradient matrix.
 */
GRAD_EXEC( WRAPPERNAME,
{
  struct WrapperInternals::internalState * p_internalState = static_cast<struct WrapperInternals::internalState *>(p_state);

  internalStateIncrement(p_internalState);

  if ( (inPoint->size_     != p_internalState->info.inSize_) ||
  (outMatrix->nbrows_ != p_internalState->info.inSize_) ||
  (outMatrix->nbcols_ != p_internalState->info.outSize_) )  return WRAPPER_EXECUTION_ERROR;

  /* The real computation is here */
  double & E     = inPoint->data_[0];
  double & F     = inPoint->data_[1];
  double & L     = inPoint->data_[2];
  double & I     = inPoint->data_[3];

  double & df_dE = outMatrix->data_[0];
  double & df_dF = outMatrix->data_[1];
  double & df_dL = outMatrix->data_[2];
  double & df_dI = outMatrix->data_[3];

  if ((E == 0.0) || (I == 0.0))
  {
    setError( p_error, "Neither E nor I should be zero. Got E=%g and I=%g", E, I );
    return WRAPPER_EXECUTION_ERROR;
  }

  df_dE =  ( F * L*L * L ) / ( 3 * E*E * I   );
  df_dF = -(     L*L * L ) / ( 3 * E   * I   );
  df_dL = -( F * L * L   ) / (     E   * I   );
  df_dI =  ( F * L*L * L ) / ( 3 * E   * I * I );
} )




/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
GRAD_FINALIZE( WRAPPERNAME , {} )






/*
******************************************************************************************
*                                                                                        *
*                             compute_deviation hessian                                  *
*                                                                                        *
******************************************************************************************
*/


/* The createState function is optional */
HESS_CREATESTATE( WRAPPERNAME ,
{
  WrapperInternals::commonInternalStateCreation(p_p_state, p_exchangedData, p_error);
} )

/* The deleteState function is optional */
HESS_DELETESTATE( WRAPPERNAME ,
{
  WrapperInternals::commonInternalStateDestruction(p_state, p_error);
} )





/* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
 * signatures of the wrapper functions. In particular, it hold the size of the input NumericalPoint
 * (inSize_) and of the output NumericalPoint (outSize_).
 * Those information are also used by the hessient and hessian functions to set the correct size
 * of the returned matrix and tensor.
 */
HESS_INFO( WRAPPERNAME ,
{
  WrapperInternals::commonGetInfo(p_state, p_info, p_error);
} )


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
 * it, ie create a temparary subdirectory (remember that the wrapper may be called
 * concurrently), store exchanged data in some internal storage, do some
 * pre-computational operations, etc.
 */
HESS_INIT( WRAPPERNAME , {} )




/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns a hessient matrix.
 */
HESS_EXEC( WRAPPERNAME,
{
  struct WrapperInternals::internalState * p_internalState = static_cast<struct WrapperInternals::internalState *>(p_state);

  internalStateIncrement(p_internalState);


  if ( (inPoint->size_       != p_internalState->info.inSize_) ||
  (outTensor->nbrows_   != p_internalState->info.inSize_) ||
  (outTensor->nbcols_   != p_internalState->info.inSize_) ||
  (outTensor->nbsheets_ != p_internalState->info.outSize_) ) return WRAPPER_EXECUTION_ERROR;


  /* The real computation is here */
  double & E = inPoint->data_[0];
  double & F = inPoint->data_[1];
  double & L = inPoint->data_[2];
  double & I = inPoint->data_[3];

  if ((E == 0.0) || (I == 0.0))
  {
    setError( p_error, "Neither E nor I should be zero. Got E=%g and I=%g", E, I );
    return WRAPPER_EXECUTION_ERROR;
  }

  double d2f_dE2  = -( 2 * F * L*L * L ) / ( 3 * E*E*E * I     );
  double d2f_dEdF =  (         L*L * L ) / ( 3 * E*E   * I     );
  double d2f_dEdL =  (     F * L * L   ) / (     E*E   * I     );
  double d2f_dEdI = -(     F * L*L * L ) / ( 3 * E*E   * I * I   );

  double d2f_dF2  =  0;
  double d2f_dFdL = -(         L * L   ) / (     E     * I     );
  double d2f_dFdI =  (         L*L * L ) / ( 3 * E     * I * I   );

  double d2f_dL2  = -( 2 * F * L     ) / (     E     * I     );
  double d2f_dLdI =  (     F * L * L   ) / (     E     * I * I   );

  double d2f_dI2  = -( 2 * F * L*L * L ) / ( 3 * E     * I*I * I );

  outTensor->data_[ 0] = d2f_dE2;
  outTensor->data_[ 1] = d2f_dEdF;
  outTensor->data_[ 2] = d2f_dEdL;
  outTensor->data_[ 3] = d2f_dEdI;

  outTensor->data_[ 4] = d2f_dEdF;
  outTensor->data_[ 5] = d2f_dF2;
  outTensor->data_[ 6] = d2f_dFdL;
  outTensor->data_[ 7] = d2f_dFdI;

  outTensor->data_[ 8] = d2f_dEdL;
  outTensor->data_[ 9] = d2f_dFdL;
  outTensor->data_[10] = d2f_dL2;
  outTensor->data_[11] = d2f_dLdI;

  outTensor->data_[12] = d2f_dEdI;
  outTensor->data_[13] = d2f_dFdI;
  outTensor->data_[14] = d2f_dLdI;
  outTensor->data_[15] = d2f_dI2;
} )




/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
HESS_FINALIZE( WRAPPERNAME , {} )



WRAPPER_END

END_C_DECLS
