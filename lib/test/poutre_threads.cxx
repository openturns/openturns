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
#include "OT.hxx"
#include "WrapperInterface.h"

/* TODO : Add error treatment when something goes wrong into one wrapper function. We CAN'T use exceptions */

/*
 * Here is some functions absolutely internal to the wrapper. They are not exported or seen
 * by the platform, so you can do anything you want.
 */

namespace WrapperInternals
{

struct internalState
{
  long numberOfCalls;
};

void internalStateInitialization(struct internalState * p_internalState)
{
  if (p_internalState) p_internalState->numberOfCalls = 0;
}

pthread_mutex_t mutexState;

void internalStateIncrement(struct internalState * p_internalState)
{
  pthread_mutex_lock (&mutexState);
  if (p_internalState) p_internalState->numberOfCalls++;
  pthread_mutex_unlock (&mutexState);
}

long internalStateGetNumberOfCalls(struct internalState * p_internalState)
{
  return (p_internalState ? p_internalState->numberOfCalls : -1);
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



/* State creation function */
enum WrapperErrorCode func_createState_compute_deviation(void ** p_p_state, const struct WrapperExchangedData * p_exchangedData)
{
  *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));
  internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));

#ifdef DEBUG
  std::cerr << "<<< in void func_createState_compute_deviation(void ** p_p_state=" << p_p_state << ") [*p_p_state=" << *p_p_state << "] >>>" << std::endl;
#endif
  return WRAPPER_OK;
}

/* State deletion function */
enum WrapperErrorCode func_deleteState_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void func_deleteState_compute_deviation(void * p_state=" << p_state << ") >>>" << std::endl;
#endif

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
static struct WrapperInformation info_compute_deviation = {/* inSize_  = */ 4,
         /* outSize_ = */ 1
};

enum WrapperErrorCode func_getInfo_compute_deviation(void * p_state, struct WrapperInformation * p_info)
{
  *p_info = info_compute_deviation;
  return WRAPPER_OK;
}


/* Any function declared into the wrapper MUST declare three actual function prefixed with
 * 'init_', 'exec_' and 'finalize_' folowed by the name of the function, here 'compute_deviation'.
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
enum WrapperErrorCode func_init_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void func_init_compute_deviation(const struct WrapperExchangedData * p_exchangedData="
            << p_exchangedData
            << ") >>>" << std::endl;

  std::cerr << "<<< (func_init_compute_deviation) Exchanged data :" << std::endl;

  // Print the file list
  const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
  while (currentFileElement)
  {
    const struct WrapperFileList * nextElement = currentFileElement->next_;
    std::cerr << "    (func_init_compute_deviation) File (name='" << currentFileElement->file_->name_
              << "', path='" << currentFileElement->file_->path_
              << "', type='" << currentFileElement->file_->type_
              << "')" << std::endl;
    currentFileElement = nextElement;
  }

  // Print the variable list
  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    const struct WrapperVariableList * nextElement = currentVariableElement->next_;
    std::cerr << "    (func_init_compute_deviation) Variable (comment='" << currentVariableElement->variable_->comment_
              << "', unit='" << currentVariableElement->variable_->unit_
              << "', regexp='" << currentVariableElement->variable_->regexp_
              << "', format='" << currentVariableElement->variable_->format_
              << "', type='" << currentVariableElement->variable_->type_
              << "')" << std::endl;
    currentVariableElement = nextElement;
  }

  std::cerr << "    (func_init_compute_deviation) >>>" << std::endl;
#endif
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
enum WrapperErrorCode func_exec_compute_deviation(void * p_state, const struct point * inPoint, struct point * outPoint)
{
  internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));

#ifdef DEBUG
  {
    std::cerr << "<<< in void func_exec_compute_deviation(void * p_state=" << p_state << ", struct point {size=" << inPoint->size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < inPoint->size_; i++, separator = ",") std::cerr << separator << inPoint->data_[i];
    std::cerr << "]}, ";
  }
#endif


  /* The real computation is here */
  double E, F, L, I;

  if (inPoint->size_ == info_compute_deviation.inSize_)
  {
    E = inPoint->data_[0];
    F = inPoint->data_[1];
    L = inPoint->data_[2];
    I = inPoint->data_[3];
  }
  else return WRAPPER_WRONG_ARGUMENT;

  if ((E == 0.0) || (I == 0.0)) return WRAPPER_EXECUTION_ERROR;

  double d = -( F * L * L * L ) / ( 3 * E * I );
  for (int i = 0; i < 1000000; i++)
    d += sin(cos(i));

  if (outPoint->size_ == info_compute_deviation.outSize_)
  {
    outPoint->data_[0] = d;
  }
  else return WRAPPER_WRONG_ARGUMENT;




#ifdef DEBUG
  {
    std::cerr << "struct point {size=" << outPoint->size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < outPoint->size_; i++, separator = ",") std::cerr << separator << outPoint->data_[i];
    std::cerr << "]} ) NumberOfCalls = "
              << WrapperInternals::internalStateGetNumberOfCalls(static_cast<struct WrapperInternals::internalState *>(p_state))
              << " >>>" << std::endl;
  }
#endif

  return WRAPPER_OK;
}

struct job
{
  void * p_state;
  const struct sample * inSample;
  struct sample * outSample;
  int begin;
  int end;
};

void* threadExecute(void* arg)
{
  struct job * localJob = (struct job*) arg;
  int begin(localJob->begin);
  int end(localJob->end);
  for (int i = begin; i < end; i++)
  {
    func_exec_compute_deviation(localJob->p_state, &(localJob->inSample->data_[i]), &(localJob->outSample->data_[i]));
  }
  return NULL;
}

/**
 * Execution function over a sample
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns another vector.
 */
enum WrapperErrorCode func_exec_sample_compute_deviation(void * p_state, const struct sample * inSample, struct sample * outSample)
{
#define NUM_THREADS 4
  pthread_t thread[NUM_THREADS];
  struct job jobs[NUM_THREADS];
  pthread_attr_t attr;
  int rc, status;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  int size(inSample->size_);
  for(int t = 0; t < NUM_THREADS; t++)
  {
    jobs[t].p_state = p_state;
    jobs[t].inSample = inSample;
    jobs[t].outSample = outSample ;
    jobs[t].begin = (t * size) / NUM_THREADS;
    jobs[t].end = ((t + 1) * size) / NUM_THREADS;
    rc = pthread_create(&thread[t], &attr, threadExecute, (void *) &jobs[t]);
    if (rc)
    {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }

  /* Free attribute and wait for the other threads */
  pthread_attr_destroy(&attr);
  for(int t = 0; t < NUM_THREADS; t++)
  {
    rc = pthread_join(thread[t], (void **)&status);
    if (rc)
    {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Completed join with thread %d status= %d\n", t, status);
  }

  return WRAPPER_OK;
}




/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
enum WrapperErrorCode func_finalize_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void func_finalize_compute_deviation() >>>" << std::endl;
#endif
  return WRAPPER_OK;
}





/*
******************************************************************************************
*                                                                                        *
*                             compute_deviation gradient                                 *
*                                                                                        *
******************************************************************************************
*/


/* State creation function */
enum WrapperErrorCode grad_createState_compute_deviation(void ** p_p_state, const struct WrapperExchangedData * p_exchangedData)
{
  *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));
  internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));

#ifdef DEBUG
  std::cerr << "<<< in void grad_createState_compute_deviation(void ** p_p_state=" << p_p_state << ") [*p_p_state=" << *p_p_state << "] >>>" << std::endl;
#endif
  return WRAPPER_OK;
}

/* State deletion function */
enum WrapperErrorCode grad_deleteState_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void grad_deleteState_compute_deviation(void * p_state=" << p_state << ") >>>" << std::endl;
#endif

  free(p_state);
  return WRAPPER_OK;
}





/* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
 * signatures of the wrapper functions. In particular, it hold the size of the input NumericalPoint
 * (inSize_) and of the output NumericalPoint (outSize_).
 * Those information are also used by the gradient and hessian functions to set the correct size
 * of the returned matrix and tensor.
 */



enum WrapperErrorCode grad_getInfo_compute_deviation(void * p_state, struct WrapperInformation * p_info)
{
  *p_info = info_compute_deviation;
  return WRAPPER_OK;
}


/* Any function declared into the wrapper MUST declare three actual function prefixed with
 * 'init_', 'exec_' and 'finalize_' folowed by the name of the function, here 'compute_deviation'.
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
enum WrapperErrorCode grad_init_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void grad_init_compute_deviation(const struct WrapperExchangedData * p_exchangedData="
            << p_exchangedData
            << ") >>>" << std::endl;

  std::cerr << "<<< (grad_init_compute_deviation) Exchanged data :" << std::endl;

  // Print the file list
  const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
  while (currentFileElement)
  {
    const struct WrapperFileList * nextElement = currentFileElement->next_;
    std::cerr << "    (grad_init_compute_deviation) File (name='" << currentFileElement->file_->name_
              << "', path='" << currentFileElement->file_->path_
              << "', type='" << currentFileElement->file_->type_
              << "')" << std::endl;
    currentFileElement = nextElement;
  }

  // Print the variable list
  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    const struct WrapperVariableList * nextElement = currentVariableElement->next_;
    std::cerr << "    (grad_init_compute_deviation) Variable (comment='" << currentVariableElement->variable_->comment_
              << "', unit='" << currentVariableElement->variable_->unit_
              << "', regexp='" << currentVariableElement->variable_->regexp_
              << "', format='" << currentVariableElement->variable_->format_
              << "', type='" << currentVariableElement->variable_->type_
              << "')" << std::endl;
    currentVariableElement = nextElement;
  }

  std::cerr << "    (grad_init_compute_deviation) >>>" << std::endl;
#endif
  return WRAPPER_OK;
}




/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns a gradient matrix.
 */
enum WrapperErrorCode grad_exec_compute_deviation(void * p_state, const struct point * inPoint, struct matrix * outMatrix)
{
  internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));

#ifdef DEBUG
  {
    std::cerr << "<<< in void grad_exec_compute_deviation(void * p_state=" << p_state << ", struct point {size=" << inPoint->size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < inPoint->size_; i++, separator = ",") std::cerr << separator << inPoint->data_[i];
    std::cerr << "]}, ";
  }
#endif


  /* The real computation is here */
  double E, F, L, I;

  if (inPoint->size_ == info_compute_deviation.inSize_)
  {
    E = inPoint->data_[0];
    F = inPoint->data_[1];
    L = inPoint->data_[2];
    I = inPoint->data_[3];
  }
  else return WRAPPER_WRONG_ARGUMENT;

  double df_dE =  ( F * L * L * L ) / ( 3 * E * E * I   );
  double df_dF = -(     L * L * L ) / ( 3 * E   * I   );
  double df_dL = -( F * L * L   ) / (     E   * I   );
  double df_dI =  ( F * L * L * L ) / ( 3 * E   * I * I );

  if ( (outMatrix->nbrows_ == info_compute_deviation.inSize_) && (outMatrix->nbcols_ == info_compute_deviation.outSize_) )
  {
    outMatrix->data_[0] = df_dE;
    outMatrix->data_[1] = df_dF;
    outMatrix->data_[2] = df_dL;
    outMatrix->data_[3] = df_dI;
  }
  else return WRAPPER_WRONG_ARGUMENT;



#ifdef DEBUG
  {
    std::cerr << "struct matrix {rows=" << outMatrix->nbrows_ << ",cols=" << outMatrix->nbcols_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < outMatrix->nbrows_; i++)
      for(unsigned int j = 0; j < outMatrix->nbcols_; j++, separator = ",")
        std::cerr << separator << outMatrix->data_[i + j * outMatrix->nbrows_];
    std::cerr << "]} ) NumberOfCalls = "
              << WrapperInternals::internalStateGetNumberOfCalls(static_cast<struct WrapperInternals::internalState *>(p_state))
              << " >>>" << std::endl;
  }
#endif

  return WRAPPER_OK;
}




/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
enum WrapperErrorCode grad_finalize_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void grad_finalize_compute_deviation() >>>" << std::endl;
#endif
  return WRAPPER_OK;
}







/*
******************************************************************************************
*                                                                                        *
*                             compute_deviation hessian                                  *
*                                                                                        *
******************************************************************************************
*/


/* State creation function */
enum WrapperErrorCode hess_createState_compute_deviation(void ** p_p_state, const struct WrapperExchangedData * p_exchangedData)
{
  *p_p_state = malloc(sizeof(struct WrapperInternals::internalState));
  internalStateInitialization(static_cast<struct WrapperInternals::internalState *>(*p_p_state));

#ifdef DEBUG
  std::cerr << "<<< in void hess_createState_compute_deviation(void ** p_p_state=" << p_p_state << ") [*p_p_state=" << *p_p_state << "] >>>" << std::endl;
#endif
  return WRAPPER_OK;
}

/* State deletion function */
enum WrapperErrorCode hess_deleteState_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void hess_deleteState_compute_deviation(void * p_state=" << p_state << ") >>>" << std::endl;
#endif

  free(p_state);
  return WRAPPER_OK;
}





/* The wrapper information informs the NumericalMathFunction object that loads the wrapper of the
 * signatures of the wrapper functions. In particular, it hold the size of the input NumericalPoint
 * (inSize_) and of the output NumericalPoint (outSize_).
 * Those information are also used by the hessient and hessian functions to set the correct size
 * of the returned matrix and tensor.
 */



enum WrapperErrorCode hess_getInfo_compute_deviation(void * p_state, struct WrapperInformation * p_info)
{
  *p_info = info_compute_deviation;
  return WRAPPER_OK;
}


/* Any function declared into the wrapper MUST declare three actual function prefixed with
 * 'init_', 'exec_' and 'finalize_' folowed by the name of the function, here 'compute_deviation'.
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
enum WrapperErrorCode hess_init_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void hess_init_compute_deviation(const struct WrapperExchangedData * p_exchangedData="
            << p_exchangedData
            << ") >>>" << std::endl;

  std::cerr << "<<< (hess_init_compute_deviation) Exchanged data :" << std::endl;

  // Print the file list
  const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
  while (currentFileElement)
  {
    const struct WrapperFileList * nextElement = currentFileElement->next_;
    std::cerr << "    (hess_init_compute_deviation) File (name='" << currentFileElement->file_->name_
              << "', path='" << currentFileElement->file_->path_
              << "', type='" << currentFileElement->file_->type_
              << "')" << std::endl;
    currentFileElement = nextElement;
  }

  // Print the variable list
  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    const struct WrapperVariableList * nextElement = currentVariableElement->next_;
    std::cerr << "    (hess_init_compute_deviation) Variable (comment='" << currentVariableElement->variable_->comment_
              << "', unit='" << currentVariableElement->variable_->unit_
              << "', regexp='" << currentVariableElement->variable_->regexp_
              << "', format='" << currentVariableElement->variable_->format_
              << "', type='" << currentVariableElement->variable_->type_
              << "')" << std::endl;
    currentVariableElement = nextElement;
  }

  std::cerr << "    (hess_init_compute_deviation) >>>" << std::endl;
#endif
  return WRAPPER_OK;
}




/**
 * Execution function
 * This function is called by the platform to do the real work of the wrapper. It may be
 * called concurrently, so be aware of not using shared or global data not protected by
 * a critical section.
 * This function has a mathematical meaning. It operates on one vector (aka point) and
 * returns a hessient matrix.
 */
enum WrapperErrorCode hess_exec_compute_deviation(void * p_state, const struct point * inPoint, struct tensor * outTensor)
{
  internalStateIncrement(static_cast<struct WrapperInternals::internalState *>(p_state));

#ifdef DEBUG
  {
    std::cerr << "<<< in void hess_exec_compute_deviation(void * p_state=" << p_state << ", struct point {size=" << inPoint->size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < inPoint->size_; i++, separator = ",") std::cerr << separator << inPoint->data_[i];
    std::cerr << "]}, ";
  }
#endif


  /* The real computation is here */
  double E, F, L, I;

  if (inPoint->size_ == info_compute_deviation.inSize_)
  {
    E = inPoint->data_[0];
    F = inPoint->data_[1];
    L = inPoint->data_[2];
    I = inPoint->data_[3];
  }
  else return WRAPPER_WRONG_ARGUMENT;

  double d2f_dE2  = -( 2 * F * L * L * L ) / ( 3 * E * E * E * I     );
  double d2f_dEdF =  (         L * L * L ) / ( 3 * E * E   * I     );
  double d2f_dEdL =  (     F * L * L   ) / (     E * E   * I     );
  double d2f_dEdI = -(     F * L * L * L ) / ( 3 * E * E   * I * I   );

  double d2f_dF2  =  0;
  double d2f_dFdL = -(         L * L   ) / (     E     * I     );
  double d2f_dFdI =  (         L * L * L ) / ( 3 * E     * I * I   );

  double d2f_dL2  = -( 2 * F * L     ) / (     E     * I     );
  double d2f_dLdI =  (     F * L * L   ) / (     E     * I * I   );

  double d2f_dI2  = -( 2 * F * L * L * L ) / ( 3 * E     * I * I * I );

  if ( (outTensor->nbrows_ == info_compute_deviation.inSize_) && (outTensor->nbcols_ == info_compute_deviation.inSize_)  && (outTensor->nbsheets_ == info_compute_deviation.outSize_) )
  {
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
  }
  else return WRAPPER_WRONG_ARGUMENT;



#ifdef DEBUG
  {
    std::cerr << "struct tensor {rows=" << outTensor->nbrows_
              << ",cols=" << outTensor->nbcols_
              << ",sheets=" << outTensor->nbsheets_
              << ", data=[";
    const char * separator = "";
    for(unsigned int k = 0; k < outTensor->nbsheets_; k++)
      for(unsigned int i = 0; i < outTensor->nbrows_; i++)
        for(unsigned int j = 0; j < outTensor->nbcols_; j++, separator = ",")
          std::cerr << separator << outTensor->data_[i + j * outTensor->nbrows_ + k * outTensor->nbrows_ * outTensor->nbcols_];
    std::cerr << "]} ) NumberOfCalls = "
              << WrapperInternals::internalStateGetNumberOfCalls(static_cast<struct WrapperInternals::internalState *>(p_state))
              << " >>>" << std::endl;
  }
#endif

  return WRAPPER_OK;
}




/**
 * Finalization function
 * This function is called once just before the wrapper is unloaded. It is the place to flush
 * any output file or free any allocated memory. When this function returns, the wrapper is supposed
 * to have all its work done, so it is not possible to get anymore information from it after that.
 */
enum WrapperErrorCode hess_finalize_compute_deviation(void * p_state)
{
#ifdef DEBUG
  std::cerr << "<<< in void hess_finalize_compute_deviation() >>>" << std::endl;
#endif
  return WRAPPER_OK;
}



WRAPPER_END

END_C_DECLS
