//                                               -*- C++ -*-
/**
 *  @brief A WrapperObject binds a dynamic library symbol to itself
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
#include <sstream>               // for std::istringstream
#include <algorithm>             // for std::min
#include <signal.h>
#include "OTconfig.hxx"
#include "WrapperObject.hxx"
#include "LibraryLoader.hxx"
#include "OSS.hxx"
#include "Log.hxx"
#include "Path.hxx"
#include "ResourceMap.hxx"
#include "WrapperInterface.h"
#include "WrapperCommon_macros.h"
#include "WrapperCommon_extern.h"
#include "WrapperCommonFunctions.hxx"
#include "WrapperMacros.h"
#include "TBB.hxx"
#include "FieldImplementation.hxx"
#ifdef __MINGW32__
#include <sys/time.h> // gettimeofday
#endif
#include "AtomicFunctions.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(WrapperObject);

// Sub-classes definitions
WrapperObject::ObjectPrefix::ObjectPrefix()
  : std::vector<String>(WrapperObject::ObjectPrefixSize)
{
  ObjectPrefix & ref_me = *this;
  ref_me[WrapperObject::FUNCTION]        = "func_";
  ref_me[WrapperObject::GRADIENT]        = "grad_";
  ref_me[WrapperObject::HESSIAN]         = "hess_";
}

WrapperObject::FunctionPrefix::FunctionPrefix()
  : std::vector<String>(WrapperObject::FunctionPrefixSize)
{
  FunctionPrefix & ref_me = *this;
  ref_me[WrapperObject::STATE_CREATION]       = "createState_";
  ref_me[WrapperObject::STATE_DELETION]       = "deleteState_";
  ref_me[WrapperObject::INFORMATION]          = "getInfo_";
  ref_me[WrapperObject::INITIALIZATION]       = "init_";
  ref_me[WrapperObject::EXECUTION]            = "exec_";
  ref_me[WrapperObject::EXECUTION_SAMPLE]     = "exec_sample_";
  ref_me[WrapperObject::EXECUTION_FIELD]      = "exec_field_";
  ref_me[WrapperObject::FINALIZATION]         = "finalize_";
}

// Initialization of class members
const WrapperObject::ObjectPrefix   WrapperObject::ObjectPrefix_;
const WrapperObject::FunctionPrefix WrapperObject::FunctionPrefix_;








WrapperObject::Error::Error()
  : p_error_(new WrapperError)
{
  struct WrapperError * p_wrapperError = static_cast<struct WrapperError *>( p_error_.get() );
  pthread_mutex_init( &(p_wrapperError->mutex), NULL );
  p_wrapperError->length  = 0;
  p_wrapperError->message = NULL;
}

WrapperObject::Error::~Error()
{
  struct WrapperError * p_wrapperError = static_cast<struct WrapperError *>( p_error_.get() );
  clearWrapperError( p_wrapperError );
  pthread_mutex_destroy( &(p_wrapperError->mutex) );
}

const struct WrapperError * WrapperObject::Error::get() const
{
  return p_error_.get();
}

struct WrapperError * WrapperObject::Error::get()
{
  return p_error_.get();
}







struct WrapperSymbols : public Object
{
  GetWrapperInformationFunctionPointer getInfoSymbol_;
  InitializationFunctionPointer initSymbol_;
  ExecutionFunctionPointer execSymbol_;
  ExecutionSampleFunctionPointer execSampleSymbol_;
  ExecutionFieldFunctionPointer execFieldSymbol_;
  GradientFunctionPointer gradSymbol_;
  HessianFunctionPointer hessSymbol_;
  FinalizationFunctionPointer finalizeSymbol_;
  StateCreationFunctionPointer stateCreationSymbol_;
  StateDeletionFunctionPointer stateDeletionSymbol_;
  WrapperSymbols()
    : getInfoSymbol_(0),
      initSymbol_(0),
      execSymbol_(0),
      execSampleSymbol_(0),
      execFieldSymbol_(0),
      gradSymbol_(0),
      hessSymbol_(0),
      finalizeSymbol_(0),
      stateCreationSymbol_(0),
      stateDeletionSymbol_(0)
  {}
  ~WrapperSymbols() throw() {}



  static enum WrapperErrorCode optionalWrapperFunction(...)
  {
    const char FUNCTIONNAME[] = "optionalWrapperFunction";
    wrapper_printEntrance(FUNCTIONNAME);
    wrapper_printExit(FUNCTIONNAME);

    return WRAPPER_OK;
  }

  static enum WrapperErrorCode defaultWrapperGetInfoFunction(void * p_state,
      struct WrapperInformation * p_info,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error)
  {
    const char FUNCTIONNAME[] = "defaultWrapperGetInfoFunction";
    wrapper_printEntrance(FUNCTIONNAME);
    wrapper_printState(FUNCTIONNAME, p_state);

    p_info->inSize_  = wrapper_getNumberOfVariables(p_exchangedData, WRAPPER_IN);
    p_info->outSize_ = wrapper_getNumberOfVariables(p_exchangedData, WRAPPER_OUT);

    wrapper_printWrapperInformation(FUNCTIONNAME, p_info);
    wrapper_printExit(FUNCTIONNAME);

    return WRAPPER_OK;
  }

  static enum WrapperErrorCode defaultWrapperCreateStateFunction(void ** p_p_state,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error)
  {
    const char FUNCTIONNAME[] = "defaultWrapperCreateStateFunction";
    wrapper_printEntrance(FUNCTIONNAME);
    if (!p_p_state)
    {
      setWrapperError( p_error, "No storage available for internal state" );
      return WRAPPER_CANNOT_CREATE_STATE;
    }
    wrapper_printState(FUNCTIONNAME, *p_p_state);

    if (!p_exchangedData)
    {
      setWrapperError( p_error, "No exchanged data sent by the platform" );
      return WRAPPER_INTERNAL_ERROR;
    }

    wrapper_printWrapperExchangedData(FUNCTIONNAME, p_exchangedData);
    wrapper_printExit(FUNCTIONNAME);

    return WRAPPER_OK;
  }

  static enum WrapperErrorCode defaultWrapperDeleteStateFunction(void * p_state,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error)
  {
    const char FUNCTIONNAME[] = "defaultWrapperDeleteStateFunction";
    wrapper_printEntrance(FUNCTIONNAME);

    wrapper_printState(FUNCTIONNAME, p_state);
    wrapper_printExit(FUNCTIONNAME);

    return WRAPPER_OK;
  }

  static enum WrapperErrorCode defaultWrapperExecFunction(void * p_state,
      const struct point * inPoint,
      struct point * outPoint,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error)
  {
    const char FUNCTIONNAME[] = "defaultWrapperExecFunction";
    wrapper_printEntrance(FUNCTIONNAME);
    wrapper_printState(FUNCTIONNAME, p_state);
    wrapper_printPoint(FUNCTIONNAME, inPoint);

    /* We save the current working directory for a future come back */
    char * currentWorkingDirectory = wrapper_getCurrentWorkingDirectory( p_error );
    if (currentWorkingDirectory == NULL) return WRAPPER_EXECUTION_ERROR;

    /* We build a temporary directory in which we will work */
    char * temporaryDirectory = wrapper_createTemporaryDirectory( wrapper_getUserPrefix(p_exchangedData), p_exchangedData, p_error );
    if (temporaryDirectory == NULL) return WRAPPER_EXECUTION_ERROR;

    /* We create the input files for the external code in the temporary directory */
    if (wrapper_createInputFiles(temporaryDirectory, p_exchangedData, inPoint, p_error ))
      return WRAPPER_EXECUTION_ERROR;

    /* The real computation is here */
    long rc = wrapper_runInsulatedCommand( temporaryDirectory, p_exchangedData, inPoint, p_error );

    /* Read the output values */
    if ( rc == 0 )
      if (wrapper_readOutputFiles( temporaryDirectory, p_exchangedData, outPoint, p_error ))
      {
        const char * msg = wrapper_getError( p_error );
        wrapper_setError( p_error, "Can't read output files. Reason: %s", msg );
        free( (void *) msg );
        return WRAPPER_EXECUTION_ERROR;
      }

    /* We kill the temporary directory */
    wrapper_deleteTemporaryDirectory( temporaryDirectory, rc, p_error );

    free( currentWorkingDirectory );

    if ( rc != 0 )
    {
      const char * msg = wrapper_getError( p_error );
      wrapper_setError( p_error, "Code returned error %d. Reason: %s", rc, msg );
      free( (void *) msg );
      return WRAPPER_EXECUTION_ERROR;
    }

    wrapper_printPoint(FUNCTIONNAME, outPoint);
    wrapper_printExit(FUNCTIONNAME);

    return WRAPPER_OK;
  }


#ifdef OPENTURNS_HAVE_TBB
  struct RunCodeFunctor
  {
    ExecutionFunctionPointer func_;
    void * p_state_;
    const struct sample * inSample_;
    struct sample * outSample_;
    const struct WrapperExchangedData * p_exchangedData_;
    void * p_error_;
    AtomicInt & count_;
    const int size_;
    const int width_;
    const int modulo_;

    RunCodeFunctor(ExecutionFunctionPointer func,
                   void * p_state,
                   const struct sample * inSample,
                   struct sample * outSample,
                   const struct WrapperExchangedData * p_exchangedData,
                   void * p_error,
                   AtomicInt & count)
      : func_(func),
        p_state_(p_state),
        inSample_(inSample),
        outSample_(outSample),
        p_exchangedData_(p_exchangedData),
        p_error_(p_error),
        count_(count),
        size_(inSample ? inSample->size_ : 0),
        width_( size_ != 0 ? ceil( log10( 1.0 * size_ ) ) : 1 ),
        modulo_( size_ > 1000 ? size_ / 1000 : size_ )
    {}

    inline
    void incrementCount() const
    {
      int count = count_.fetchAndAdd( 1 );
      if ( (count % modulo_) == 0 )
      {
        double ratio = 100. * count / size_;
        LOGINFO( OSS() << "Progression : "
                 << std::setw(7) << ratio << "% ( "
                 << std::setw(width_) << count << " / "
                 << std::setw(width_) << size_ << " )"
               );
      }
    }

    void operator() ( const tbb::blocked_range<UnsignedInteger> & r ) const
    {
      enum WrapperErrorCode rc;
      for( UnsignedInteger i = r.begin(); i != r.end(); ++i )
      {
        struct point inPoint;
        inPoint.size_ = inSample_->dimension_;
        inPoint.data_ = &( inSample_->data_[ i * inSample_->dimension_ ] );
        struct point outPoint;
        outPoint.size_ = outSample_->dimension_;
        outPoint.data_ = &( outSample_->data_[ i * outSample_->dimension_ ] );
        rc = (*func_)( p_state_, &inPoint, &outPoint, p_exchangedData_, p_error_ );
        if (rc != WRAPPER_OK) throw WrapperInternalException(HERE)
              << "Wrapper function returned error message: " << wrapper_getErrorAsString(rc)
              << ". Reason: " << getWrapperError( p_error_ );
        incrementCount();
      }
    }

  }; /* end struct RunCodeFunctor */

  enum WrapperErrorCode defaultWrapperExecSampleFunction_TBB(void * p_state,
      const struct sample * inSample,
      struct sample * outSample,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error) const
  {
    const char FUNCTIONNAME[] = "defaultWrapperExecSampleFunction_TBB";
    wrapper_printEntrance(FUNCTIONNAME);
    wrapper_printState(FUNCTIONNAME, p_state);
    wrapper_printSample(FUNCTIONNAME, inSample);

    const UnsignedInteger size = inSample->size_;
    const UnsignedInteger grainSize = ResourceMap::GetAsUnsignedInteger( "parallel-execution-grainsize" );

    AtomicInt count;
    RunCodeFunctor functor( execSymbol_, p_state, inSample, outSample, p_exchangedData, p_error, count );
    try
    {
      tbb::parallel_for( tbb::blocked_range<UnsignedInteger>( 0, size, grainSize ), functor );
      functor.incrementCount();

    }
    catch (const tbb::captured_exception & ex)
    {
      LOGINFO( OSS() << "Catched exception from code execution: " << ex.what() );
      return WRAPPER_EXECUTION_ERROR;

    }
    catch (const Exception & ex)
    {
      LOGINFO( OSS() << "Catched exception from code execution: " << ex );
      return WRAPPER_EXECUTION_ERROR;
    }


    wrapper_printSample(FUNCTIONNAME, outSample);
    wrapper_printExit(FUNCTIONNAME);
    return WRAPPER_OK;
  }

#elif defined(_MSC_VER) /* OPENTURNS_HAVE_TBB */
#pragma message ( "WARNING: WrapperObject.cxx has currently been ported on Windows MSVC only with TBB enabled" )

  enum WrapperErrorCode defaultWrapperExecSampleFunction_PTHREAD(void * p_state,
      const struct sample * inSample,
      struct sample * outSample,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error) const
  {
    LOGINFO( OSS() << "Build OpenTURNS with TBB to use wrappers on Windows" );
    return WRAPPER_NOT_IMPLEMENTED;
  }
#else /* OPENTURNS_HAVE_TBB */

  struct AdapterArguments
  {
    ExecutionFunctionPointer func_;
    void * p_state;
    struct WrapperExchangedData * p_exchangedData;
    void * p_error;
    const struct sample * inSample;
    struct sample * outSample;
    long begin;
    long end;
    long threadId;
    enum WrapperErrorCode rc;
    volatile long * p_errorEncountered;
    volatile long index;
  };

  struct ObserverArguments
  {
    long nbThreads;
    long size;
    long count;
    struct AdapterArguments * func_exec_args;
#ifdef WIN32
    pthread_cond_t cancelCond;
    int cancel;
#endif
  };



  static void * defaultWrapperExecAdapterFunction( void * ptr )
  {
    long i;
    struct AdapterArguments * args = REINTERPRET_CAST(struct AdapterArguments *, ptr);
    assert(args);

    struct point inPoint;
    inPoint.size_ = args->inSample->dimension_;

    struct point outPoint;
    outPoint.size_ = args->outSample->dimension_;

    for( i = args->begin ; i < args->end ; ++i )
    {
      long error = Atomic::FetchAndAdd( (int*) args->p_errorEncountered, 0 ); /* Test for error */

      if (error) break;

      inPoint.data_  = & (args->inSample->data_ [i * args->inSample->dimension_ ]);
      outPoint.data_ = & (args->outSample->data_[i * args->outSample->dimension_]);

      args->rc = (*args->func_) ( args->p_state,
                                  &inPoint,
                                  &outPoint,
                                  args->p_exchangedData,
                                  args->p_error );
      if (args->rc != WRAPPER_OK)
      {
        Atomic::Increment( (int*) args->p_errorEncountered ); /* Set error */

        const char * err = NULL;
        err = wrapper_getError( args->p_error );
        LOGWARN( OSS() << "code returned error rc=" << wrapper_getErrorAsString( args->rc ) << ". Reason: " << err );
        free( (char *) err );

        break;
      }
      Atomic::Increment( (int*) & (args->index) ); /* Increment shared local counter */

    } /* end for */
    return &( args->rc );
  }

  static void threadObserver_printProgression( void * ptr )
  {
    if (ResourceMap::Get("computation-progression-visible") == "true")
    {

      struct ObserverArguments * args = REINTERPRET_CAST(struct ObserverArguments *, ptr);
      assert(args);

      long nbThreads = args->nbThreads;
      struct AdapterArguments * func_exec_args = args->func_exec_args;

      args->count = 0;
      for (long i = 0; i < nbThreads; ++i)
        args->count += Atomic::FetchAndAdd( (int*) & (func_exec_args[i].index), 0 ); /* Get the shared local counters */


      double ratio = 100.0 * args->count / args->size;
      int width = static_cast<int>( (args->size != 0 ? ceil( log10( 1.0 * args->size ) ) : 1) );
      LOGINFO( OSS() << "Progression : "
               << std::setw(7) << ratio << "% ( "
               << std::setw(width) << args->count << " / "
               << std::setw(width) << args->size << " )"
             );
    }
  }

  static void * threadObserver( void * ptr )
  {
    struct ObserverArguments * args = REINTERPRET_CAST(struct ObserverArguments *, ptr);
    assert(args);

    struct AdapterArguments * func_exec_args = args->func_exec_args;

#ifndef WIN32
    pthread_cleanup_push( threadObserver_printProgression, ptr );
#else
    pthread_mutex_t cancelMutex;
    pthread_mutex_init( &cancelMutex, NULL );
#endif

    long error = 0;
    while ( (error = Atomic::FetchAndAdd( (int*) func_exec_args[0].p_errorEncountered, 0 )) == 0
#ifdef WIN32
            && args->cancel == 0
#endif
          ) /* Test for error */
    {


      threadObserver_printProgression( ptr );
      if (args->count == args->size) break;

      // Go to sleep
      const long T = ResourceMap::GetAsUnsignedInteger("computation-progression-update-interval");
      for (long i = 0; i < T; ++i)
      {
#ifdef __MINGW32__
        struct timeval  tp;
        gettimeofday( &tp, NULL );
        struct timespec ts;
        ts.tv_sec  = tp.tv_sec;
        ts.tv_nsec = tp.tv_usec * 1000;
        ts.tv_sec += 1;

        pthread_mutex_lock( &cancelMutex );
        if (args->cancel == 0)
          pthread_cond_timedwait( &(args->cancelCond), &cancelMutex, &ts );
        pthread_mutex_unlock( &cancelMutex );
#else
        pthread_testcancel();
        sleep(1);
#endif
      }
    } /* end while */

#ifndef WIN32
    pthread_cleanup_pop( 0 );
#else
    threadObserver_printProgression( ptr );
    pthread_mutex_destroy( &cancelMutex );
#endif
    return NULL;
  }

  enum WrapperErrorCode defaultWrapperExecSampleFunction_PTHREAD(void * p_state,
      const struct sample * inSample,
      struct sample * outSample,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error) const
  {
    const char FUNCTIONNAME[] = "defaultWrapperExecSampleFunction_PTHREAD";
    wrapper_printEntrance(FUNCTIONNAME);
    wrapper_printState(FUNCTIONNAME, p_state);
    wrapper_printSample(FUNCTIONNAME, inSample);

    long nbThreads;
    long i;
    long chunk;
    long remainder;
    long begin;
    long end;
    pthread_t * threadsIds;
    pthread_t observerId;
    int rc;
    enum WrapperErrorCode funcRc = WRAPPER_OK;
    struct AdapterArguments * func_exec_args;
    struct ObserverArguments observer_args;
    volatile long errorEncountered = 0;
    struct WrapperExchangedData * p_exchangedData_copy = NULL;
    char * threadSpecificTempDir = NULL;
    char * prefix = NULL;

    nbThreads = std::min( wrapper_getNumberOfCPUs( p_exchangedData ), inSample->size_ );

    threadsIds = REINTERPRET_CAST(pthread_t *, calloc( nbThreads, sizeof(pthread_t) ) );
    if (threadsIds == NULL)
    {
      wrapper_setError( p_error,  "No memory to allocate the thread id table" );
      return WRAPPER_EXECUTION_ERROR;
    }

    func_exec_args = REINTERPRET_CAST(struct AdapterArguments *, calloc( nbThreads, sizeof(struct AdapterArguments) ) );
    if (func_exec_args == NULL)
    {
      wrapper_setError( p_error, "No memory to allocate the thread arguments table" );
      return WRAPPER_EXECUTION_ERROR;
    }


    /* we compute the chunk size that each thread will serve */
    chunk     = inSample->size_ / nbThreads ;
    remainder = inSample->size_ % nbThreads ;
    begin = 0;
    end = chunk + ( (remainder-- > 0) ? 1 : 0 );

    prefix = newFormattedString( "openturns_th%lu", pthread_self() );
    if (prefix == NULL)
    {
      wrapper_setError( p_error, "No memory to allocate the path of the thread specific temporary directory" );
      funcRc = WRAPPER_EXECUTION_ERROR;
      goto ERR;
    }

    threadSpecificTempDir = wrapper_createTemporaryDirectory( prefix, p_exchangedData, p_error );
    if (threadSpecificTempDir == NULL)
    {
      const char * err = NULL;
      err = wrapper_getError( p_error );
      wrapper_setError( p_error, "Unable to create the thread specific temporary directory. Reason: %s", err );
      free( (char *) err );
      funcRc =  WRAPPER_EXECUTION_ERROR;
      goto ERR;
    }

    /* we create a thread specific exchanged data with an individual temporary directory */
    if (wrapper_copyWrapperExchangedData( &p_exchangedData_copy, p_exchangedData, p_error ))
    {
      wrapper_setError( p_error, "No memory to allocate thread specific exchanged data structure" );
      funcRc =  WRAPPER_EXECUTION_ERROR;
      goto ERR;
    }

    free( p_exchangedData_copy->platform_->generalTemporaryDirectory_ );
    p_exchangedData_copy->platform_->generalTemporaryDirectory_ = strdup( threadSpecificTempDir );

    // Ask insulateCommand() to create one shared script. See comment in insulateCommand().
    lock( p_exchangedData_copy );
    if (p_exchangedData_copy->platform_->command_path_ == NULL)
    {
      p_exchangedData_copy->platform_->command_path_ = strdup( "" );
      if (p_exchangedData_copy->platform_->command_path_ == NULL) funcRc = WRAPPER_EXECUTION_ERROR_NO_RETRY;
    }
    unlock( p_exchangedData_copy );

    if (funcRc != WRAPPER_OK) goto ERR;


    for( i = 0; i < nbThreads; ++i )
    {
      func_exec_args[i].func_              = execSymbol_;
      func_exec_args[i].p_state            = p_state;
      func_exec_args[i].p_exchangedData    = p_exchangedData_copy;
      func_exec_args[i].p_error            = p_error;
      func_exec_args[i].inSample           = inSample;
      func_exec_args[i].outSample          = outSample;
      func_exec_args[i].begin              = begin;
      func_exec_args[i].end                = end;
      func_exec_args[i].threadId           = i;
      func_exec_args[i].rc                 = WRAPPER_OK;
      func_exec_args[i].p_errorEncountered = &errorEncountered;
      func_exec_args[i].index              = 0;
      begin = end;
      end   = begin + chunk + ( (remainder-- > 0) ? 1 : 0 );
    } /* end for */

    // We set and launch the abserver
    observer_args.nbThreads = nbThreads;
    observer_args.size = inSample->size_;
    observer_args.count = 0;
    observer_args.func_exec_args = func_exec_args;
#ifdef WIN32
    observer_args.cancel = 0;
    pthread_cond_init( &(observer_args.cancelCond), NULL );
#endif
    pthread_create( &observerId, NULL, threadObserver, &observer_args );


    for( i = 0; i < nbThreads; ++i )
    {
      rc = pthread_create( threadsIds + i, NULL,
                           defaultWrapperExecAdapterFunction,
                           func_exec_args + i );
      if (rc != 0)
      {
        int j;
        for( j = i - 1 ; j >= 0 ; --j ) pthread_kill( threadsIds[j], SIGTERM );
        wrapper_setError( p_error, "Thread creation failed" );
        funcRc = WRAPPER_EXECUTION_ERROR;
        goto ERR;
      }
    } /* end for */

    for( i = 0; i < nbThreads; ++i )
    {
      int * threadRc;
      rc = pthread_join( threadsIds[i], (void**) &threadRc );
      if (rc != 0)
      {
        wrapper_setError( p_error, "Thread join failed" );
        funcRc = WRAPPER_EXECUTION_ERROR;
      }
      else if (*threadRc != WRAPPER_OK)
      {
        const char * msg = wrapper_getError( p_error );
        wrapper_setError( p_error, "Execution failure. Return code was %d. Last message was: %s", *threadRc, msg );
        free( (void *) msg );
        funcRc = WRAPPER_EXECUTION_ERROR_NO_RETRY;
      }
    } /* end for */

    // We join the observer
#ifndef WIN32
    pthread_cancel( observerId );
#else
    // no mutex needed: no strong concurrent access protection needed
    observer_args.cancel = 1;
    pthread_cond_signal( &(observer_args.cancelCond) );
#endif

    pthread_join( observerId, NULL );
#ifdef WIN32
    pthread_cond_destroy( &(observer_args.cancelCond) );
#endif

    lock( p_exchangedData_copy );
#ifndef WIN32
    unlink( p_exchangedData_copy->platform_->command_path_ );
#endif
    free( p_exchangedData_copy->platform_->command_path_ );
    p_exchangedData_copy->platform_->command_path_ = NULL;
    unlock( p_exchangedData_copy );


    if (rmdir(threadSpecificTempDir))
    {
      const char * msg = wrapper_getError( p_error );
      wrapper_setError( p_error, "Can't remove directory %s. Last message was: %s", threadSpecificTempDir, msg );
      free( (void *) msg );
      funcRc = WRAPPER_EXECUTION_ERROR_NO_RETRY;
    }
    free( threadSpecificTempDir );
    threadSpecificTempDir = NULL;

    if (funcRc == WRAPPER_OK) goto OK;

ERR:
    free( threadsIds );
    threadsIds = NULL;
    free( func_exec_args );
    func_exec_args = NULL;
    free( threadSpecificTempDir );
    threadSpecificTempDir = NULL;
    wrapper_freeWrapperExchangedData( p_exchangedData_copy );
    free( prefix );
    prefix = NULL;
    return funcRc;

OK:
    //deleteTemporaryDirectory( threadSpecificTempDir, 0, p_error );
    free( threadsIds );
    threadsIds = NULL;
    free( func_exec_args );
    func_exec_args = NULL;
    wrapper_freeWrapperExchangedData( p_exchangedData_copy );
    free( prefix );
    prefix = NULL;

    wrapper_printSample(FUNCTIONNAME, outSample);
    wrapper_printExit(FUNCTIONNAME);
    return WRAPPER_OK;
  }
#endif /* OPENTURNS_HAVE_TBB */

  enum WrapperErrorCode defaultWrapperExecSampleFunction(void * p_state,
      const struct sample * inSample,
      struct sample * outSample,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error) const
  {
#ifdef OPENTURNS_HAVE_TBB
    return defaultWrapperExecSampleFunction_TBB( p_state, inSample, outSample, p_exchangedData, p_error );
#else /* OPENTURNS_HAVE_TBB */
    return defaultWrapperExecSampleFunction_PTHREAD( p_state, inSample, outSample, p_exchangedData, p_error );
#endif /* OPENTURNS_HAVE_TBB */
  }

  enum WrapperErrorCode defaultWrapperExecFieldFunction(void * p_state,
      const struct field * inField,
      struct field * outField,
      const struct WrapperExchangedData * p_exchangedData,
      void * p_error) const
  {
    // #ifdef OPENTURNS_HAVE_TBB
    //     return defaultWrapperExecSampleFunction_TBB( p_state, inSample, outSample, p_exchangedData, p_error );
    // #else /* OPENTURNS_HAVE_TBB */
    //     return defaultWrapperExecSampleFunction_PTHREAD( p_state, inSample, outSample, p_exchangedData, p_error );
    // #endif /* OPENTURNS_HAVE_TBB */
    return WRAPPER_OK;
  }

  static enum WrapperErrorCode notDefinedFunction(...)
  {
    const char FUNCTIONNAME[] = "notDefinedFunction";
    wrapper_printEntrance(FUNCTIONNAME);
    wrapper_printExit(FUNCTIONNAME);
    return WRAPPER_NOT_IMPLEMENTED;
  }


  CLASSNAME;
};

CLASSNAMEINIT(WrapperSymbols);





/* Default constructor */
WrapperObject::WrapperObject(const FileName & libraryPath,
                             const String & symbolName,
                             const WrapperData & data,
                             ObjectType o
                            )
  : PersistentObject(),
    handle_(LibraryLoader::GetInstance().load(libraryPath)),
    data_(data),
    p_exchangedData_(data.getNewWrapperExchangedDataForCInterface()),
    type_(o),
    error_(),
    wrapperSymbols_(new WrapperSymbols),
    wrapperInfo_(0)
{
  setName(symbolName);
  if (symbolName.empty())
  {
    String name;
    switch (type_)
    {
      case WrapperObject::FUNCTION:
        name = "function";
        break;
      case WrapperObject::GRADIENT:
        name = "gradient";
        break;
      case WrapperObject::HESSIAN:
        name = "hessian";
        break;
      default:
        name = "unknown";
    };
    throw WrapperInternalException(HERE) << "No symbol name for " << name << " in library " << libraryPath;
  }

  const Bool optional = true;
  const LibrarySymbol optWrpFunc = REINTERPRET_CAST( LibrarySymbol, &WrapperSymbols::optionalWrapperFunction           );
  const LibrarySymbol defWrpFunc = REINTERPRET_CAST( LibrarySymbol, &WrapperSymbols::defaultWrapperGetInfoFunction     );
  const LibrarySymbol notDefFunc = REINTERPRET_CAST( LibrarySymbol, &WrapperSymbols::notDefinedFunction                );
  const LibrarySymbol createFunc = REINTERPRET_CAST( LibrarySymbol, &WrapperSymbols::defaultWrapperCreateStateFunction );
  const LibrarySymbol deleteFunc = REINTERPRET_CAST( LibrarySymbol, &WrapperSymbols::defaultWrapperDeleteStateFunction );
  const LibrarySymbol execFunc   = REINTERPRET_CAST( LibrarySymbol, &WrapperSymbols::defaultWrapperExecFunction        );


  wrapperSymbols_->getInfoSymbol_        = REINTERPRET_CAST(GetWrapperInformationFunctionPointer,
      getSymbol(getFunctionName(INFORMATION), optional, defWrpFunc) );
  wrapperSymbols_->stateCreationSymbol_  = REINTERPRET_CAST(StateCreationFunctionPointer,
      getSymbol(getFunctionName(STATE_CREATION), optional, createFunc) );
  wrapperSymbols_->stateDeletionSymbol_  = REINTERPRET_CAST(StateDeletionFunctionPointer,
      getSymbol(getFunctionName(STATE_DELETION), optional, deleteFunc) );
  wrapperSymbols_->initSymbol_           = REINTERPRET_CAST(InitializationFunctionPointer,
      getSymbol(getFunctionName(INITIALIZATION), optional, optWrpFunc) );
  wrapperSymbols_->finalizeSymbol_       = REINTERPRET_CAST(FinalizationFunctionPointer,
      getSymbol(getFunctionName(FINALIZATION), optional, optWrpFunc) );
  wrapperSymbols_->execSymbol_           = REINTERPRET_CAST(ExecutionFunctionPointer,
      getSymbol(getFunctionName(EXECUTION), optional, execFunc) ) ;
  wrapperSymbols_->gradSymbol_           = REINTERPRET_CAST(GradientFunctionPointer,
      getSymbol(getFunctionName(EXECUTION), optional, notDefFunc) );
  wrapperSymbols_->hessSymbol_           = REINTERPRET_CAST(HessianFunctionPointer,
      getSymbol(getFunctionName(EXECUTION), optional, notDefFunc) );
  wrapperSymbols_->execSampleSymbol_     = REINTERPRET_CAST(ExecutionSampleFunctionPointer,
      getSymbol(getFunctionName(EXECUTION_SAMPLE), optional, 0) );
  wrapperSymbols_->execFieldSymbol_ = REINTERPRET_CAST(ExecutionFieldFunctionPointer,
                                      getSymbol(getFunctionName(EXECUTION_FIELD), optional, 0) );

  // Bind symbols in wrapper so wrapper can call back internal functions.
  // Compute the number of methods to bind in order to size the array
  int nbMethods = 0;
#undef BIND_ACTION
#define BIND_ACTION(rtype, name, args) ++nbMethods;
  BIND_METHODS;

  // fill in the array with the pointer of the functions
  METHODS * methodsToBind = new METHODS[nbMethods];
  METHODS * ptr = methodsToBind;
#undef BIND_ACTION
#define BIND_ACTION(rtype, name, args) *ptr++ = reinterpret_cast<METHODS>( wrapper_ ## name );
  BIND_METHODS;

  BindMethodsFunctionPointer bindMethodsSymbol_ = REINTERPRET_CAST(BindMethodsFunctionPointer,
      getSymbol(BIND_METHODS_FUNCTION_NAME) );
  assert( bindMethodsSymbol_ != 0 );
  enum WrapperErrorCode returnCode = (* bindMethodsSymbol_)( methodsToBind );
  if (returnCode != WRAPPER_OK) throw DynamicLibraryException(HERE) << "Method binding error. Report bug.";
  delete [] methodsToBind;
}

/* Destructor */
WrapperObject::~WrapperObject()
{
  WrapperData::FreeWrapperExchangedDataForCInterface( p_exchangedData_ );
}

/* Virtual constructor */
WrapperObject * WrapperObject::clone() const
{
  return new WrapperObject(*this);
}

/* Copy constructor */
WrapperObject::WrapperObject(const WrapperObject & other)
  : PersistentObject(other),
    handle_(other.handle_),
    data_(other.data_),
    p_exchangedData_(data_.getNewWrapperExchangedDataForCInterface()),
    type_(other.type_),
    error_(),
    wrapperSymbols_(new WrapperSymbols),
    wrapperInfo_(0)
{
  // Nothing to do
}

/* Comparison operator */
/* NOT IMPLEMENTED */

/* String converter */
String WrapperObject::__repr__() const
{
  return OSS(true) << "class=" << WrapperObject::GetClassName()
         << " name=" << getName()
         << " handle=" << handle_
         << " type_=" << type_;
}

/* String converter */
String WrapperObject::__str__(const String & offset) const
{
  return OSS(false) << offset << getName() << " -> " << handle_;
}



/* Method getFunctionName returns the name of the symbol
 * in the shared library.
 */
String WrapperObject::getFunctionName(FunctionType f) const
{
  return String().append(ObjectPrefix_[type_]).append(FunctionPrefix_[f]).append(getName());
}


/* Method getInNumericalPointDimension returns the dimension of the in point */
UnsignedInteger WrapperObject::getInNumericalPointDimension(void * p_state) const
{
  if (wrapperInfo_.isNull() )
  {
    wrapperInfo_.reset(new WrapperInformation);
    enum WrapperErrorCode returnCode = (*(wrapperSymbols_->getInfoSymbol_))( p_state, wrapperInfo_.get(), p_exchangedData_, error_.get() );
    if (returnCode != WRAPPER_OK)
      throw WrapperInternalException(HERE)
          << "Wrapper function '" << getFunctionName(INFORMATION)
          << "' returned error message: " << wrapper_getErrorAsString(returnCode)
          << ". Reason: " << getWrapperError( error_.get() );
  }
  return wrapperInfo_->inSize_;
}

/* Method getDescription returns the description of the input and output variables of the functions */
Description WrapperObject::getDescription() const
{
  Description description;
  WrapperData::VariableListType variableList(data_.getVariableList());
  for (UnsignedInteger i = 0; i < variableList.getSize(); ++i) description.add(variableList[i].id_);
  return description;
}


/* Method getInNumericalPointDimension returns the dimension of the out point */
UnsignedInteger WrapperObject::getOutNumericalPointDimension(void * p_state) const
{
  if (wrapperInfo_.isNull() )
  {
    wrapperInfo_.reset(new WrapperInformation);
    enum WrapperErrorCode returnCode = (*(wrapperSymbols_->getInfoSymbol_))( p_state, wrapperInfo_.get(), p_exchangedData_, error_.get() );
    if (returnCode != WRAPPER_OK)
      throw WrapperInternalException(HERE)
          << "Wrapper function '" << getFunctionName(INFORMATION)
          << "' returned error message: " << wrapper_getErrorAsString(returnCode)
          << ". Reason: " << getWrapperError( error_.get() );
  }
  return wrapperInfo_->outSize_;
}


/* Method initialize calls the initializationSymbol of the library */
void WrapperObject::initialize(void * p_state) const
{
  // We initialize the wrapper
  enum WrapperErrorCode returnCode = (*(wrapperSymbols_->initSymbol_))( p_state, p_exchangedData_, error_.get() );
  if (returnCode != WRAPPER_OK)
    throw WrapperInternalException(HERE)
        << "Wrapper function '" << getFunctionName(INITIALIZATION)
        << "' returned error message: " << wrapper_getErrorAsString(returnCode)
        << ". Reason: " << getWrapperError( error_.get() );
}


// TODO : make this method simpler !!!
/* Method execute actually realizes the desired treatment */
/* TODO: MAKE THIS ROUTINE THREAD-SAFE */
NumericalPoint WrapperObject::execute(void * p_state,
                                      const NumericalPoint & inP) const
{
  // Check the in argument
  if (inP.getDimension() != getInNumericalPointDimension(p_state))
    throw InvalidArgumentException(HERE)
        << "Argument 'in' has incorrect dimension (" << inP.getDimension()
        << "). Expected = " << getInNumericalPointDimension(p_state);



  // We create a point structure to embed the in NumericalPoint passed as argument
  struct point inPoint;
  inPoint.size_ = inP.getDimension();
  inPoint.data_ = const_cast<NumericalScalar *>(&inP[0]);

  // We create a point structure to embed the returned out NumericalPoint
  struct point outPoint;
  outPoint.size_ = getOutNumericalPointDimension(p_state);
  NumericalPoint out(outPoint.size_);
  outPoint.data_ = const_cast<NumericalScalar *>(&out[0]);

  if (Log::HasDebug())
  {
    OSS oss;
    oss << "In struct point {size= " << inPoint.size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < inPoint.size_; i++, separator = ",") oss << separator << inPoint.data_[i];
    oss << "]}";
    LOGDEBUG( oss );
  }

  // Then we call the wrapper execution function
  enum WrapperErrorCode returnCode = (*(wrapperSymbols_->execSymbol_))( p_state, & inPoint, & outPoint, p_exchangedData_, error_.get() );

  if (returnCode != WRAPPER_OK)
    throw WrapperInternalException(HERE)
        << "Wrapper function '" << getFunctionName(EXECUTION)
        << "' returned error message: " << wrapper_getErrorAsString(returnCode)
        << ". Reason: " << getWrapperError( error_.get() );


  if (Log::HasDebug())
  {
    OSS oss;
    oss << "Out struct point {size= " << outPoint.size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < outPoint.size_; i++, separator = ",") oss << separator << outPoint.data_[i];
    oss << "]}";
    LOGDEBUG( oss );
  }

  return out;
} // WrapperObject::execute(void * p_state, const NumericalPoint & inP) const



/* Method execute actually realizes the desired treatment */
/* TODO: MAKE THIS ROUTINE THREAD-SAFE */
NumericalSample WrapperObject::execute(void * p_state,
                                       const NumericalSample & inS) const
{
  const UnsignedInteger inDimension = getInNumericalPointDimension(p_state);
  // Check the inS argument
  if (inS.getDimension() != inDimension)
    throw InvalidArgumentException(HERE)
        << "Argument 'inS' has incorrect dimension (" << inS.getDimension()
        << "). Expected = " << inDimension;

  const UnsignedInteger size = inS.getSize();
  const NumericalSampleImplementation & inSi = * inS.getImplementation();

  // We create a point structure to embed the in NumericalPoint passed as argument
  struct sample inSample;
  inSample.size_      = size;
  inSample.dimension_ = inDimension;
  inSample.data_      = const_cast<NumericalScalar *>( &inSi[0][0] );

  // We create the output sample
  const UnsignedInteger outDimension = getOutNumericalPointDimension(p_state);
  NumericalSample outS(size, outDimension);
  NumericalSampleImplementation & outSi = * outS.getImplementation();

  // We create a point structure to embed the returned outS NumericalSample
  struct sample outSample;
  outSample.size_      = size;
  outSample.dimension_ = outDimension;
  outSample.data_      = &outSi[0][0];


  // Then we call the wrapper execution function for a sample
  enum WrapperErrorCode returnCode = WRAPPER_OK;
  try
  {
    returnCode = (wrapperSymbols_->execSampleSymbol_ != 0) ?
                 (*(wrapperSymbols_->execSampleSymbol_))( p_state, & inSample, & outSample, p_exchangedData_, error_.get() ) :
                 wrapperSymbols_->defaultWrapperExecSampleFunction( p_state, & inSample, & outSample, p_exchangedData_, error_.get() ) ;
    if (returnCode != WRAPPER_OK)
      throw WrapperInternalException(HERE) << "Wrapper function '"
                                           << getFunctionName(EXECUTION_SAMPLE)
                                           << "' returned error Message: "
                                           << wrapper_getErrorAsString(returnCode)
                                           << ". Reason: " << getWrapperError( error_.get() );
  }
  catch (WrapperInternalException & ex)
  {
    LOGINFO(OSS() << "Sample evaluation function returned an error. Reason: " << ex ) ;
    throw ex;
  }

  return outS;
}


Field WrapperObject::execute(void * p_state,
                             const Field & inFld) const
{
  const UnsignedInteger inDimension = getInNumericalPointDimension(p_state);
  // Check the inFld argument
  if (inFld.getDimension() != inDimension)
    throw InvalidArgumentException(HERE)
        << "Argument 'inFld' has incorrect dimension (" << inFld.getDimension()
        << "). Expected = " << inDimension;

  const UnsignedInteger size = inFld.getSize();

  // We create a point structure to embed the in NumericalPoint passed as argument
  struct field inField;
  inField.size_      = size;
  inField.dimension_ = inDimension;
  // TO BE IMPLEMENTED
  //  inField.data_      = const_cast<NumericalScalar *>( &inFieldi->getValues()[0] );
  //  inField.vertices_  = const_cast<UnsignedScalar *>( &inFieldi->getMesh().
  // We create the output sample
  const UnsignedInteger outDimension = getOutNumericalPointDimension(p_state);
  Field outFld;

  // We create a point structure to embed the returned outS NumericalSample
  struct field outField;
  outField.size_      = size;
  outField.dimension_ = outDimension;
  // outField.data_      = &outTSi[0][0];


  // Then we call the wrapper execution function for a sample
  enum WrapperErrorCode returnCode = WRAPPER_OK;
  try
  {
    returnCode = (wrapperSymbols_->execFieldSymbol_ != 0) ?
                 (*(wrapperSymbols_->execFieldSymbol_))( p_state, & inField, & outField, p_exchangedData_, error_.get() ) :
                 wrapperSymbols_->defaultWrapperExecFieldFunction( p_state, & inField, & outField, p_exchangedData_, error_.get() ) ;
    if (returnCode != WRAPPER_OK)
      throw WrapperInternalException(HERE) << "Wrapper function '"
                                           << getFunctionName(EXECUTION_FIELD)
                                           << "' returned error Message: "
                                           << wrapper_getErrorAsString(returnCode)
                                           << ". Reason: " << getWrapperError( error_.get() );
  }
  catch (WrapperInternalException & ex)
  {
    LOGINFO(OSS() << "Field evaluation function returned an error. Reason: " << ex ) ;
    throw ex;
  }

  return outFld;
}


/* Method gradient actually realizes the desired treatment */
Matrix WrapperObject::gradient(void * p_state, const NumericalPoint & inP) const
{
  // Check the inP argument
  if (inP.getDimension() != getInNumericalPointDimension(p_state))
    throw InvalidArgumentException(HERE)
        << "Argument 'inP' has incorrect size (" << inP.getDimension()
        << "). Expected = " << getInNumericalPointDimension(p_state);



  // We create a point structure to embed the inP NumericalPoint passed as argument
  struct point inPoint;
  inPoint.size_ = inP.getDimension();
  inPoint.data_ = new double[inPoint.size_];
  for(unsigned int i = 0; i < inPoint.size_; i++) inPoint.data_[i] = inP[i];

  // We create a point structure to embed the returned out NumericalPoint
  struct matrix outMatrix;
  outMatrix.nbrows_ = getInNumericalPointDimension(p_state);
  outMatrix.nbcols_ = getOutNumericalPointDimension(p_state);
  outMatrix.data_   = new double[outMatrix.nbrows_ * outMatrix.nbcols_];

  if (Log::HasDebug())
  {
    OSS oss;
    oss << "In struct point {size= " << inPoint.size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < inPoint.size_; i++, separator = ",") oss << separator << inPoint.data_[i];
    oss << "]}";
    LOGDEBUG( oss );
  }

  // Then we call the wrapper execution function
  enum WrapperErrorCode returnCode = (*(wrapperSymbols_->gradSymbol_))( p_state, & inPoint, & outMatrix, p_exchangedData_, error_.get() );
  if (returnCode != WRAPPER_OK)
    throw WrapperInternalException(HERE)
        << "Wrapper function '" << getFunctionName(EXECUTION)
        << "' returned error message: " << wrapper_getErrorAsString(returnCode)
        << ". Reason: " << getWrapperError( error_.get() );




  if (Log::HasDebug())
  {
    OSS oss;
    oss << "Out struct matrix {rows= " << outMatrix.nbrows_
        << ", cols=" << outMatrix.nbcols_ << ", data=[[";
    const char * separator = "";
    for(unsigned int i = 0; i < outMatrix.nbrows_; i++, separator = "],[")
      for(unsigned int j = 0; j < outMatrix.nbcols_; j++, separator = ",")
        oss << separator << outMatrix.data_[i * outMatrix.nbcols_ + j];
    oss << "]]}";
    LOGDEBUG( oss );
  }

  // Should be improved ! No need to double-copy data
  // Collection<double> temporaryCollection(outMatrix.data_, outMatrix.data_+(outMatrix.nbrows_*outMatrix.nbcols_));
  Matrix out(outMatrix.nbrows_, outMatrix.nbcols_, outMatrix.data_, outMatrix.data_ + (outMatrix.nbrows_ * outMatrix.nbcols_) );

  delete [] inPoint.data_;
  delete [] outMatrix.data_;

  return out;
}


/* Method hessian actually realizes the desired treatment */
SymmetricTensor WrapperObject::hessian(void * p_state, const NumericalPoint & inP) const
{
  // Check the inP argument
  if (inP.getDimension() != getInNumericalPointDimension(p_state))
    throw InvalidArgumentException(HERE)
        << "Argument 'inP' has incorrect size (" << inP.getDimension()
        << "). Expected = " << getInNumericalPointDimension(p_state);


  // We create a point structure to embed the inP NumericalPoint passed as argument
  struct point inPoint;
  inPoint.size_ = inP.getDimension();
  inPoint.data_ = new double[inPoint.size_];
  for(unsigned int i = 0; i < inPoint.size_; i++) inPoint.data_[i] = inP[i];

  // We create a point structure to embed the returned out NumericalPoint
  struct tensor outTensor;
  outTensor.nbrows_   = getInNumericalPointDimension(p_state);
  outTensor.nbcols_   = getInNumericalPointDimension(p_state);
  outTensor.nbsheets_ = getOutNumericalPointDimension(p_state);
  outTensor.data_     = new double[outTensor.nbrows_ * outTensor.nbcols_ * outTensor.nbsheets_];

  if (Log::HasDebug())
  {
    OSS oss;
    oss << "In struct point {size= " << inPoint.size_ << ", data=[";
    const char * separator = "";
    for(unsigned int i = 0; i < inPoint.size_; i++, separator = ",") oss << separator << inPoint.data_[i];
    oss << "]}";
    LOGDEBUG( oss );
  }

  // Then we call the wrapper execution function
  enum WrapperErrorCode returnCode = (*(wrapperSymbols_->hessSymbol_))( p_state, & inPoint, & outTensor, p_exchangedData_, error_.get() );
  if (returnCode != WRAPPER_OK)
    throw WrapperInternalException(HERE)
        << "Wrapper function '" << getFunctionName(EXECUTION)
        << "' returned error message: " << wrapper_getErrorAsString(returnCode)
        << ". Reason: " << getWrapperError( error_.get() );


  if (Log::HasDebug())
  {
    OSS oss;
    oss << "Out struct tensor {rows= " << outTensor.nbrows_
        << ", cols=" << outTensor.nbcols_
        << ", sheets=" << outTensor.nbsheets_
        << ", data=[[[";
    const char * separator = "";
    for(unsigned int i = 0; i < outTensor.nbrows_; i++, separator = "],[")
      for(unsigned int j = 0; j < outTensor.nbcols_; j++, separator = "],[")
        for(unsigned int k = 0; k < outTensor.nbsheets_; k++, separator = ",")
          oss << separator << outTensor.data_[(i * outTensor.nbcols_ + j) * outTensor.nbsheets_ + k];
    oss << "]]]}";
    LOGDEBUG( oss );
  }

  SymmetricTensor out(outTensor.nbrows_,
                      /* outTensor.nbcols_, */
                      outTensor.nbsheets_,
                      outTensor.data_,
                      outTensor.data_ + (outTensor.nbrows_ *
                                         outTensor.nbcols_ *
                                         outTensor.nbsheets_));

  delete [] inPoint.data_;
  delete [] outTensor.data_;

  return out;
}




/* Method finalize calls the finalizationSymbol of the library */
void WrapperObject::finalize(void * p_state) const
{
  enum WrapperErrorCode returnCode = (*(wrapperSymbols_->finalizeSymbol_))( p_state, p_exchangedData_, error_.get() );
  if (returnCode != WRAPPER_OK)
    throw WrapperInternalException(HERE)
        << "Wrapper function '" << getFunctionName(FINALIZATION)
        << "' returned error message: " << wrapper_getErrorAsString(returnCode)
        << ". Reason: " << getWrapperError( error_.get() );
}




/* State managment */
void * WrapperObject::createNewState()
{
  void * p_state = 0;

  enum WrapperErrorCode returnCode = (*(wrapperSymbols_->stateCreationSymbol_))( & p_state, p_exchangedData_, error_.get() );
  if (returnCode != WRAPPER_OK)
    throw WrapperInternalException(HERE)
        << "Wrapper function '" << getFunctionName(STATE_CREATION)
        << "' returned error message: " << wrapper_getErrorAsString(returnCode)
        << ". Reason: " << getWrapperError( error_.get() );

  return p_state;
}


void WrapperObject::deleteState(void * p_state)
{
  enum WrapperErrorCode returnCode = (*(wrapperSymbols_->stateDeletionSymbol_))( p_state, p_exchangedData_, error_.get() );
  if (returnCode != WRAPPER_OK)
    throw WrapperInternalException(HERE)
        << "Wrapper function '" << getFunctionName(STATE_DELETION)
        << "' returned error message: " << wrapper_getErrorAsString(returnCode)
        << ". Reason: " << getWrapperError( error_.get() );

}



/* Symbol accessor */
LibrarySymbol WrapperObject::getSymbol(const String & name, Bool optional, LibrarySymbol alternateSymbol) const
{
  try
  {
    return handle_.getSymbol(name);

  }
  catch (DynamicLibraryException & ex)
  {
    if (optional)
    {
      LOGDEBUG(OSS() << "Library symbol '" << name << "' linked to an internal symbol");
      return alternateSymbol;
    }
    else
    {
      LOGWARN( ex.what() );
      throw WrapperInternalException(HERE) << ex;
    }
  }
}


END_NAMESPACE_OPENTURNS
