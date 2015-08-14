//                                               -*- C++ -*-
/**
 * @brief This file provides objects and functions commonly used by wrappers
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
#if !defined(WIN32) // no strerror_r on mingw
#define _XOPEN_SOURCE 600 // for strerror_r
#endif

#include "OTwindows.h" // for GetTempFileName

#if defined(__APPLE__)
#define _DARWIN_C_SOURCE
#endif

#ifdef _MSC_VER
# include <direct.h>
# define mkdir(p)  _mkdir(p)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#ifndef _MSC_VER
#include <sys/time.h>  // for select(2)
#endif
#include <sys/types.h> // for select(2)
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>    // for select(2)
#ifndef _MSC_VER
#include <libgen.h>    // for dirname(3)
#endif
#include <cstring>
#include <fcntl.h>
#include <cerrno>
#include <cmath>
#include <cstdarg>
#include <cstdlib>     // for mkstemp(3)
#include <map>
#include <algorithm>   // for std::copy

#include "OTthread.hxx"
#include "OTtypes.hxx"
#include "Os.hxx"
#include "WrapperCommonFunctions.hxx"
#include "WrapperCommon_extern.h"
#include "ResourceMap.hxx"
#include "Log.hxx"
#include "TTY.hxx"
#include "Path.hxx"

using OT::ResourceMap;
using OT::Os;
using OT::Log;
using namespace OT;

#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
static const int ERRORBUF_LENGTH = 2048;
#endif



/* The following declarations MUST be coherent with enumeration declarations in WrapperInterface.h */
static const char * WrapperListElementTypeAsString[] =
{
  /* WRAPPER_IN         */ "in",
  /* WRAPPER_OUT        */ "out",
  /* WRAPPER_INTERNAL   */ "internal",
  /* UNUSED_LISTELEMENT */ "unused. Report bug"
};

static const char * WrapperProvidedAsString[] =
{
  /* WRAPPER_NO      */ "no",
  /* WRAPPER_YES     */ "yes",
  /* UNUSED_PROVIDED */ "unused. Report bug"
};

static const char * WrapperConfigurationStateAsString[] =
{
  /* WRAPPER_SHAREDSTATE       */ "shared",
  /* WRAPPER_SPECIFICSTATE     */ "specific",
  /* UNUSED_CONFIGURATIONSTATE */ "unused. Report bug"
};

static const char * WrapperConfigurationModeAsString[] =
{
  /* WRAPPER_STATICLINK       */ "static-link",
  /* WRAPPER_DYNAMICLINK      */ "dynamic-link",
  /* WRAPPER_FORK             */ "fork",
  /* UNUSED_CONFIGURATIONMODE */ "unused. Report bug"
};

static const char * WrapperDataTransferModeAsString[] =
{
  /* WRAPPER_FILES           */ "files",
  /* WRAPPER_PIPE            */ "pipe",
  /* WRAPPER_ARGUMENTS       */ "arguments",
  /* WRAPPER_SOCKET          */ "socket",
  /* WRAPPER_CORBA           */ "corba",
  /* UNUSED_DATATRANSFERMODE */ "unused. Report bug"
};

static const char * WrapperLocationAsString[] =
{
  /* WRAPPER_LINE    */ "line",
  /* WRAPPER_REGEXP  */ "regexp",
  /* UNUSED_LOCATION */ "unused. Report bug"
};

static const char * WrapperErrorAsString[] =
{
  /* WRAPPER_OK                         */ "Success",
  /* WRAPPER_MEMORY_ERROR               */ "Wrapper memory error. It may be an allocation failure or something related to memory storage",
  /* WRAPPER_INITIALIZATION_ERROR       */ "Wrapper initialization error. The wrapper failed to initialize",
  /* WRAPPER_EXECUTION_ERROR            */ "Wrapper execution error. The wrapper failed or the external code sent an error code to the wrapper during computation",
  /* WRAPPER_FINALIZATION_ERROR         */ "Wrapper finilization error. The wrapper failed to finalize",
  /* WRAPPER_CANNOT_CREATE_STATE        */ "Wrapper creation state error. The state can NOT be created or allocated",
  /* WRAPPER_CANNOT_DELETE_STATE        */ "Wrapper deletion state error. The state can NOT be freed",
  /* WRAPPER_CANNOT_PROVIDE_INFORMATION */ "Wrapper information error. The wrapper is unable to provide information to the calling function",
  /* WRAPPER_INTERNAL_ERROR             */ "Wrapper internal error. The wrapper failed in an internal function. Please send a bug report",
  /* WRAPPER_WRONG_ARGUMENT             */ "Wrapper wrong argument. The wrapper has received a argument it can not handle",
  /* WRAPPER_USAGE_ERROR                */ "Wrapper usage error. The wrapper has a configuration file not concording with its internals",
  /* WRAPPER_NOT_IMPLEMENTED            */ "Wrapper method not implemented. This may be due to a bug but the general case is to use unimplemented methods to provide optional features",
  /* WRAPPER_EXECUTION_ERROR_NO_RETRY   */ "Wrapper execution error. The wrapper failed or the external code sent an error code to the wrapper during computation",
  /* WRAPPER_BIND_ERROR                 */ "Wrapper bind error. The wrapper can't find the methods to bind to",
  /* UNUSED_ERROR                       */ "Unused error. You should never see this message. Please report bug to help us release better software"
};


BEGIN_C_DECLS


/* Return a string describing the list element type encountered */
extern const char * wrapper_getListElementTypeAsString(enum WrapperListElementType typeCode)
{
  if ( (typeCode < 0) || (typeCode >= UNUSED_LISTELEMENT) )
    return "Invalid list element type";
  return WrapperListElementTypeAsString[typeCode];
}

/* Return a string describing the list element type encountered */
extern const char * wrapper_getProvidedAsString(enum WrapperProvided provided)
{
  if ( (provided < 0) || (provided >= UNUSED_PROVIDED) )
    return "Invalid provided code";
  return WrapperProvidedAsString[provided];
}

/* Return a string describing the location encountered */
extern const char * wrapper_getLocationAsString(enum WrapperLocationType location)
{
  if ( (location < 0) || (location >= UNUSED_LOCATION) )
    return "Invalid location";
  return WrapperLocationAsString[location];
}

/* Return a string describing the configuration state encountered */
extern const char * wrapper_getConfigurationStateAsString(enum WrapperConfigurationState state)
{
  if ( (state < 0) || (state >= UNUSED_CONFIGURATIONSTATE) )
    return "Invalid configuration state";
  return WrapperConfigurationStateAsString[state];
}

/* Return a string describing the configuration mode encountered */
extern const char * wrapper_getConfigurationModeAsString(enum WrapperConfigurationMode mode)
{
  if ( (mode < 0) || (mode >= UNUSED_CONFIGURATIONMODE) )
    return "Invalid configuration mode";
  return WrapperConfigurationModeAsString[mode];
}

/* Return a string describing the data transfer mode encountered */
extern const char * wrapper_getDataTransferModeAsString(enum WrapperDataTransferMode mode)
{
  if ( (mode < 0) || (mode >= UNUSED_DATATRANSFERMODE) )
    return "Invalid data transfer mode";
  return WrapperDataTransferModeAsString[mode];
}


/* Return a string describing the error encountered */
extern const char * wrapper_getErrorAsString(enum WrapperErrorCode errorCode)
{
  if ( (errorCode < 0) || (errorCode >= UNUSED_ERROR) )
    return "Invalid return code delivered by the wrapper. Check your wrapper. If correct, please report bug to help us release better software";
  return WrapperErrorAsString[errorCode];
}





/* Register an error message that will be later read by the platform */
extern void wrapper_setError(void * p_error,
                             const char * format,
                             ...)
{
  va_list args;
  va_start( args, format );
  char * buf = vnewFormattedString( format, args );
  va_end( args );
  setWrapperError( p_error, buf );
  free( buf );
}




/* Get the registered error message. The returned string need to be freed to avoid memory leaks */
extern const char * wrapper_getError(void * p_error)
{
  return strdup( getWrapperError( p_error ).c_str() );
}





/* Print out the user message passed as argument */
extern void wrapper_printUserMessage(const char * functionName,
                                     const char * format,
                                     ...)
{
  if (Log::HasUser())
  {
    va_list args;
    va_start( args, format );
    char * message = vnewFormattedString( format, args );
    va_end( args );
    printToLogUser( "(%s) %s", functionName, message );
    free( message );
  }
}





/* Print out the info message passed as argument */
extern void wrapper_printInfoMessage(const char * functionName,
                                     const char * format,
                                     ...)
{
  if (Log::HasInfo())
  {
    va_list args;
    va_start( args, format );
    char * message = vnewFormattedString( format, args );
    va_end( args );
    printToLogInfo( "(%s) %s", functionName, message );
    free( message );
  }
}




/* Print out the message passed as argument */
extern void wrapper_printMessage(const char * functionName,
                                 const char * format,
                                 ...)
{
  if (Log::HasWrapper())
  {
    va_list args;
    va_start( args, format );
    char * message = vnewFormattedString( format, args );
    va_end( args );
    printToLogWrapper( "(%s) %s", functionName, message );
    free( message );
  }
}





/* Print out a message telling that we enter a function */
extern void wrapper_printEntrance(const char * functionName)
{
  if (Log::HasWrapper())
    printToLogWrapper( "(%s) Entering", functionName );
}





/* Print out a message telling that we leave a function */
extern void wrapper_printExit(const char * functionName)
{
  if (Log::HasWrapper())
    printToLogWrapper( "(%s) Exiting", functionName );
}




/* Print out a message telling which state we are working on */
extern void wrapper_printState(const char * functionName,
                               void * p_state)
{
  if (Log::HasDebug())
    printToLogDebug( "(%s) p_state=%p", functionName, p_state );
}




/* Print out the content of the WrapperExchangedData structure */
extern void wrapper_printWrapperExchangedData(const char * functionName,
    const struct WrapperExchangedData * p_exchangedData)
{
  if (Log::HasDebug())
  {
    printToLogDebug( "(%s) p_exchangedData=%p", functionName, p_exchangedData );

    if (  p_exchangedData )
    {
      printToLogDebug( "(%s) Exchanged data :", functionName );

      /* Print the file list */
      const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
      while (currentFileElement)
      {
        const struct WrapperFileList * nextElement = currentFileElement->next_;
        printToLogDebug( "(%s) File (id='%s', name='%s', path='%s', type='%s', subst='%s')",
                         functionName,
                         currentFileElement->file_->id_,
                         currentFileElement->file_->name_,
                         currentFileElement->file_->path_,
                         wrapper_getListElementTypeAsString( currentFileElement->file_->type_ ),
                         currentFileElement->file_->subst_ );
        currentFileElement = nextElement;
      }

      /* Print the variable list */
      const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
      while (currentVariableElement)
      {
        const struct WrapperVariableList * nextElement = currentVariableElement->next_;
        printToLogDebug( "(%s) Variable (id='%s', comment='%s', unit='%s', regexp='%s', format='%s', type='%s', gradient='%s', fromType='%s', from='%s', toType='%s', to='%s')",
                         functionName,
                         currentVariableElement->variable_->id_,
                         currentVariableElement->variable_->comment_,
                         currentVariableElement->variable_->unit_,
                         currentVariableElement->variable_->regexp_,
                         currentVariableElement->variable_->format_,
                         wrapper_getListElementTypeAsString( currentVariableElement->variable_->type_ ),
                         wrapper_getProvidedAsString( currentVariableElement->variable_->gradient_ ),
                         wrapper_getLocationAsString( currentVariableElement->variable_->fromType_ ),
                         currentVariableElement->variable_->from_,
                         wrapper_getLocationAsString( currentVariableElement->variable_->toType_ ),
                         currentVariableElement->variable_->to_ );
        currentVariableElement = nextElement;
      }

      /* Print the wrapper parameters */
      struct WrapperConfiguration * parameters = p_exchangedData->parameters_;
      printToLogDebug( "(%s) Parameters (mode='%s', state='%s', in='%s', out='%s', command='%s', user prefix='%s')",
                       functionName,
                       wrapper_getConfigurationModeAsString( parameters->mode_ ),
                       wrapper_getConfigurationStateAsString( parameters->state_ ),
                       wrapper_getDataTransferModeAsString( parameters->in_ ),
                       wrapper_getDataTransferModeAsString( parameters->out_ ),
                       parameters->command_,
                       parameters->userPrefix_ );

      /* Print the platform configuration */
      struct PlatformConfiguration * platform = p_exchangedData->platform_;
      printToLogDebug( "(%s) Platform configuration (temporary directory='%s', real regexp shortcut='%s', integer regexp shortcut='%s', separator regexp shortcut='%s', number of CPU=%d, output file timeout=%d, run command retries=%d, command_path=%s)",
                       functionName,
                       platform->generalTemporaryDirectory_,
                       platform->realRegexpShortcut_,
                       platform->integerRegexpShortcut_,
                       platform->separatorRegexpShortcut_,
                       platform->nbCpus_,
                       platform->outputFilesTimeout_,
                       platform->retries_,
                       (platform->command_path_ ? platform->command_path_ : "(nil)") );

    }
  }
}





/* Print out the content of the WrapperInformation structure */
extern void wrapper_printWrapperInformation(const char * functionName,
    const struct WrapperInformation * p_info)
{
  if (Log::HasDebug())
  {
    printToLogDebug( "(%s) p_info=%p", functionName, p_info );
    printToLogDebug( "(%s) Information (inSize=%d, outSize=%d)", functionName, p_info->inSize_, p_info->outSize_ );
  }
}




/* Print out the content of a point structure */
extern void wrapper_printPoint(const char * functionName,
                               const struct point * p_point)
{
  if (Log::HasDebug())
  {
    if (p_point != NULL)
    {
      char SZ[64];
      sprintf( SZ, "%lu", p_point->size_ );
      String msg;
      msg += "(";
      msg += functionName;
      msg += ") struct point {size=";
      msg += SZ;
      msg += ", data=[";

      char DATA[128];
      const char * sep = "";
      for (int i = 0; i < static_cast<int>(p_point->size_); ++i, sep = ",")
      {
        sprintf( DATA, "%s%.16g", sep, p_point->data_[i] );
        msg += DATA;
      }

      msg += "]}";
      printToLogDebug( msg.c_str() );
    }
  }
}






/* Print out the content of a sample structure */
extern void wrapper_printSample(const char * functionName,
                                const struct sample * p_sample)
{
  if (Log::HasDebug())
  {
    if (p_sample != NULL)
    {
      char SZ[64];
      sprintf( SZ, "%lu", p_sample->size_ );
      String msg;
      msg += "(";
      msg += functionName;
      msg += ") struct sample {size=";
      msg += SZ;
      msg += ", data=[";

      char DATA[128];
      const char * seps = "[";
      const char * sepp = "";
      for (int i = 0; i < static_cast<int>(p_sample->size_); ++i, seps = "],[")
      {
        msg += seps;
        sepp = "";
        const double * tab = &(p_sample->data_[i * p_sample->size_]);
        for (int j = 0; j < static_cast<int>(p_sample->dimension_); ++j, sepp = ",")
        {
          sprintf( DATA, "%s%.16g", sepp, tab[j] );
          msg += DATA;
        }
      }


      msg += "]}";
      printToLogDebug( msg.c_str() );
    }
  }
}





/* Print out the content of a field structure */
extern void wrapper_printField(const char * functionName,
                               const struct field * p_field)
{
  if (Log::HasDebug())
  {
    if (p_field != NULL)
    {
      char SZ[64];
      char DIM[64];
      sprintf( SZ,  "%lu", p_field->size_      );
      sprintf( DIM, "%lu", p_field->dimension_ );
      String msg;
      msg += "(";
      msg += functionName;
      msg += ") struct field {size=";
      msg += SZ;
      msg += ", dimension=";
      msg += DIM;
      msg += ", data=[";

      char DATA[128];
      const char * seps = "[";
      const char * sepp = "";
      for (int i = 0; i < static_cast<int>(p_field->size_); ++i, seps = "],[")
      {
        msg += seps;
        sepp = "";
        const double * tab = &(p_field->data_[i * p_field->size_]);
        for (int j = 0; j < static_cast<int>(p_field->dimension_ + 1); ++j, sepp = ",")
        {
          sprintf( DATA, "%s%.16g", sepp, tab[j] );
          msg += DATA;
        }
      }


      msg += "]}";
      printToLogDebug( msg.c_str() );
    }
  }
}





/* Print out the content of a matrix structure */
extern void wrapper_printMatrix(const char * functionName,
                                const struct matrix * p_matrix)
{
  if (Log::HasDebug())
  {
    if (p_matrix != NULL)
    {
      char ROWS[64];
      sprintf( ROWS, "%lu", p_matrix->nbrows_ );
      char COLS[64];
      sprintf( COLS, "%lu", p_matrix->nbcols_ );
      String msg;
      msg += "(";
      msg += functionName;
      msg += ") struct matrix {rows=";
      msg += ROWS;
      msg += ", cols=";
      msg += COLS;
      msg += ", data=[";

      char DATA[128];
      const char * sepr = "[";
      const char * sepc = "";
      for (int i = 0; i < static_cast<int>(p_matrix->nbrows_); ++i, sepr = "],[")
      {
        msg += sepr;
        sepc = "";
        for (int j = 0; j < static_cast<int>(p_matrix->nbcols_); ++j, sepc = ",")
        {
          sprintf( DATA, "%s%.16g", sepc, p_matrix->data_[i * p_matrix->nbcols_ + j] );
          msg += DATA;
        }
      }


      msg += "]}";
      printToLogDebug( msg.c_str() );
    }
  }
}




/* Print out the content of a tensor structure */
extern void wrapper_printTensor(const char * functionName,
                                const struct tensor * p_tensor)
{
  if (Log::HasDebug())
  {
    if (p_tensor != NULL)
    {
      char ROWS[64];
      sprintf( ROWS, "%lu", p_tensor->nbrows_ );
      char COLS[64];
      sprintf( COLS, "%lu", p_tensor->nbcols_ );
      char SHEETS[64];
      sprintf( SHEETS, "%lu", p_tensor->nbsheets_ );
      String msg;
      msg += "(";
      msg += functionName;
      msg += ") struct tensor {rows=";
      msg += ROWS;
      msg += ", cols=";
      msg += COLS;
      msg += ", shhets=";
      msg += SHEETS;
      msg += ", data=[";

      char DATA[128];
      const char * seps = "[";
      const char * sepr = "";
      const char * sepc = "";
      for (int s = 0; s < static_cast<int>(p_tensor->nbsheets_); ++s, seps = "],[")
      {
        msg += seps;
        sepr = "[";
        for (int i = 0; i < static_cast<int>(p_tensor->nbrows_); ++i, sepr = "],[")
        {
          msg += sepr;
          sepc = "";
          for (int j = 0; j < static_cast<int>(p_tensor->nbcols_); ++j, sepc = ",")
          {
            sprintf( DATA, "%s%.16g", sepc, p_tensor->data_[i * p_tensor->nbcols_ + j] );
            msg += DATA;
          }
        }
        msg += "]";
      }


      msg += "]}";
      printToLogDebug( msg.c_str() );
    }
  }
}





/* Deep copy of a WrapperExchangedData structure (returns non-zero if error occured) */
/* TODO : check memory leaks when an error occurs in the middle of this function */
extern long wrapper_copyWrapperExchangedData(struct WrapperExchangedData ** p_p_new_exchangedData,
    const struct WrapperExchangedData * p_exchangedData,
    void * p_error)
{
  if (!p_p_new_exchangedData)   /* no memory location to store data */
  {
    wrapper_setError( p_error, "No memory location to store exchanged data" );
    return 1;
  }
  *p_p_new_exchangedData = NULL;

  struct WrapperExchangedData * p_new_exchangedData = (struct WrapperExchangedData *) malloc(sizeof(struct WrapperExchangedData));
  if (!p_new_exchangedData)   /* exit if no memory available */
  {
    wrapper_setError( p_error, "No memory available" );
    return 1;
  }

  /* Definitions */
  const struct WrapperFileList       *  currentFileElement          = NULL;
  struct       WrapperFileList       ** p_newCurrentFileElement     = NULL;
  struct       WrapperFileList       *  newCurrentFileElement       = NULL;
  const struct WrapperVariableList   *  currentVariableElement      = NULL;
  struct       WrapperVariableList   ** p_newCurrentVariableElement = NULL;
  struct       WrapperVariableList   *  newCurrentVariableElement   = NULL;
  struct       WrapperConfiguration  *  parameters                  = NULL;
  struct       WrapperConfiguration  *  new_parameters              = NULL;
  struct       PlatformConfiguration *  platform                    = NULL;
  struct       PlatformConfiguration *  new_platform                = NULL;


  /* Copy the file list */
  p_new_exchangedData->fileList_ = NULL;
  currentFileElement      =   p_exchangedData->fileList_;
  p_newCurrentFileElement = & p_new_exchangedData->fileList_;
  while (currentFileElement)
  {
    newCurrentFileElement = (struct WrapperFileList *) malloc(sizeof(struct WrapperFileList));
    if (!newCurrentFileElement) goto ERR;

    newCurrentFileElement->next_ = NULL;
    *p_newCurrentFileElement     =   newCurrentFileElement;
    p_newCurrentFileElement      = & newCurrentFileElement->next_;

    newCurrentFileElement->file_ = (struct WrapperFileListElement *) malloc(sizeof(struct WrapperFileListElement));
    if (!newCurrentFileElement->file_) goto ERR;

    newCurrentFileElement->file_->id_ = strdup(currentFileElement->file_->id_);
    if (!newCurrentFileElement->file_->id_) goto ERR;

    newCurrentFileElement->file_->name_ = strdup(currentFileElement->file_->name_);
    if (!newCurrentFileElement->file_->name_) goto ERR;

    newCurrentFileElement->file_->path_ = strdup(currentFileElement->file_->path_);
    if (!newCurrentFileElement->file_->path_) goto ERR;

    newCurrentFileElement->file_->subst_ = strdup(currentFileElement->file_->subst_);
    if (!newCurrentFileElement->file_->subst_) goto ERR;

    newCurrentFileElement->file_->type_ = currentFileElement->file_->type_;

    currentFileElement = currentFileElement->next_;
  } /* end while (currentFileElement) */



  /* Copy the variable list */
  p_new_exchangedData->variableList_ = NULL;
  currentVariableElement      =   p_exchangedData->variableList_;
  p_newCurrentVariableElement = & p_new_exchangedData->variableList_;
  while (currentVariableElement)
  {
    newCurrentVariableElement = (struct WrapperVariableList *) malloc(sizeof(struct WrapperVariableList));
    if (!newCurrentVariableElement) goto ERR;

    newCurrentVariableElement->next_ = NULL;
    *p_newCurrentVariableElement     =   newCurrentVariableElement;
    p_newCurrentVariableElement      = & newCurrentVariableElement->next_;

    newCurrentVariableElement->variable_ = (struct WrapperVariableListElement *) malloc(sizeof(struct WrapperVariableListElement));
    if (!newCurrentVariableElement->variable_) goto ERR;

    newCurrentVariableElement->variable_->id_ = strdup(currentVariableElement->variable_->id_);
    if (!newCurrentVariableElement->variable_->id_) goto ERR;

    newCurrentVariableElement->variable_->comment_ = strdup(currentVariableElement->variable_->comment_);
    if (!newCurrentVariableElement->variable_->comment_) goto ERR;

    newCurrentVariableElement->variable_->unit_ = strdup(currentVariableElement->variable_->unit_);
    if (!newCurrentVariableElement->variable_->unit_) goto ERR;

    newCurrentVariableElement->variable_->regexp_ = strdup(currentVariableElement->variable_->regexp_);
    if (!newCurrentVariableElement->variable_->regexp_) goto ERR;

    newCurrentVariableElement->variable_->format_ = strdup(currentVariableElement->variable_->format_);
    if (!newCurrentVariableElement->variable_->format_) goto ERR;

    newCurrentVariableElement->variable_->type_ = currentVariableElement->variable_->type_;
    newCurrentVariableElement->variable_->gradient_ = currentVariableElement->variable_->gradient_;

    newCurrentVariableElement->variable_->fromType_ = currentVariableElement->variable_->fromType_;

    newCurrentVariableElement->variable_->from_ = strdup(currentVariableElement->variable_->from_);
    if (!newCurrentVariableElement->variable_->from_) goto ERR;

    newCurrentVariableElement->variable_->toType_ = currentVariableElement->variable_->toType_;

    newCurrentVariableElement->variable_->to_ = strdup(currentVariableElement->variable_->to_);
    if (!newCurrentVariableElement->variable_->to_) goto ERR;

    currentVariableElement = currentVariableElement->next_;
  } /* end while (currentVariableElement) */




  /* Copy the wrapper parameters */
  p_new_exchangedData->parameters_ = NULL;
  parameters     = p_exchangedData->parameters_;
  new_parameters = (struct WrapperConfiguration *) malloc(sizeof(struct WrapperConfiguration));
  if (!new_parameters) goto ERR;
  p_new_exchangedData->parameters_ = new_parameters;

  new_parameters->mode_  = parameters->mode_;
  new_parameters->state_ = parameters->state_;
  new_parameters->in_    = parameters->in_;
  new_parameters->out_   = parameters->out_;

  new_parameters->command_ = strdup(parameters->command_);
  if (!new_parameters->command_) goto ERR;

  new_parameters->userPrefix_ = strdup(parameters->userPrefix_);
  if (!new_parameters->userPrefix_) goto ERR;





  /* Copy the platform configuration */
  p_new_exchangedData->platform_ = NULL;
  platform = p_exchangedData->platform_;
  new_platform = (struct PlatformConfiguration *) malloc(sizeof(struct PlatformConfiguration));
  if (!new_platform) goto ERR;
  p_new_exchangedData->platform_ = new_platform;


  new_platform->generalTemporaryDirectory_ = strdup(platform->generalTemporaryDirectory_);
  if (!new_platform->generalTemporaryDirectory_) goto ERR;

  new_platform->realRegexpShortcut_ = strdup(platform->realRegexpShortcut_);
  if (!new_platform->realRegexpShortcut_) goto ERR;

  new_platform->integerRegexpShortcut_ = strdup(platform->integerRegexpShortcut_);
  if (!new_platform->integerRegexpShortcut_) goto ERR;

  new_platform->separatorRegexpShortcut_ = strdup(platform->separatorRegexpShortcut_);
  if (!new_platform->separatorRegexpShortcut_) goto ERR;

  new_platform->nbCpus_             = platform->nbCpus_;
  new_platform->outputFilesTimeout_ = platform->outputFilesTimeout_;
  new_platform->retries_            = platform->retries_;

  new_platform->command_path_ = (platform->command_path_ != NULL) ? strdup(platform->command_path_) : NULL;


  /* The new exchanged data uses the same lock */
  p_new_exchangedData->p_lock_ = p_exchangedData->p_lock_;

  /* At last we copy this structure into its destination location */
  *p_p_new_exchangedData = p_new_exchangedData;

  return 0;

ERR:
  wrapper_freeWrapperExchangedData( p_new_exchangedData );
  wrapper_setError( p_error, "No memory available" );
  return 1;
}





/* Free a WrapperExchangedData structure */
extern void wrapper_freeWrapperExchangedData(struct WrapperExchangedData * p_exchangedData)
{
  if (p_exchangedData)
  {

    /* Free the file list */
    struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
    while (currentFileElement)
    {
      struct WrapperFileList * nextElement = currentFileElement->next_;
      free(currentFileElement->file_->id_);
      free(currentFileElement->file_->name_);
      free(currentFileElement->file_->path_);
      free(currentFileElement->file_->subst_);
      free(currentFileElement->file_);
      free(currentFileElement);
      currentFileElement = nextElement;
    }

    /* Free the variable list */
    struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
    while (currentVariableElement)
    {
      struct WrapperVariableList * nextElement = currentVariableElement->next_;
      free(currentVariableElement->variable_->id_);
      free(currentVariableElement->variable_->comment_);
      free(currentVariableElement->variable_->unit_);
      free(currentVariableElement->variable_->regexp_);
      free(currentVariableElement->variable_->format_);
      free(currentVariableElement->variable_->from_);
      free(currentVariableElement->variable_->to_);
      free(currentVariableElement->variable_);
      free(currentVariableElement);
      currentVariableElement = nextElement;
    }

    /* Free the parameters */
    free(p_exchangedData->parameters_->command_);
    free(p_exchangedData->parameters_->userPrefix_);
    free(p_exchangedData->parameters_);

    /* Free the platform configuration */
    free(p_exchangedData->platform_->generalTemporaryDirectory_);
    free(p_exchangedData->platform_->realRegexpShortcut_);
    free(p_exchangedData->platform_->integerRegexpShortcut_);
    free(p_exchangedData->platform_->separatorRegexpShortcut_);
    free(p_exchangedData->platform_->command_path_);
    free(p_exchangedData->platform_);

    /* Free the embedding structure */
    free(p_exchangedData);
  }
}


/* Append a variable to a WrapperExchangedData structure. Return non zero on failure */
/* NOTE: the variable pointer is kept by the WrapperExchangedData structure, so don't free it */
extern long wrapper_appendVariableToWrapperExchangedData(struct WrapperExchangedData * p_exchangedData,
    struct WrapperVariableListElement * p_variable,
    void * p_error)
{
  if (p_exchangedData && p_variable)
  {
    struct WrapperVariableList * newCurrentVariableElement = (struct WrapperVariableList *) malloc(sizeof(struct WrapperVariableList));
    if (!newCurrentVariableElement)
    {
      wrapper_setError( p_error, "No memory available" );
      return 1;
    }

    newCurrentVariableElement->next_     = p_exchangedData->variableList_;
    newCurrentVariableElement->variable_ = p_variable;
    p_exchangedData->variableList_ = newCurrentVariableElement;
  }

  return 0;
}




/* Create a temporary directory. This function internaly allocates the storage of the return value.
 * The symmetric function deleteTemporaryDirectory should be used to avoid memory leak.
 */
extern char * wrapper_createTemporaryDirectory(const char * tempDirPrefix,
    const struct WrapperExchangedData * p_exchangedData,
    void * p_error)
{
  char * tempDirName = NULL;
  const char sfx[] = "_XXXXXX";
#ifdef WIN32
  int ret = 0;
#else
  char * name = NULL;
  ssize_t len = 0;
  int myerrno = 0;
#endif

  if ( (tempDirPrefix == 0) || (strlen(tempDirPrefix) == 0) )
  {
    wrapper_setError( p_error, "(createTemporaryDirectory) No prefix defined to create temporary directory" );
    goto ERR;
  }

  if (!p_exchangedData)
  {
    wrapper_setError( p_error, "(createTemporaryDirectory) No exchanged data to create temporary directory" );
    goto ERR;
  }

  if (createDirectory( p_exchangedData->platform_->generalTemporaryDirectory_, p_error ))
  {
    wrapper_setError( p_error, "(createTemporaryDirectory) Can't create directory %s", p_exchangedData->platform_->generalTemporaryDirectory_ );
    goto ERR;
  }

#ifndef WIN32

  len = 1 + strlen(p_exchangedData->platform_->generalTemporaryDirectory_) +
        strlen(Os::GetDirectorySeparator()) + strlen(tempDirPrefix) + strlen(sfx);
  tempDirName = static_cast<char *>( calloc( len, sizeof(char) ) );
  if (tempDirName == NULL)
  {
    wrapper_setError( p_error, "(createTemporaryDirectory) Can't build directory name" );
    goto ERR;
  }

  snprintf( tempDirName, len, "%s%s%s%s",
            p_exchangedData->platform_->generalTemporaryDirectory_,
            Os::GetDirectorySeparator(), tempDirPrefix, sfx );

  FSLOCK( name = mkdtemp(tempDirName); myerrno = errno; );
  if (name == NULL)
  {
#  if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
    char err[ERRORBUF_LENGTH];
    strerror_r( myerrno, err, ERRORBUF_LENGTH );
    wrapper_setError( p_error, "(createTemporaryDirectory) Can't create directory %s. Reason: %s", tempDirName, err );
#  else
    wrapper_setError( p_error, "(createTemporaryDirectory) Can't create directory %s", tempDirName );

#  endif
    free( tempDirName );
    goto ERR;
  }

  //IDM
  // LOGTRACE( String("IDM - Add dir  : ") + tempDirName );

  if ( checkDirectory( name, p_error ) ) goto ERR;

#else /* WIN32 */

  char temporaryDirName[MAX_PATH];
  // no GetTempPathName found on windows :-(
  // Possible race condition between calls to GetTempFileName and mkdir, hence this loop
  for (int retry = 10000; retry >= 0; --retry)
  {
    ret = GetTempFileName(p_exchangedData->platform_->generalTemporaryDirectory_, // directory for tmp files
                          TEXT((String(tempDirPrefix) + sfx).c_str()),        // temp file name prefix
                          0,                                                      // create unique name
                          temporaryDirName);                                      // buffer for name
    if (ret == 0)
    {
      ret = -1;
      continue;
    }
    DeleteFile(temporaryDirName);
    ret = mkdir(temporaryDirName);
    if (ret == 0) break;

    // There has been a name collision, retry
  }
  if (ret != 0)
  {
    wrapper_setError( p_error, "(createTemporaryDirectory) Can't create temporary directory" );
    goto ERR;
  }

  tempDirName = strdup(temporaryDirName);
  if (tempDirName == NULL)
  {
    wrapper_setError( p_error, "(createTemporaryDirectory) Can't build directory name" );
    goto ERR;
  }

#endif /* WIN32 */

  printToLogDebug( "(createTemporaryDirectory) temporary dir %s created", tempDirName );

  //OK:
  // LOGTRACE( String("IDM - Add dir  : ") + tempDirName );
  return tempDirName;

ERR:
  return NULL;
}






/* Delete a temporary directory if executionStatus is zero (ie, no error).
 * This function internaly frees the storage allocated by
 * createTemporaryDirectory, so be careful not to do it yourself. The content of the
 * directory is erased.
 */
extern void wrapper_deleteTemporaryDirectory(char * tempDir,
    long executionStatus,
    void * p_error)
{
  if (!executionStatus)
  {
    if (deleteDirectory( tempDir, p_error ))
    {
      String msg;
      msg += "(deleteTemporaryDirectory) Can NOT remove temporary directory '";
      msg += tempDir;
      msg += "'. Last error was: ";
      msg += getWrapperError( p_error );
      if (Log::HasUser())
        printToLogUser( "deleteTemporaryDirectory", msg.c_str() );
      wrapper_setError( p_error, msg.c_str() );
    }
    // LOGTRACE( String("IDM - Add dir  : ") + tempDir );


  }
  else
  {
    String msg;
    msg += "(deleteTemporaryDirectory) Execution error. Temporary directory '";
    msg += tempDir;
    msg += "' NOT removed. Check files and values. Last error was: ";
    msg += getWrapperError( p_error );
    if (Log::HasUser())
      printToLogUser( "deleteTemporaryDirectory", msg.c_str() );
    wrapper_setError( p_error, msg.c_str() );
  }
  free(tempDir);
}






/* Return the name of the current directory in a string. Note that this storage is
 * internaly allocated by the function, so users MUST free it (using free()).
 */
extern char * wrapper_getCurrentWorkingDirectory(void * p_error)
{
#ifdef _MSC_VER
  DWORD dwSize = MAX_PATH;
  char * path = (char*) calloc(dwSize, sizeof(char));
  if (path == NULL)
  {
    wrapper_setError( p_error, "(wrapper_getCurrentWorkingDirectory) Can't build directory name" );
    return NULL;
  }
  if ( GetCurrentDirectory(dwSize, path) > 0 )
    return path;

  delete path;
  wrapper_setError( p_error, "(getCurrentWorkingDirectory) Can NOT get current working directory" );
  return NULL;
#else
  long size = BUFFER_LENGTH;
  char * path = NULL;
  char * buf = NULL;
  int myerrno UNUSED;

  path = (char*) calloc(size, sizeof(char));
  FSLOCK( buf = getcwd(path, size); myerrno = errno; );
  while ( buf == NULL )
  {
    if (myerrno == ERANGE)
    {
      // The buffer is not big enough
      size *= 2;
      path = (char*) realloc(path, size * sizeof(char));
    }
    else
    {
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
      char err[ERRORBUF_LENGTH];
      strerror_r( myerrno, err, ERRORBUF_LENGTH );
      wrapper_setError( p_error, "(getCurrentWorkingDirectory) Can NOT get current working directory. Reason: %s", err );
#else
      wrapper_setError( p_error, "(getCurrentWorkingDirectory) Can NOT get current working directory" );
#endif
      break;
    }
  }

  return path;
#endif
}





/* Create the files listed in the exchanged data structure in the directory.
 * This action may result in variable substitution in the files according to the
 * regular expressions listed in the exchanged data structure. Returns non-zero if
 * some failure occured.
 */
extern long wrapper_createInputFiles(const char * directory,
                                     const struct WrapperExchangedData * p_exchangedData,
                                     const struct point * p_point,
                                     void * p_error)
{
  if ( checkDirectory( directory, p_error ) )
  {
    wrapper_setError( p_error, "(createInputFiles) Path '%s' is not a directory", directory );
    return 1;
  }

  /* For all the input files listed in the exchanged data structure, we substitute the actual values */
  const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
  while (currentFileElement)
  {
    if (currentFileElement->file_->type_ == WRAPPER_IN)
    {

      if (Log::HasWrapper())
        printToLogWrapper( "(createInputFiles) %sConsidering input file %s%s",
                           TTY::GetColor(TTY::BOLD),
                           currentFileElement->file_->id_,
                           TTY::GetColor(TTY::DEFAULT) );

      if (currentFileElement->file_->path_[0] == '/')
        if (Log::HasWrapper())
          printToLogWrapper( "(createInputFiles) Input file '%s' has absolute path. Prefer relative path", currentFileElement->file_->id_ );

      char * cwd = wrapper_getCurrentWorkingDirectory( p_error );
      if (cwd == NULL) return 1;

      String oldFile = cwd;
      oldFile += Os::GetDirectorySeparator();
      oldFile += currentFileElement->file_->path_;

      /* The new path for file */
      String destFile = directory;
      destFile += Os::GetDirectorySeparator();
      destFile += currentFileElement->file_->path_;

      free( cwd );

      // no optimization on windows : write every file
#ifndef WIN32
      if ( strcmp(currentFileElement->file_->subst_, "") )   /* File has variables to be substituted */
      {
#endif
        long sizeDiff = 0;
        struct stat file_stat;
        char * buf = NULL;
        FSLOCK( buf = readFile( oldFile.c_str(), &file_stat, p_error, wrapper_getOutputFileTimeout( p_exchangedData ) ) );
        if (buf == NULL) goto ERR;

        /* Perform all wanted substitutions in the file */
        buf = substituteVariables(buf, p_exchangedData, currentFileElement->file_->subst_, p_point, sizeDiff, p_error);

        /* We copy the file to the temporary directory */
        file_stat.st_size += sizeDiff;
        int rc = 0;
        FSLOCK( rc = writeFile( destFile.c_str(), buf, file_stat, p_error ) );

        /* Free storage after all */
        free(buf);

        if (rc != 0) return 1;

#ifndef WIN32
      }
      else     /* File has no variables to substitute */
      {

        /* Create all upper directories */
        int myerrno UNUSED;
        char * path = strdup( destFile.c_str() );
        char * parent = dirname( path );
        long rc = createDirectory( parent, p_error );
        free( path );
        if (rc !=  0)
        {
          char msg[BUFFER_LENGTH];
          const char * err = wrapper_getError( p_error );
          snprintf( msg, BUFFER_LENGTH, "(createInputFiles) Can NOT create directory %s. Reason: %s",
                    path, err );
          free( const_cast<char *>(err) );
          setWrapperError( p_error, msg );
          return 1;
        }

        FSLOCK( rc = symlink( oldFile.c_str(), destFile.c_str() ); myerrno = errno; );
        if (rc < 0)
        {
          String msg;
          msg += "(createInputFiles) Can NOT create symbolic link '";
          msg += destFile;
          msg += "' to '";
          msg += currentFileElement->file_->path_;
          msg += "'";
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
          char buf[ERRORBUF_LENGTH];
          strerror_r( myerrno, buf, ERRORBUF_LENGTH );
          msg += ". Reason: ";
          msg += buf;
#endif
          wrapper_setError( p_error, msg.c_str() );
          return 1;
        } // else LOGTRACE( String("IDM - Add file : " + destFile ) );

      } /* end if */
#endif /* WIN32 */
    }
    currentFileElement = currentFileElement->next_;
  }

  /* OK */
  return 0;

ERR:
  return 1;
}





/* Read the content of the output listed in the exchanged data structure located in
 * the directory. Returns non-zero if some failure occured. For any output variable
 * not found in output file, the corresponding coordinate in p_point is set to NaN.
 */
extern long wrapper_readOutputFiles(const char * directory,
                                    const struct WrapperExchangedData * p_exchangedData,
                                    struct point * p_point,
                                    void * p_error)
{
  if ( checkDirectory( directory, p_error) )
  {
    const char * err = wrapper_getError( p_error );
    wrapper_setError( p_error, "(readOutputFiles) Path '%s' is not a directory. Reason: %s", directory, err );
    free( const_cast<char *>( err ) );
    return 1;
  }

  /* For all the input files listed in the exchanged data structure, we substitute the actual values */
  const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
  while (currentFileElement)
  {
    if (currentFileElement->file_->type_ == WRAPPER_OUT)
    {

      if (Log::HasWrapper())
        printToLogWrapper( "(readOutputFiles) %sConsidering output file %s%s",
                           TTY::GetColor(TTY::BOLD),
                           currentFileElement->file_->id_,
                           TTY::GetColor(TTY::DEFAULT) );

      if (currentFileElement->file_->path_[0] == '/')
        if (Log::HasWrapper())
          printToLogWrapper( "(readOutputFiles) Output file '%s' has absolute path. Prefer relative path", currentFileElement->file_->id_ );

      struct stat file_stat;

      /* We append the name of the file to the temporary directory */
      String destFile = directory;
      destFile += Os::GetDirectorySeparator();
      destFile += currentFileElement->file_->path_;

      char * buf = NULL;
      FSLOCK( buf = readFile( destFile.c_str(), &file_stat, p_error, wrapper_getOutputFileTimeout( p_exchangedData ) ) );
      if (buf == NULL) goto ERR;

      /* Pick up data from the file */
      if ( strcmp(currentFileElement->file_->subst_, "") ) /* File has variables to be retrieved */
        /* long rc = */ retrieveVariables(buf, p_exchangedData, currentFileElement->file_->subst_, p_point, p_error);
      else if (Log::HasUser())
      {
        String msg;
        msg += "Output file '";
        msg += currentFileElement->file_->path_;
        msg += "' has no variable to substitute. Consider <subst> tag if you want substitution to happen.";
        printToLogUser( "readOutputFiles", msg.c_str() );
      }



      /* We copy the file to the base directory */
      //writeFile(currentFileElement->file_->path_, buf, file_stat, p_error);

      /* Free storage after all */
      free(buf);
    }
    currentFileElement = currentFileElement->next_;
  }

  /* OK */
  return 0;

ERR:
  return 1;
}





/* Make a command line from the template command and the variable list. The returned value is the new
 * command with variables substituted. This buffer is internaly allocated and MUST be deallocated
 * using free.
 */
extern char * wrapper_makeCommandFromTemplate(const char * command,
    const struct WrapperExchangedData * p_exchangedData,
    const struct point * p_point,
    void * p_error)
{
  char * cmd = (char *) strdup(command);
  if (cmd == NULL) return NULL;

  long sizeDiff = 0;
  cmd = substituteVariablesOnCommandLine(cmd, p_exchangedData, 0, p_point, sizeDiff, p_error);
  if (cmd == NULL) return NULL;

  /* OK */
  return cmd;
}





#ifndef WIN32
/* Create a batch script in the general temporary directory for use by insulateCommand() */
extern char * wrapper_makeScript(char * command,
                                 const char * directory,
                                 const struct WrapperExchangedData * p_exchangedData,
                                 void * p_error)
{
  int rc = 0;
  int fd = 0;
  int myerrno UNUSED;
  char * path = newFormattedString( "%s%sopenturns_batch", directory, Os::GetDirectorySeparator() );
  if (path == NULL)
  {
    setWrapperError( p_error, "(makeScript) Memory error" );
    return NULL;
  }

  FSLOCK( fd = open( path, O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC | O_WRONLY, S_IRWXU ); myerrno = errno; );
  if (fd < 0)
  {
    String msg;
    msg += "(makeScript) Failed to create script in directory ";
    msg += directory;
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
    char buf[ERRORBUF_LENGTH];
    strerror_r( myerrno, buf, ERRORBUF_LENGTH );
    msg += ". Reason: ";
    msg += buf;
#endif
    setWrapperError( p_error, msg );
    free( path );
    return NULL;
  }

  //IDM
  // LOGTRACE( String("IDM - Add file : ") + name );

  //     int rc = fcntl( fd, F_SETFL, O_DIRECT ); // Direct I/O writing
  //     if (rc == -1) {
  //       OSS oss;
  //       oss << "Failed to insulate command in directory " << temporaryDir;
  // #if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
  //       char buf[ERRORBUF_LENGTH];
  //       strerror_r( rc, buf, ERRORBUF_LENGTH );
  //       oss << ". Reason: " << buf;
  // #endif
  //       setWrapperError( p_error, oss);
  //       free( name );
  //       return NULL;
  //     }

  char * script = newFormattedString( "#!/bin/bash\n\n"
                                      "# This file was automatically generated by OpenTURNS\n\n"
                                      "set -v\n"
                                      "cd $OPENTURNS_EXECUTION_DIR\n"
                                      "%s\n"
                                      "rc=$?\n"
                                      "exit $rc\n",
                                      command );
  ssize_t written = 0;
  size_t len = strlen( script );
  while (static_cast<size_t>(written) < len)
  {
    ssize_t bytes = 0;
    FSLOCK( bytes = write( fd, script + written, len - written ); myerrno = errno; );
    if (bytes == -1)
    {
      if ( (myerrno != EAGAIN) && (myerrno != EINTR) )
      {
        String msg;
        msg += "(makeScript) Failed to write script in file ";
        msg += path;
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
        char buf[ERRORBUF_LENGTH];
        strerror_r( myerrno, buf, ERRORBUF_LENGTH );
        msg += ". Reason: ";
        msg += buf;
#endif
        setWrapperError( p_error, msg );
        free( script );
        free( path );
        return NULL;
      }
      continue;
    }
    written += bytes;
  }
  FSLOCK( fcntl( fd, F_SETFD, FD_CLOEXEC ); fsync( fd ); );

  //     struct stat file_stat;
  //     FSLOCK( rc = fstat( fd, &file_stat ); myerrno = errno; );
  //     if (rc == 0) {
  //       FSLOCK( rc = fchmod( fd, file_stat.st_mode | S_IRWXU ); myerrno = errno; );
  //       if (rc == -1) {
  //      String msg;
  //      msg += "(insulateCommand) Failed to change mode for file ";
  //      msg += name;
  // #if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
  //  char buf[ERRORBUF_LENGTH];
  //  strerror_r( myerrno, buf, ERRORBUF_LENGTH );
  //  msg += ". Reason: ";
  //  msg += buf;
  // #endif
  //  setWrapperError( p_error, msg );
  //  free( name );
  //  name = NULL;
  //  goto OUT;
  //       }
  //     }


  free( script );

  /* We close the file */
  FSLOCK( rc = close( fd ); myerrno = errno; );
  while ( rc == -1)
  {
    if ( (myerrno == EINTR) || (myerrno == EIO) ) continue;
    else
    {
      char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
      char buf[ERRORBUF_LENGTH];
      strerror_r( myerrno, buf, ERRORBUF_LENGTH );
      snprintf( msg, BUFFER_LENGTH, "(insulateCommand) Can NOT close file %s. Reason: %s", path, buf );
#else
      snprintf( msg, BUFFER_LENGTH, "(insulateCommand) Can NOT close file %s", path );
#endif
      setWrapperError( p_error, msg );
      free( path );
      return NULL;
    }
  }

  return path;
}
#endif /* WIN32 */





/* Insulate a shell command, so that it can be run in a random temporary directory */
extern char * wrapper_insulateCommand(char * command,
                                      const char * temporaryDir,
                                      const struct WrapperExchangedData * p_exchangedData,
                                      void * p_error)
{

#ifndef WIN32

  int rc = 0;
  char * name = newFormattedString( "%s%sopenturns_batch", temporaryDir, Os::GetDirectorySeparator() );
  if (name == NULL)
  {
    setWrapperError( p_error, "(insulateCommand) Memory error" );
    return NULL;
  }
  char * path = NULL;
  const char * dir = NULL;

  // Note: p_exchangedData->platform_->command_path_ has a special meaning when used in different contexts
  //    1. When insulateCommand() is called in single threaded environment, p_exchangedData->platform_->command_path_
  //       MUST be NULL, so the script is created in the local temporary directory of the computation.
  //    2. When insulateCommand() is called in multithreaded environment, p_exchangedData->platform_->command_path_
  //       MUST be allocated to the empty string (""), so strlen() is ZERO and the script is created in the
  //       general temporary directory, and a link is created to this script in the local temporary directory.
  //       This link is a hard link because the local and the general temporary directory reside in the same filesystem.

  lock( p_exchangedData );
  if (p_exchangedData->platform_->command_path_ == NULL)   // Need to create the batch file in local temporary directory
  {
    dir = temporaryDir;
    path = wrapper_makeScript( command, dir, p_exchangedData, p_error );
    if (path == NULL) rc = -1;

  }
  else if (strlen(p_exchangedData->platform_->command_path_) == 0)     // Need to create the first batch file for multithreading
  {
    dir = p_exchangedData->platform_->generalTemporaryDirectory_;
    path = wrapper_makeScript( command, dir, p_exchangedData, p_error );
    if (path != NULL)
    {
      free( p_exchangedData->platform_->command_path_ );
      p_exchangedData->platform_->command_path_ = strdup( path );
    }
    else rc = -1;
  }
  else     // Just link to the shared batch file
  {
    dir = p_exchangedData->platform_->generalTemporaryDirectory_;
    path = strdup( p_exchangedData->platform_->command_path_ );
    if (path == NULL) rc = -1;
  }
  unlock( p_exchangedData );

  if (rc < 0)
  {
    String msg;
    msg += "(insulateCommand) Can NOT create script";
    wrapper_setError( p_error, msg.c_str() );
    free( name );
    return NULL;
  }

  // Now we create a link to the script in the local temporary directory
  // This prevents the filesystem from being too stressed, in which case execve() fails randomly
  int myerrno UNUSED;
  if (dir != temporaryDir)
  {
    //FSLOCK( rc = symlink( path, name ); myerrno = errno; );
    FSLOCK( rc = link( path, name ); myerrno = errno; );
    if (rc < 0)
    {
      String msg;
      msg += "(insulateCommand) Can NOT create link '";
      msg += name;
      msg += "' to '";
      msg += path;
      msg += "'";
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
      char buf[ERRORBUF_LENGTH];
      strerror_r( myerrno, buf, ERRORBUF_LENGTH );
      msg += ". Reason: ";
      msg += buf;
#endif
      wrapper_setError( p_error, msg.c_str() );
      free( name );
      free( path );
      return NULL;
    }
  }

  free( path );

#else /* WIN32 */

  char * currentWorkingDirectory = wrapper_getCurrentWorkingDirectory( p_error );

  // antislash filename
  FileName antislashCurrentWorkingDirectory = currentWorkingDirectory;
  Path::AntislashFileName(antislashCurrentWorkingDirectory);

  FileName antislashTemporaryDir = temporaryDir;
  Path::AntislashFileName(antislashTemporaryDir);

  /* add current dir to PATH, cd to temp dir, launch command */
  char * name = newFormattedString( "set PATH=%%PATH%%;%s& cd /D %s& %s",
                                    antislashCurrentWorkingDirectory.c_str(),
                                    antislashTemporaryDir.c_str(),
                                    command );
  free ( currentWorkingDirectory );

  if (name == NULL)
    setWrapperError( p_error, "(insulateCommand) Memory error" );

#endif /* WIN32 */

  return name;
}




/* Get the number of variables of some type (in or out) in the data read from the description file */
extern unsigned long wrapper_getNumberOfVariables(const struct WrapperExchangedData * p_exchangedData,
    unsigned long type)
{
  return _getNumberOfVariables( p_exchangedData, type );
}



/* Get the number of files of some type (in or out) in the data read from the description file */
extern unsigned long wrapper_getNumberOfFiles(const struct WrapperExchangedData * p_exchangedData,
    unsigned long type)
{
  return _getNumberOfFiles( p_exchangedData, type );
}




/* Get the command line from the exchanged data structure */
extern const char * wrapper_getCommand(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  assert(p_exchangedData->parameters_);
  return p_exchangedData->parameters_->command_;
}




/* Get the user prefix from the exchanged data structure */
extern const char * wrapper_getUserPrefix(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  assert(p_exchangedData->parameters_);
  return p_exchangedData->parameters_->userPrefix_;
}




/* Get the study id from the exchanged data structure */
extern long wrapper_getStudyId(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  return 0;
}




/* Get the study case from the exchanged data structure */
extern const char * wrapper_getStudyCase(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  return "";
}




/* Get the component name from the exchanged data structure */
extern const char * wrapper_getComponentName(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  return "";
}




/* Get the number of virtual CPUs from the exchanged data structure */
extern unsigned long wrapper_getNumberOfCPUs(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  assert(p_exchangedData->platform_);
  return p_exchangedData->platform_->nbCpus_;
}





/* Get the timeout when waiting for output files to be read */
extern unsigned long wrapper_getOutputFileTimeout(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  assert(p_exchangedData->platform_);
  return p_exchangedData->platform_->outputFilesTimeout_;
}




/* Get the number of retries allowed for a failed command */
extern unsigned long wrapper_getRunCommandRetries(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  assert(p_exchangedData->platform_);
  return p_exchangedData->platform_->retries_;
}




/* Get a pointer to a variable structure. NULL if not found */
extern const struct WrapperVariableListElement * wrapper_getVariableById(const struct WrapperExchangedData * p_exchangedData,
    const char * id)
{
  assert(p_exchangedData);
  if (id == NULL) return NULL;
  return _getVariableById( p_exchangedData, id );
}




/* Get a pointer to a file structure. NULL if not found */
extern const struct WrapperFileListElement * wrapper_getFileById(const struct WrapperExchangedData * p_exchangedData,
    const char * id)
{
  assert(p_exchangedData);
  if (id == NULL) return NULL;
  return _getFileById( p_exchangedData, id );
}




/* Run an insulated command based on the template read from the wrapper description file
 *
 * unix call flow:
 *   runInsulatedCommand        (WrapperCommon, entry point to compute a numericalpoint)
 *   |- makeCommandFromTemplate (WrapperCommon, substitute variable in real command string)
 *   |- insulateCommand         (WrapperCommon, substitude real command to a bash script,
 *   |  |                        on threaded computing the bash script is created only once,
 *   |  |                        other bash script are (sym)linked to the first one)
 *   |  `- makeScript           (WrapperCommon, create a bash script that cd to temp dir and start the real command)
 *   `- runCommand              (WrapperCommonFunctions, set envar, fork and launch the bash script
 *
 * windows call flow:
 *   runInsulatedCommand        (WrapperCommon, entry point to compute a numericalpoint)
 *   |- makeCommandFromTemplate (WrapperCommon, substitute variable in real command string)
 *   |- insulateCommand         (WrapperCommon, prefix real command by a cd to the temp dir
 *   `- system                  (stdlib, launch insulated command)
 */
extern long wrapper_runInsulatedCommand(const char * temporaryDir,
                                        const struct WrapperExchangedData * p_exchangedData,
                                        const struct point * p_point,
                                        void * p_error)
{
  if (!p_exchangedData)
  {
    wrapper_setError( p_error, "(runInsulatedCommand) No exchanged data to insulate run command" );
    return 1;
  }

  int retries;
  long rc = 0;

  char * cmd1 = wrapper_makeCommandFromTemplate(wrapper_getCommand(p_exchangedData), p_exchangedData, p_point, p_error);
  char * cmd2 = NULL;

#ifdef SLOW_FILESYSTEM
  LOCK;
#endif /* SLOW_FILESYSTEM */

  cmd2 = wrapper_insulateCommand(cmd1, temporaryDir, p_exchangedData, p_error);
  free( cmd1 );
  cmd1 = NULL;

  if (!cmd2)
  {
    const char * err = wrapper_getError( p_error );
    char * msg = newFormattedString( "(runInsulatedCommand) Insulation of command failed. Last error was: %s", err );
    setWrapperError( p_error, msg );
    free( const_cast<char *>(err) );
    free( msg );
    goto ERR;
  }

#ifdef SLOW_FILESYSTEM
  UNLOCK;
#  ifndef WIN32
  /* wait that cmd2 (the bash script) is present on the filesystem */
  retries = MAX_ERRORS;
  rc = -1;
  {
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = ResourceMap::GetAsUnsignedInteger("slow-filesystem-wait-time");
    FSLOCK( rc = access( cmd2, R_OK ); myerrno = errno; );
    while ( (retries-- > 0) && (rc == -1) )
    {
      // Wait for 5 ms to ensure that the filesystem has been updated. Note: slow-filesystem-wait-time = 5000 usec
      select(0, NULL, NULL, NULL, &tv);
      tv.tv_usec *= 2;
      FSLOCK( rc = access( cmd2, R_OK ); myerrno = errno; );
    }
  }
  if (rc == -1)
  {
    char * msg = NULL;
#    if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
    char buf[ERRORBUF_LENGTH];
    strerror_r( myerrno, buf, ERRORBUF_LENGTH );
    msg = newFormattedString( "Execution of command '%d' impossible. Reason: %s", cmd2, buf );
#    else
    msg = newFormattedString( "Execution of command '%d' impossible", cmd2 );
#    endif
    setWrapperError( p_error, msg );
    free( msg );
    free( cmd2 );
    goto ERR;
  }
#  endif /* WIN32 */
#endif /* SLOW_FILESYSTEM */

  //IDM
  // LOGTRACE( String("IDM - ??? file  : ") + cmd2 );


  if (Log::HasDebug())
  {
    char * msg = newFormattedString( "Running shell command '%s'", cmd2 );
    printToLogDebug( "(runInsulatedCommand) %s", msg );
    free( msg );
  }

  rc = 0;
  retries = wrapper_getRunCommandRetries( p_exchangedData );
  {
    long waitfor = ResourceMap::GetAsUnsignedInteger("slow-filesystem-error-recovery");
#ifndef WIN32
    const long MICROSECONDS = 1000 * 1000;
    struct timeval tv;
    tv.tv_sec  = waitfor / MICROSECONDS;
    tv.tv_usec = waitfor % MICROSECONDS;
    while ( (retries-- > 0) && ((rc = runCommand( cmd2, temporaryDir, p_exchangedData, p_point, p_error)) == 33) )
    {
      if (Log::HasWrapper())
      {
        char * msg = newFormattedString( "Command %s failed to execute (execve failure). Try #%d out of %d",
                                         cmd2,
                                         wrapper_getRunCommandRetries( p_exchangedData ) - retries,
                                         wrapper_getRunCommandRetries( p_exchangedData ) );
        printToLogWrapper( "(runInsulatedCommand) %s", msg );
        free( msg );
      }
      // Wait for 1s to ensure that the filesystem has been updated.
      select(0, NULL, NULL, NULL, &tv);
      waitfor *= 2;
      tv.tv_sec  = waitfor / MICROSECONDS;
      tv.tv_usec = waitfor % MICROSECONDS;
    }
#else /* WIN32 */
    while ( (retries-- > 0) && ((rc = system( cmd2 )) != 0) )
    {
      rc = rc & 0xff; // WEXITSTATUS(rc)
      if (Log::HasWrapper())
      {
        char * msg = newFormattedString( "Command %s failed to execute (execve failure). Try #%d out of %d",
                                         cmd2,
                                         wrapper_getRunCommandRetries( p_exchangedData ) - retries,
                                         wrapper_getRunCommandRetries( p_exchangedData ) );
        printToLogWrapper( "(runInsulatedCommand) %s", msg );
        free( msg );
      }
      // Wait for 1s to ensure that the filesystem has been updated.
      Sleep(waitfor * 1000);
      waitfor *= 2;
    }
#endif /* WIN32 */
  }

  if (rc != 0)
  {
    char * msg = NULL;
    msg = newFormattedString( "(runInsulatedCommand) Command returned code rc=%d", rc );
    setWrapperError( p_error, msg );
    free( msg );
  }

  if (Log::HasDebug())
  {
    char * msg = newFormattedString( "Command returned code rc=%d", rc );
    printToLogDebug( "(runInsulatedCommand) %s", msg );
    free( msg );
  }

  free(cmd2);
  return rc;

ERR:
  return 1;
}



END_C_DECLS
