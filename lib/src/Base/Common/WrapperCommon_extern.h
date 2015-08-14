//                                               -*- C -*-
/**
 * @brief This file provides objects and functions commonly used by wrappers
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

#ifndef OPENTURNS_WRAPPERCOMMON_PRIVATE_H
#define OPENTURNS_WRAPPERCOMMON_PRIVATE_H

#include <WrapperInterface.h>
#include "OTdebug.h"

BEGIN_C_DECLS

/** Return a string describing the list element type encountered */
extern OT_API const char * wrapper_getListElementTypeAsString(enum WrapperListElementType typeCode);

/** Return a string describing the list element type encountered */
extern OT_API const char * wrapper_getProvidedAsString(enum WrapperProvided provided);

/** Return a string describing the location encountered */
extern OT_API const char * wrapper_getLocationAsString(enum WrapperLocationType location);

/** Return a string describing the configuration state encountered */
extern OT_API const char * wrapper_getConfigurationStateAsString(enum WrapperConfigurationState state);

/** Return a string describing the configuration mode encountered */
extern OT_API const char * wrapper_getConfigurationModeAsString(enum WrapperConfigurationMode mode);

/** Return a string describing the data transfer mode encountered */
extern OT_API const char * wrapper_getDataTransferModeAsString(enum WrapperDataTransferMode mode);

/** Return a string describing the error encountered */
extern OT_API const char * wrapper_getErrorAsString(enum WrapperErrorCode errorCode);

/** Register an error message that will be later read by the platform */
extern OT_API void wrapper_setError(void * p_error, const char * format, ...);

/** Get the registered error message. The returned string need to be freed to avoid memory leaks */
extern OT_API const char * wrapper_getError(void * p_error);

/** Print out the user message passed as argument */
extern OT_API void wrapper_printUserMessage(const char * functionName, const char * format, ...);

/* Print out the info message passed as argument */
extern OT_API void wrapper_printInfoMessage(const char * functionName, const char * format, ...);

/** Print out the message passed as argument */
extern OT_API void wrapper_printMessage(const char * functionName, const char * format, ...);

/** Print out a message telling that we enter a function */
extern OT_API void wrapper_printEntrance(const char * functionName);

/** Print out a message telling that we leave a function */
extern OT_API void wrapper_printExit(const char * functionName);

/** Print out a message telling which state we are working on */
extern OT_API void wrapper_printState(const char * functionName, void * p_state);

/** Print out the content of the WrapperExchangedData structure */
extern OT_API void wrapper_printWrapperExchangedData(const char * functionName, const struct WrapperExchangedData * p_exchangedData);

/* Print out the content of the WrapperInformation structure */
extern OT_API void wrapper_printWrapperInformation(const char * functionName, const struct WrapperInformation * p_info);

/* Print out the content of a point structure */
extern OT_API void wrapper_printPoint(const char * functionName, const struct point * p_point);

/* Print out the content of a sample structure */
extern OT_API void wrapper_printSample(const char * functionName, const struct sample * p_sample);

/* Print out the content of a field structure */
extern OT_API void wrapper_printField(const char * functionName, const struct field * p_field);

/* Print out the content of a matrix structure */
extern OT_API void wrapper_printMatrix(const char * functionName, const struct matrix * p_matrix);

/* Print out the content of a tensor structure */
extern OT_API void wrapper_printTensor(const char * functionName, const struct tensor * p_tensor);






/* Get the number of variables of some type (in or out) in the data read from the description file */
extern OT_API unsigned long wrapper_getNumberOfVariables(const struct WrapperExchangedData * p_exchangedData, unsigned long type);

/* Get the number of files of some type (in or out) in the data read from the description file */
extern OT_API unsigned long wrapper_getNumberOfFiles(const struct WrapperExchangedData * p_exchangedData, unsigned long type);

/* Deep copy of a WrapperExchangedData structure (returns non-zero if error occured) */
extern OT_API long wrapper_copyWrapperExchangedData(struct WrapperExchangedData ** p_p_new_exchangedData, const struct WrapperExchangedData * p_exchangedData, void * p_error);

/* Free a WrapperExchangedData structure */
extern OT_API void wrapper_freeWrapperExchangedData(struct WrapperExchangedData * p_exchangedData);

/* Append a variable to a WrapperExchangedData structure. Return non zero on failure */
/* NOTE: the variable pointer is kept by the WrapperExchangedData structure, so don't free it */
extern OT_API long wrapper_appendVariableToWrapperExchangedData(struct WrapperExchangedData * p_exchangedData,
                                                         struct WrapperVariableListElement * p_variable,
                                                         void * p_error);

/* Get the command line from the exchanged data structure */
extern OT_API const char * wrapper_getCommand(const struct WrapperExchangedData * p_exchangedData);

/* Get the user prefix from the exchanged data structure */
extern OT_API const char * wrapper_getUserPrefix(const struct WrapperExchangedData * p_exchangedData);

/* DEPRECATED: Get the study id from the exchanged data structure */
extern OT_API long wrapper_getStudyId(const struct WrapperExchangedData * p_exchangedData);

/* DEPRECATED: Get the study case from the exchanged data structure */
extern OT_API const char * wrapper_getStudyCase(const struct WrapperExchangedData * p_exchangedData);

/* DEPRECATED: Get the component name from the exchanged data structure */
extern OT_API const char * wrapper_getComponentName(const struct WrapperExchangedData * p_exchangedData);

/* Get the number of virtual CPUs from the exchanged data structure */
extern OT_API unsigned long wrapper_getNumberOfCPUs(const struct WrapperExchangedData * p_exchangedData);

/* Get the timeout when waiting for output files to be read */
extern OT_API unsigned long wrapper_getOutputFileTimeout(const struct WrapperExchangedData * p_exchangedData);

/* Get the number of retries allowed for a failed command */
extern OT_API unsigned long wrapper_getRunCommandRetries(const struct WrapperExchangedData * p_exchangedData);

/* Get a pointer to a variable structure. NULL if not found */
extern OT_API const struct WrapperVariableListElement * wrapper_getVariableById(const struct WrapperExchangedData * p_exchangedData, const char * id);

/* Get a pointer to a file structure. NULL if not found */
extern OT_API const struct WrapperFileListElement * wrapper_getFileById(const struct WrapperExchangedData * p_exchangedData, const char * id);


/* Create a temporary directory. This function internaly allocates the storage of the return value.
 * The symmetric function deleteTemporaryDirectory should be used to avoid memory leak.
 */
extern OT_API char * wrapper_createTemporaryDirectory(const char * tempDirPrefix, const struct WrapperExchangedData * p_exchangedData, void * p_error);

/* Delete a temporary directory if executionStatus is zero (ie, no error).
 * This function internaly frees the storage allocated by
 * createTemporaryDirectory, so be careful not to do it yourself. The content of the
 * directory is erased.
 */
extern OT_API void wrapper_deleteTemporaryDirectory(char * tempDir, long executionStatus, void * p_error);

/* Return the name of the current directory in a string. Note that this storage is
 * internaly allocated by the function, so users MUST free it (using free()).
 */
extern OT_API char * wrapper_getCurrentWorkingDirectory(void * p_error);

/* Create the files listed in the exchanged data structure in the directory.
 * This action may result in variable substitution in the files according to the
 * regular expressions listed in the exchanged data structure. Returns non-zero if
 * some failure occured.
 */
extern OT_API long wrapper_createInputFiles(const char * directory, const struct WrapperExchangedData * p_exchangedData, const struct point * p_point, void * p_error);

/* Read the content of the output listed in the exchanged data structure located in
 * the directory. Returns non-zero if some failure occured. For any output variable
 * not found in output file, the corresponding coordinate in p_point is set to NaN.
 */
extern OT_API long wrapper_readOutputFiles(const char * directory, const struct WrapperExchangedData * p_exchangedData, struct point * p_point, void * p_error);

/* Make a command line from the template command and the variable list. The returned value is the new
 * command with variables substituted. This buffer is internaly allocated and MUST be deallocated
 * using free.
 */
extern OT_API char * wrapper_makeCommandFromTemplate(const char * command, const struct WrapperExchangedData * p_exchangedData, const struct point * p_point, void * p_error);

#ifndef WIN32
/* Create a batch script in the general temporary directory for use by insulateCommand() */
extern OT_API char * wrapper_makeScript(char * command, const char * directory, const struct WrapperExchangedData * p_exchangedData, void * p_error);
#endif

/* Insulate a shell command, so that it can be run in a random temporary directory */
extern OT_API char * wrapper_insulateCommand(char * command, const char * temporaryDir, const struct WrapperExchangedData * p_exchangedData, void * p_error);

/* Run an insulated command based on the template read from the wrapper description file */
extern OT_API long wrapper_runInsulatedCommand(const char * temporaryDir, const struct WrapperExchangedData * p_exchangedData, const struct point * p_point, void * p_error);

END_C_DECLS

#endif /* OPENTURNS_WRAPPERCOMMON_PRIVATE_H */
