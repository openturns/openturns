/*                                               -*- C -*- */
/**
 * @brief This file provides macros to ease wrapper writing
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_WRAPPERMACROS_H
#define OPENTURNS_WRAPPERMACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "OTdebug.h"
#include "OTconfig.hxx"

#ifdef OPENTURNS_HAVE_PTHREAD_H
#include "OTthread.hxx"
#include <signal.h>
#define CHECK_MULTITHREAD
#else
#define CHECK_MULTITHREAD const char _Multithread_NOT_available_[-1];
#endif

#include "WrapperInterface.h"

/***************************************************************************************
 *                                                                                     *
 *                               Various macros                                        *
 *                                                                                     *
 ***************************************************************************************/

#ifdef __cplusplus
# define C_DECL extern "C"
#else
# define C_DECL extern
#endif


#ifdef DEBUG
#define dbg_printMessage( fname, msg )                        printMessage((fname), (msg))
#define dbg_printEntrance( fname )                            printEntrance((fname))
#define dbg_printExit( fname )                                printExit((fname))
#define dbg_printState( fname, state )                        printState((fname), (state))
#define dbg_printWrapperExchangedData( fname, exchangedData ) printWrapperExchangedData((fname), (exchangedData))
#define dbg_printWrapperInformation( fname, info )            printWrapperInformation((fname), (info))
#define dbg_printPoint( fname, point )                        printPoint((fname), (point))
#define dbg_printSample( fname, sample )                      printSample((fname), (sample))
#define dbg_printMatrix( fname, matrix )                      printMatrix((fname), (matrix))
#define dbg_printTensor( fname, tensor )                      printTensor((fname), (tensor))
#else
#define dbg_printMessage( fname, msg )
#define dbg_printEntrance( fname )
#define dbg_printExit( fname )
#define dbg_printState( fname, state )
#define dbg_printWrapperExchangedData( fname, exchangedData )
#define dbg_printWrapperInformation( fname, info )
#define dbg_printPoint( fname, point )
#define dbg_printSample( fname, sample )
#define dbg_printMatrix( fname, matrix )
#define dbg_printTensor( fname, tensor )
#endif

#if defined(__cplusplus)
#define CAST( type, arg ) reinterpret_cast< type >(arg)
#else
#define CAST( type, arg ) ((type) (arg))
#endif /* defined(__cplusplus) */


#define FIND_EXCHANGED_DATA_IN_INTERNAL_STATE( pointer ) FIND_EXCHANGED_DATA_IN_INTERNAL_STATE_DEF( pointer )
#define FIND_EXCHANGED_DATA_IN_INTERNAL_STATE_DEF( pointer ) CAST(struct WrapperExchangedData *, pointer)

#define SET_INFORMATION_FROM_EXCHANGED_DATA( p_exchangedData )          \
  do {                                                                  \
    p_info->inSize_  = getNumberOfVariables( (p_exchangedData), WRAPPER_IN); \
    p_info->outSize_ = getNumberOfVariables( (p_exchangedData), WRAPPER_OUT); \
  } while (0);

#define GET_EXCHANGED_DATA_FROM( pointer ) GET_EXCHANGED_DATA_FROM_DEF( pointer )
#define GET_EXCHANGED_DATA_FROM_DEF( pointer )                          \
  struct WrapperExchangedData * p_exchangedData = FIND_EXCHANGED_DATA_IN_INTERNAL_STATE( pointer );

#define COPY_EXCHANGED_DATA_TO( pointer ) COPY_EXCHANGED_DATA_TO_DEF( pointer )
#define COPY_EXCHANGED_DATA_TO_DEF( pointer )                           \
  copyWrapperExchangedData( CAST(struct WrapperExchangedData **, pointer) \
                            , p_exchangedData                           \
                            , p_error);

#define DELETE_EXCHANGED_DATA_FROM( pointer ) DELETE_EXCHANGED_DATA_FROM_DEF( pointer )
#define DELETE_EXCHANGED_DATA_FROM_DEF( pointer )                       \
  freeWrapperExchangedData( CAST(struct WrapperExchangedData *, pointer) );

#define CHECK_WRAPPER_MODE( mode ) if ( p_exchangedData->parameters_->mode_ != (mode) )  { \
    printMessage( FUNCTIONNAME, "Incorrrect mode for wrapper" );        \
    return WRAPPER_USAGE_ERROR;                                         \
  }
#define CHECK_WRAPPER_IN( mode )   if ( p_exchangedData->parameters_->in_   != (mode) )  { \
    printMessage( FUNCTIONNAME, "Incorrrect 'in' parameter for wrapper" ); \
    return WRAPPER_USAGE_ERROR;                                         \
  }
#define CHECK_WRAPPER_OUT( mode )  if ( p_exchangedData->parameters_->out_  != (mode) )  { \
    printMessage( FUNCTIONNAME, "Incorrrect 'out' parameter for wrapper" ); \
    return WRAPPER_USAGE_ERROR;                                         \
  }

#define PRINT(...) printUserMessage( FUNCTIONNAME, __VA_ARGS__ );
#define SETERROR(...) setError( p_error, __VA_ARGS__ );
#define MIN(a, b) ( ( (a) < (b) ) ? (a) : (b) )
#define MAX(a, b) ( ( (a) > (b) ) ? (a) : (b) )

#define DEF_STRUCT_ARGUMENTS                            \
  struct AdapterArguments {                             \
    void * p_state;                                     \
    struct WrapperExchangedData * p_exchangedData;      \
    void * p_error;                                     \
    const struct sample * inSample;                     \
    struct sample * outSample;                          \
    long begin;                                         \
    long end;                                           \
    long threadId;                                      \
    enum WrapperErrorCode rc;                           \
    volatile long * p_errorEncountered;                 \
  };



#define INPOINT_ARRAY  (inPoint->data_)
#define OUTPOINT_ARRAY (outPoint->data_)

#define INPOINT_SIZE  (inPoint->size_)
#define OUTPOINT_SIZE (outPoint->size_)

#define INPOINT_COORD( i )  (inPoint->data_[  (i) ])
#define OUTPOINT_COORD( i ) (outPoint->data_[ (i) ])

#define OUTMATRIX_ARRAY (outMatrix->data_)
#define OUTMATRIX_ROWS  (outMatrix->nbrows_)
#define OUTMATRIX_COLS  (outMatrix->nbcols_)
#define OUTMATRIX_COORD( i, j ) (outMatrix->data_[ (i) + (j) * outMatrix->nbrows_ ])

#define OUTTENSOR_ARRAY  (outTensor->data_)
#define OUTTENSOR_ROWS   (outTensor->nbrows_)
#define OUTTENSOR_COLS   (outTensor->nbcols_)
#define OUTTENSOR_SHEETS (outTensor->nbsheets_)
#define OUTTENSOR_COORD( i, j, k ) (outTensor->data_[ (i) + (j) * outMatrix->nbrows_ + (k) *  outMatrix->nbrows_ * outMatrix->nbcols_ ])

#define INSAMPLE_ARRAY  (inSample->data_)
#define OUTSAMPLE_ARRAY (outSample->data_)

#define INSAMPLE_SIZE  (inSample->size_)
#define OUTSAMPLE_SIZE (outSample->size_)

#define INSAMPLE_VALUES( i )  (&(inSample->data_[  (i) * inSample->size_  ]))
#define OUTSAMPLE_VALUES( i ) (&(outSample->data_[ (i) * outSample->size_ ]))

#define INFIELD_ARRAY  (inField->data_)
#define OUTFIELD_ARRAY (outField->data_)

#define INFIELD_TIME( i )  (inField->data_[  (i) * inField->size_  ])
#define OUTFIELD_TIME( i ) (outField->data_[ (i) * outField->size_ ])

#define INFIELD_VALUES( i )  (&(inField->data_[  (i) * inField->size_  + 1 ]))
#define OUTFIELD_VALUES( i ) (&(outField->data_[ (i) * outField->size_ + 1 ]))

#define INFIELD_SIZE  (inField->size_)
#define OUTFIELD_SIZE (outField->size_)

#define INFIELD_DIMENSION  (inField->dimension_)
#define OUTFIELD_DIMENSION (outField->dimension_)


/***************************************************************************************
 *                                                                                     *
 *                               Function macros                                       *
 *                                                                                     *
 ***************************************************************************************/

#define BIND_METHODS_FUNCTION               __wrapper_bind_methods
#define BIND_METHODS_FUNCTION_NAME         "__wrapper_bind_methods"
#define BIND_METHODS_FUNCTION_HIDDEN        __wrapper_bind_methods_hidden
#define BIND_METHODS_FUNCTION_HIDDEN_NAME  "__wrapper_bind_methods_hidden"

#define BIND_METHODS_FUNCTION_DEF                                       \
  C_DECL                                                                \
  RETURNCODE BIND_METHODS_FUNCTION ( METHODS methods[] )                \
  {                                                                     \
    RETURNCODE BIND_METHODS_FUNCTION_HIDDEN ( METHODS methods[] );      \
    return BIND_METHODS_FUNCTION_HIDDEN( methods );                     \
  }

#define WRAPPER_BEGIN                           \
  BIND_METHODS_FUNCTION_DEF

#define WRAPPER_END                             \
  /* Nothing to do */


#define FUNC_INFO( name , code )     FUNC_INFO_DEF( name, code )
#define FUNC_INFO_DEF( name , code )                                    \
  C_DECL                                                                \
  RETURNCODE func_getInfo_ ## name (STATE p_state,                      \
                                    INFORMATION p_info,                 \
                                    EXCHANGEDDATA p_exchangedData,      \
                                    ERROR p_error)                      \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "func_getInfo_" #name ;          \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printWrapperInformation(FUNCTIONNAME, p_info);                  \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define FUNC_CREATESTATE( name , code )     FUNC_CREATESTATE_DEF( name, code )
#define FUNC_CREATESTATE_DEF( name , code )                             \
  C_DECL                                                                \
  RETURNCODE func_createState_ ## name (NEW_STATE p_p_state,            \
                                        EXCHANGEDDATA p_exchangedData,  \
                                        ERROR p_error)                  \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "func_createState_" #name ;      \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printWrapperExchangedData(FUNCTIONNAME, p_exchangedData);       \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printState(FUNCTIONNAME, *p_p_state);                           \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define FUNC_DELETESTATE( name , code )     FUNC_DELETESTATE_DEF( name, code )
#define FUNC_DELETESTATE_DEF( name , code )                             \
  C_DECL                                                                \
  RETURNCODE func_deleteState_ ## name (STATE p_state,                  \
                                        EXCHANGEDDATA p_exchangedData,  \
                                        ERROR p_error)                  \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "func_deleteState_" #name ;      \
    dbg_printEntrance(FUNCTIONNAME);                                    \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printState(FUNCTIONNAME, p_state);                              \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define FUNC_INIT( name , code )     FUNC_INIT_DEF( name, code )
#define FUNC_INIT_DEF( name , code )                            \
  C_DECL                                                        \
  RETURNCODE func_init_ ## name (STATE p_state,                 \
                                 EXCHANGEDDATA p_exchangedData, \
                                 ERROR p_error)                 \
  {                                                             \
    const char FUNCTIONNAME[] UNUSED = "func_init_" #name ;     \
    dbg_printEntrance(FUNCTIONNAME);                            \
    dbg_printState(FUNCTIONNAME, p_state);                      \
                                                                \
    code ;                                                      \
                                                                \
    dbg_printExit(FUNCTIONNAME);                                \
                                                                \
    return WRAPPER_OK;                                          \
  }

/* If prefix is NULL (= 0) then get user prefix either in wrapper description file or in resource map */
#define FUNC_EXEC_BODY_CALLING_COMMAND_IN_TEMP_DIR( prefix ) FUNC_EXEC_BODY_CALLING_COMMAND_IN_TEMP_DIR_DEF( prefix )
#define FUNC_EXEC_BODY_CALLING_COMMAND_IN_TEMP_DIR_DEF( prefix )        \
  {                                                                     \
    long rc = 0;                                                        \
    char * temporaryDirectory = 0;                                      \
    char * cmd = 0;                                                     \
    char * currentWorkingDirectory = 0;                                 \
                                                                        \
    /* We save the current working directory for a future come back */  \
    currentWorkingDirectory = getCurrentWorkingDirectory( p_error );    \
    if (currentWorkingDirectory == NULL) return WRAPPER_EXECUTION_ERROR; \
                                                                        \
    /* We build a temporary directory in which we will work */          \
    temporaryDirectory = createTemporaryDirectory( (prefix) ? (prefix) : getUserPrefix(p_exchangedData), \
                                                   p_exchangedData, p_error ); \
    if (temporaryDirectory == NULL) return WRAPPER_EXECUTION_ERROR;     \
                                                                        \
    /* We create the input files for the external code in the temporary directory */ \
    if (createInputFiles(temporaryDirectory, p_exchangedData, inPoint, p_error )) \
      return WRAPPER_EXECUTION_ERROR;                                   \
                                                                        \
    /* The real computation is here */                                  \
    rc = runInsulatedCommand(temporaryDirectory, p_exchangedData, inPoint, p_error ); \
                                                                        \
    /* Read the output values */                                        \
    if ( rc == 0 )                                                      \
      if (readOutputFiles(temporaryDirectory, p_exchangedData, outPoint, p_error )) { \
        const char * msg = getError( p_error );                         \
        setError( p_error, "Can't read output files. Reason: %s", msg ); \
        free( (void *) msg );                                           \
        return WRAPPER_EXECUTION_ERROR;                                 \
      }                                                                 \
                                                                        \
    /* We kill the temporary directory */                               \
    deleteTemporaryDirectory(temporaryDirectory, rc, p_error );         \
                                                                        \
    free( currentWorkingDirectory );                                    \
                                                                        \
    if ( rc != 0 ) {                                                    \
      const char * msg = getError( p_error );                           \
      setError( p_error, "Code returned error %d. Reason: %s", rc, msg ); \
      free( (void *) msg );                                             \
      return WRAPPER_EXECUTION_ERROR;                                   \
    }                                                                   \
  }


#define FUNC_EXEC( name , code )     FUNC_EXEC_DEF( name, code )
#define FUNC_EXEC_DEF( name , code )                            \
  C_DECL                                                        \
  RETURNCODE func_exec_ ## name (STATE p_state,                 \
                                 INPOINT inPoint,               \
                                 OUTPOINT outPoint,             \
                                 EXCHANGEDDATA p_exchangedData, \
                                 ERROR p_error)                 \
  {                                                             \
    const char FUNCTIONNAME[] UNUSED = "func_exec_" #name ;     \
    dbg_printEntrance(FUNCTIONNAME);                            \
    dbg_printState(FUNCTIONNAME, p_state);                      \
    dbg_printPoint(FUNCTIONNAME, inPoint);                      \
                                                                \
    code ;                                                      \
                                                                \
    dbg_printPoint(FUNCTIONNAME, outPoint);                     \
    dbg_printExit(FUNCTIONNAME);                                \
                                                                \
    return WRAPPER_OK;                                          \
  }


#define FUNC_EXEC_SAMPLE( name , code )     FUNC_EXEC_SAMPLE_DEF( name, code )
#define FUNC_EXEC_SAMPLE_DEF( name , code )                             \
  C_DECL                                                                \
  RETURNCODE func_exec_sample_ ## name (STATE p_state,                  \
                                        INSAMPLE inSample,              \
                                        OUTSAMPLE outSample,            \
                                        EXCHANGEDDATA p_exchangedData,  \
                                        ERROR p_error)                  \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "func_exec_sample_" #name ;      \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
    dbg_printSample(FUNCTIONNAME, inSample);                            \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printSample(FUNCTIONNAME, outSample);                           \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }


#define FUNC_EXEC_FIELD( name , code )     FUNC_EXEC_FIELD_DEF( name, code )
#define FUNC_EXEC_FIELD_DEF( name , code )                         \
  C_DECL                                                                \
  RETURNCODE func_exec_field_ ## name (STATE p_state,              \
				       INFIELD inField,		   \
				       OUTFIELD outField,		\
				       EXCHANGEDDATA p_exchangedData,	\
				       ERROR p_error)			\
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "func_exec_field_" #name ;  \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
    dbg_printField(FUNCTIONNAME, inField);                    \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printField(FUNCTIONNAME, outField);                   \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }


#define FUNC_FINALIZE( name , code )     FUNC_FINALIZE_DEF( name, code )
#define FUNC_FINALIZE_DEF( name , code )                                \
  C_DECL                                                                \
  RETURNCODE func_finalize_ ## name (STATE p_state,                     \
                                     EXCHANGEDDATA p_exchangedData,     \
                                     ERROR p_error)                     \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "func_finalize_" #name ;         \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }





/***************************************************************************************
 *                                                                                     *
 *                               Gradient macros                                       *
 *                                                                                     *
 **************************************************************************************/


#define GRAD_INFO( name , code )     GRAD_INFO_DEF( name, code )
#define GRAD_INFO_DEF( name , code )                                    \
  C_DECL                                                                \
  RETURNCODE grad_getInfo_ ## name (STATE p_state,                      \
                                    INFORMATION p_info,                 \
                                    EXCHANGEDDATA p_exchangedData,      \
                                    ERROR p_error)                      \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "grad_getInfo_" #name ;          \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define GRAD_CREATESTATE( name , code )     GRAD_CREATESTATE_DEF( name, code )
#define GRAD_CREATESTATE_DEF( name , code )                             \
  C_DECL                                                                \
  RETURNCODE grad_createState_ ## name (NEW_STATE p_p_state,            \
                                        EXCHANGEDDATA p_exchangedData,  \
                                        ERROR p_error)                  \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "grad_createState_" #name ;      \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printWrapperExchangedData(FUNCTIONNAME, p_exchangedData);       \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printState(FUNCTIONNAME, *p_p_state);                           \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define GRAD_DELETESTATE( name , code )     GRAD_DELETESTATE_DEF( name, code )
#define GRAD_DELETESTATE_DEF( name , code )                             \
  C_DECL                                                                \
  RETURNCODE grad_deleteState_ ## name (STATE p_state,                  \
                                        EXCHANGEDDATA p_exchangedData,  \
                                        ERROR p_error)                  \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "grad_deleteState_" #name ;      \
    dbg_printEntrance(FUNCTIONNAME);                                    \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printState(FUNCTIONNAME, p_state);                              \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define GRAD_INIT( name , code )     GRAD_INIT_DEF( name, code )
#define GRAD_INIT_DEF( name , code )                            \
  C_DECL                                                        \
  RETURNCODE grad_init_ ## name (STATE p_state,                 \
                                 EXCHANGEDDATA p_exchangedData, \
                                 ERROR p_error)                 \
  {                                                             \
    const char FUNCTIONNAME[] UNUSED = "grad_init_" #name ;     \
    dbg_printEntrance(FUNCTIONNAME);                            \
    dbg_printState(FUNCTIONNAME, p_state);                      \
                                                                \
    code ;                                                      \
                                                                \
    dbg_printExit(FUNCTIONNAME);                                \
                                                                \
    return WRAPPER_OK;                                          \
  }

#define GRAD_EXEC( name , code )     GRAD_EXEC_DEF( name, code )
#define GRAD_EXEC_DEF( name , code )                            \
  C_DECL                                                        \
  RETURNCODE grad_exec_ ## name (STATE p_state,                 \
                                 INPOINT inPoint,               \
                                 OUTMATRIX outMatrix,           \
                                 EXCHANGEDDATA p_exchangedData, \
                                 ERROR p_error)                 \
  {                                                             \
    const char FUNCTIONNAME[] UNUSED = "grad_exec_" #name ;     \
    dbg_printEntrance(FUNCTIONNAME);                            \
    dbg_printState(FUNCTIONNAME, p_state);                      \
    dbg_printPoint(FUNCTIONNAME, inPoint);                      \
                                                                \
    code ;                                                      \
                                                                \
    dbg_printMatrix(FUNCTIONNAME, outMatrix);                   \
    dbg_printExit(FUNCTIONNAME);                                \
                                                                \
    return WRAPPER_OK;                                          \
  }

#define GRAD_FINALIZE( name , code )     GRAD_FINALIZE_DEF( name, code )
#define GRAD_FINALIZE_DEF( name , code )                                \
  C_DECL                                                                \
  RETURNCODE grad_finalize_ ## name (STATE p_state,                     \
                                     EXCHANGEDDATA p_exchangedData,     \
                                     ERROR p_error)                     \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "grad_finalize_" #name ;         \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }




/***************************************************************************************
 *                                                                                     *
 *                               Hessian macros                                        *
 *                                                                                     *
 **************************************************************************************/


#define HESS_INFO( name , code )     HESS_INFO_DEF( name, code )
#define HESS_INFO_DEF( name , code )                                    \
  C_DECL                                                                \
  RETURNCODE hess_getInfo_ ## name (STATE p_state,                      \
                                    INFORMATION p_info,                 \
                                    EXCHANGEDDATA p_exchangedData,      \
                                    ERROR p_error)                      \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "hess_getInfo_" #name ;          \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define HESS_CREATESTATE( name , code )     HESS_CREATESTATE_DEF( name, code )
#define HESS_CREATESTATE_DEF( name , code )                             \
  C_DECL                                                                \
  RETURNCODE hess_createState_ ## name (NEW_STATE p_p_state,            \
                                        EXCHANGEDDATA p_exchangedData,  \
                                        ERROR p_error)                  \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "hess_createState_" #name ;      \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printWrapperExchangedData(FUNCTIONNAME, p_exchangedData);       \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printState(FUNCTIONNAME, *p_p_state);                           \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define HESS_DELETESTATE( name , code )     HESS_DELETESTATE_DEF( name, code )
#define HESS_DELETESTATE_DEF( name , code )                             \
  C_DECL                                                                \
  RETURNCODE hess_deleteState_ ## name (STATE p_state,                  \
                                        EXCHANGEDDATA p_exchangedData,  \
                                        ERROR p_error)                  \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "hess_deleteState_" #name ;      \
    dbg_printEntrance(FUNCTIONNAME);                                    \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printState(FUNCTIONNAME, p_state);                              \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }

#define HESS_INIT( name , code )     HESS_INIT_DEF( name, code )
#define HESS_INIT_DEF( name , code )                            \
  C_DECL                                                        \
  RETURNCODE hess_init_ ## name (STATE p_state,                 \
                                 EXCHANGEDDATA p_exchangedData, \
                                 ERROR p_error)                 \
  {                                                             \
    const char FUNCTIONNAME[] UNUSED = "hess_init_" #name ;     \
    dbg_printEntrance(FUNCTIONNAME);                            \
    dbg_printState(FUNCTIONNAME, p_state);                      \
                                                                \
    code ;                                                      \
                                                                \
    dbg_printExit(FUNCTIONNAME);                                \
                                                                \
    return WRAPPER_OK;                                          \
  }

#define HESS_EXEC( name , code )     HESS_EXEC_DEF( name, code )
#define HESS_EXEC_DEF( name , code )                            \
  C_DECL                                                        \
  RETURNCODE hess_exec_ ## name (STATE p_state,                 \
                                 INPOINT inPoint,               \
                                 OUTTENSOR outTensor,           \
                                 EXCHANGEDDATA p_exchangedData, \
                                 ERROR p_error)                 \
  {                                                             \
    const char FUNCTIONNAME[] UNUSED = "hess_exec_" #name ;     \
    dbg_printEntrance(FUNCTIONNAME);                            \
    dbg_printState(FUNCTIONNAME, p_state);                      \
    dbg_printPoint(FUNCTIONNAME, inPoint);                      \
                                                                \
    code ;                                                      \
                                                                \
    dbg_printTensor(FUNCTIONNAME, outTensor);                   \
    dbg_printExit(FUNCTIONNAME);                                \
                                                                \
    return WRAPPER_OK;                                          \
  }

#define HESS_FINALIZE( name , code )     HESS_FINALIZE_DEF( name, code )
#define HESS_FINALIZE_DEF( name , code )                                \
  C_DECL                                                                \
  RETURNCODE hess_finalize_ ## name (STATE p_state,                     \
                                     EXCHANGEDDATA p_exchangedData,     \
                                     ERROR p_error)                     \
  {                                                                     \
    const char FUNCTIONNAME[] UNUSED = "hess_finalize_" #name ;         \
    dbg_printEntrance(FUNCTIONNAME);                                    \
    dbg_printState(FUNCTIONNAME, p_state);                              \
                                                                        \
    code ;                                                              \
                                                                        \
    dbg_printExit(FUNCTIONNAME);                                        \
                                                                        \
    return WRAPPER_OK;                                                  \
  }


#endif /* OPENTURNS_WRAPPERMACROS_H */
