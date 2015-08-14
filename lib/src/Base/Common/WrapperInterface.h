//                                               -*- C -*-
/**
 * @brief This file provides an interface for external code C wrappers
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

#ifndef OPENTURNS_WRAPPERINTERFACE_H
#define OPENTURNS_WRAPPERINTERFACE_H

#include "OTdebug.h"
#include "OTconfig.hxx"

#ifdef OPENTURNS_HAVE_PTHREAD_H
#include "OTthread.hxx"
#include <signal.h>
#endif /* OPENTURNS_HAVE_PTHREAD_H */

BEGIN_C_DECLS

/**
 * Error codes and type that can be returned by the wrapper
 */
enum WrapperErrorCode {
  WRAPPER_OK = 0,
  WRAPPER_MEMORY_ERROR,
  WRAPPER_INITIALIZATION_ERROR,
  WRAPPER_EXECUTION_ERROR,
  WRAPPER_FINALIZATION_ERROR,
  WRAPPER_CANNOT_CREATE_STATE,
  WRAPPER_CANNOT_DELETE_STATE,
  WRAPPER_CANNOT_PROVIDE_INFORMATION,
  WRAPPER_INTERNAL_ERROR,
  WRAPPER_WRONG_ARGUMENT,
  WRAPPER_USAGE_ERROR,
  WRAPPER_NOT_IMPLEMENTED,
  WRAPPER_EXECUTION_ERROR_NO_RETRY,
  WRAPPER_BIND_ERROR,
  UNUSED_ERROR
};


/**
 * All those structures and definitions are designed for being exchanged
 * through the wrapper C API.
 */

/**
 * @struct WrapperInformation
 *
 * This structure is returned by the wrapper when the platform OpenTURNS
 * asks it to detail its internal function.
 * There is one WrapperInformation structure for each internal function.
 */
struct WrapperInformation {
  unsigned long inSize_;  /* size of the in  numerical point of the function */
  unsigned long outSize_; /* size of the out numerical point of the function */
};

/**
 * @struct point
 *
 * This structure holds the numerical point that is consumed or produced
 * by the internal function.
 * Remember to allocate or free the data_ pointer. The memory management
 * policy is : When you allocate memory, you have to deallocate it !
 * So if you pass a point structure for which you have allocated memory
 * to a function, the function should not free the memory when called,
 * so you MUST free it when the function returns. OK ?
 */
struct point {
  unsigned long size_; /* size of data vector */
  double * data_; /* data of the numerical point ordered as a vector */
};

/**
 * @struct sample
 *
 * This structure holds the numerical sample that is consumed or produced
 * by the internal function.
 * Remember to allocate or free the data_ pointer. The memory management
 * policy is : When you allocate memory, you have to deallocate it !
 * So if you pass a sample structure for which you have allocated memory
 * to a function, the function should not free the memory when called,
 * so you MUST free it when the function returns. OK ?
 */
struct sample {
  unsigned long size_;      /* size of data sample */
  unsigned long dimension_; /* dimension of data sample */
  double * data_;           /* data of the numerical sample ordered as a vector */
};


/**
 * @struct field
 *
 * This structure holds the field that is consumed or produced
 * by the internal function.
 * Remember to allocate or free the data_ pointer. The memory management
 * policy is : When you allocate memory, you have to deallocate it !
 * So if you pass a sample structure for which you have allocated memory
 * to a function, the function should not free the memory when called,
 * so you MUST free it when the function returns. OK ?
 */
struct field {
  unsigned long size_;      /* size of data sample */
  unsigned long dimension_; /* dimension of data sample. Important note: see Field.hxx */
  double * data_;           /* data of the field ordered as a vector */
};


/**
 * @struct matrix
 *
 * This structure holds the matrix that produced
 * by the internal gradient.
 * Remember to allocate or free the data_ pointer. The memory management
 * policy is : When you allocate memory, you have to deallocate it !
 * So if you pass a matrix structure for which you have allocated memory
 * to a function, the function should not free the memory when called,
 * so you MUST free it when the function returns. OK ?
 */
struct matrix {
  unsigned long nbrows_; /* number of rows into data vector */
  unsigned long nbcols_; /* number of columns into data vector */
  double * data_; /* data of the matrix ordered as a vector */
};


/**
 * @struct tensor
 *
 * This structure holds the tensor that produced
 * by the internal hessian.
 * Remember to allocate or free the data_ pointer. The memory management
 * policy is : When you allocate memory, you have to deallocate it !
 * So if you pass a tensor structure for which you have allocated memory
 * to a function, the function should not free the memory when called,
 * so you MUST free it when the function returns. OK ?
 */
struct tensor {
  unsigned long nbrows_; /* number of rows into data vector */
  unsigned long nbcols_; /* number of columns into data vector */
  unsigned long nbsheets_; /* number of sheets into data vector */
  double * data_; /* data of the tensor ordered as a vector */
};


/**
 * @enum WrapperListElementType
 *
 * This simple enumeration holds the type (input or output) of elements
 * used by the wrapper.
 */
enum WrapperListElementType { WRAPPER_IN=0, WRAPPER_OUT, WRAPPER_INTERNAL, UNUSED_LISTELEMENT };
enum WrapperProvided        { WRAPPER_NO=0, WRAPPER_YES, UNUSED_PROVIDED };
enum WrapperLocationType    { WRAPPER_LINE=0, WRAPPER_REGEXP, UNUSED_LOCATION };


/**
 * @ struct WrapperFileListElement
 *
 * This structure is a shell for all information about files
 * used by the wrapper.
 */
struct WrapperFileListElement {
  char * id_; /* The id of the file (for human reading) */
  char * name_; /* The name of the file (for human reading) */
  char * path_; /* The path of the file */
  char * subst_; /* The substitution list of the file */
  enum WrapperListElementType type_; /* The type (input or output) of the file */
};

/**
 * @struct WrapperFileList
 *
 * This is the link of a chained list. This list holds all the files
 * exchanged with the wrapper.
 */
struct WrapperFileList {
  struct WrapperFileList * next_; /* Points to the next element in list */
  struct WrapperFileListElement * file_; /* The file passed to the wrapper */
};




/**
 * @struct WrapperVariableListElement
 *
 * This structure is a shell for all information about variables
 * used by the wrapper.
 */
struct WrapperVariableListElement {
  char * id_;              /* The identifier of the variable */
  char * comment_;         /* The comment informs on the variable (for human reading) */
  char * unit_;            /* The unit which the variable is expressed in */
  char * regexp_;          /* The regular expression used to find the variable location in files */
  char * format_;          /* The format in which the variable must be printed in files */
  enum WrapperListElementType type_;     /* The type (input or output) of the variable */
  enum WrapperProvided gradient_; /* The gradient of this variable is computed if non-zero */
  enum WrapperLocationType fromType_; /* The type of information stored in from_ member */
  char * from_;            /* The location in file where substitution should start */
  enum WrapperLocationType toType_;   /* The type of information stored in to_ member */
  char * to_;              /* The location in file where substitution should stop */
};

/**
 * @struct WrapperVariableList
 *
 * This is the link of a chained list. This list holds all the variables
 * exchanged with the wrapper.
 */
struct WrapperVariableList {
  struct WrapperVariableList * next_; /* Points to the next element in list */
  struct WrapperVariableListElement * variable_; /* The variable passed to the wrapper */
};


enum WrapperConfigurationState { WRAPPER_SHAREDSTATE=0, WRAPPER_SPECIFICSTATE, UNUSED_CONFIGURATIONSTATE };
enum WrapperConfigurationMode  { WRAPPER_STATICLINK=0, WRAPPER_DYNAMICLINK, WRAPPER_FORK, UNUSED_CONFIGURATIONMODE };
enum WrapperDataTransferMode   { WRAPPER_FILES=0, WRAPPER_PIPE, WRAPPER_ARGUMENTS, WRAPPER_SOCKET, WRAPPER_CORBA, UNUSED_DATATRANSFERMODE };

/**
 * @struct WrapperConfiguration
 *
 * This structure contains general information about the wrapper
 */
struct WrapperConfiguration {
  enum WrapperConfigurationState state_; /* The mode of sharing of the internal state */
  enum WrapperConfigurationMode mode_;  /* The wrapping mode */
  enum WrapperDataTransferMode in_;    /* The input transfer mode */
  enum WrapperDataTransferMode out_;   /* The output transfer mode */
  char * command_;      /* The command invoqued by the wrapper to run the external code */
  char * userPrefix_;   /* The prefix that helps the user to find its compute dir  */
};


/**
 * @struct PlatformConfiguration
 *
 * This structure contains some information about the platform configuration
 * at the time the wrapper is loaded.
 */
struct PlatformConfiguration {
  char * generalTemporaryDirectory_; /* The main temporary directory in which wrappers can work */
  char * realRegexpShortcut_;        /* The shortcut for regular expression that match reals    */
  char * integerRegexpShortcut_;     /* The shortcut for regular expression that match integers */
  char * separatorRegexpShortcut_;   /* The shortcut for regular expression that match blanks   */
  unsigned long nbCpus_;             /* The number of virtual CPUs on the system                */
  unsigned long outputFilesTimeout_; /* The timeout when waiting for output files to be read    */
  long retries_;                     /* The number of retries for a failed command              */
  char * command_path_;              /* The path to the script file to run with multithreading  */
};


/**
 * @struct WrapperExchangedData
 *
 * This is the main structure that holds all the informations
 * that can be exchanged with the wrapper.
 */
struct WrapperExchangedData {
  struct WrapperFileList       * fileList_;     /* A chained list of files for the wrapper */
  struct WrapperVariableList   * variableList_; /* A chained list of variables for the wrapper */
  struct WrapperConfiguration  * parameters_;   /* The configuration of the wrapper */
  struct PlatformConfiguration * platform_;     /* The configuration of the platform */
  pthread_mutex_t              * p_lock_;       /* The mutex that locks this structure */
};



/**
 * This is the wrapper C API.
 * Every wrapper that intends to plug into the OpenTURNS platform should
 * provide those functions. They may do nothing if the wrapper does not
 * need it.
 *
 * Anyway, always remember that THE WRAPPER IS TO BE CALLED CONCURRENTLY
 * by the platform, so be careful with data sharing and global variable.
 * If not protected with critical sections, it is almost sure that something
 * will break one day or another.
 *
 * Because there is no internal synchronisation mechanism in C, it is UP TO YOU
 * to do the work.
 *
 * So keep cool. Do simple things. And if things come to be tough, have a break !
 * If at last, you always stands with global or static variables, shared data or
 * things leaving outside of your functions, then you have to enter the wonderful
 * but terrible world or parallelism. Do you have a talisman ?
 */


/**
 * StateCreationFunctionPointer : a function that creates a reserved memory space
 * StateDeletionFunctionPointer : a function that frees the reserved memory space
 * GetWrapperInformationFunctionPointer : a function that gives information about
 *   the wrapper internal function
 * InitializationFunctionPointer : a function that prepare the wrapper before the
 *   first call of the wrapper internal function
 * ExecutionFunctionPointer : a function that calls the internal wrapper function
 *   which is the purpose of the wrapper
 * GradientFunctionPointer : a function that calls the internal wrapper gradient
 *   which is the purpose of the wrapper
 * HessianFunctionPointer : a function that calls the internal wrapper hessian
 *   which is the purpose of the wrapper
 * FinalizationFunctionPointer : a function that clean everything up after the last
 *   call of the wrapper internal function
 */
typedef enum  WrapperErrorCode               RETURNCODE;
typedef       void                       (*  METHODS)();
typedef       void                        *  STATE;
typedef       void                        ** NEW_STATE;
typedef       void                        *  ERROR;
typedef const struct WrapperExchangedData *  EXCHANGEDDATA;
typedef       struct WrapperInformation   *  INFORMATION;
typedef const struct point                *  INPOINT;
typedef const struct sample               *  INSAMPLE;
typedef const struct field           *  INFIELD;
typedef       struct point                *  OUTPOINT;
typedef       struct sample               *  OUTSAMPLE;
typedef       struct field           *  OUTFIELD;
typedef       struct matrix               *  OUTMATRIX;
typedef       struct tensor               *  OUTTENSOR;


typedef RETURNCODE ( * BindMethodsFunctionPointer           )( METHODS[]                                                    );
typedef RETURNCODE ( * StateCreationFunctionPointer         )( NEW_STATE,                              EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * StateDeletionFunctionPointer         )(     STATE,                              EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * GetWrapperInformationFunctionPointer )(     STATE, INFORMATION,                 EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * InitializationFunctionPointer        )(     STATE,                              EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * ExecutionFunctionPointer             )(     STATE, INPOINT,      OUTPOINT,      EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * ExecutionSampleFunctionPointer       )(     STATE, INSAMPLE,     OUTSAMPLE,     EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * ExecutionFieldFunctionPointer   )(     STATE, INFIELD, OUTFIELD, EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * GradientFunctionPointer              )(     STATE, INPOINT,      OUTMATRIX,     EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * HessianFunctionPointer               )(     STATE, INPOINT,      OUTTENSOR,     EXCHANGEDDATA, ERROR );
typedef RETURNCODE ( * FinalizationFunctionPointer          )(     STATE,                              EXCHANGEDDATA, ERROR );

END_C_DECLS

/* For compatibility with old wrappers */
#include "WrapperInterface_V0.h"

#endif /* OPENTURNS_WRAPPERINTERFACE_H */
