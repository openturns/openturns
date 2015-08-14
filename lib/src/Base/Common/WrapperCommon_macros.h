//                                               -*- C -*-
/**
 * @brief This file provides macro definitions to ease exporting function definitions
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

#ifndef OPENTURNS_WRAPPERCOMMON_MACROS_H
#define OPENTURNS_WRAPPERCOMMON_MACROS_H

/* WARNING: This file MUST be coherent with WrapperCommon.h */

/* In order to use this macro, you have to define BIND_ACTION in your own file
 * Ex: counting how many files are defined
 *     int count = 0;
 *     #undef BIND_ACTION
 *     #define BIND_ACTION(name) ++count;
 *     BIND_METHODS;
 */
#define BIND_METHODS                                                    \
  BIND_ACTION(const char *, getListElementTypeAsString, (enum WrapperListElementType typeCode)) \
  BIND_ACTION(const char *, getProvidedAsString, (enum WrapperProvided provided)) \
  BIND_ACTION(const char *, getLocationAsString, (enum WrapperLocationType location)) \
  BIND_ACTION(const char *, getConfigurationStateAsString, (enum WrapperConfigurationState state)) \
  BIND_ACTION(const char *, getConfigurationModeAsString, (enum WrapperConfigurationMode mode)) \
  BIND_ACTION(const char *, getDataTransferModeAsString, (enum WrapperDataTransferMode mode)) \
  BIND_ACTION(const char *, getErrorAsString, (enum WrapperErrorCode errorCode)) \
  BIND_ACTION(void, setError, (void * p_error, const char * format, ...)) \
  BIND_ACTION(const char *, getError, (void * p_error))                 \
  BIND_ACTION(void, printUserMessage, (const char * functionName, const char * format, ...)) \
  BIND_ACTION(void, printInfoMessage, (const char * functionName, const char * format, ...)) \
  BIND_ACTION(void, printMessage, (const char * functionName, const char * format, ...)) \
  BIND_ACTION(void, printEntrance, (const char * functionName))         \
  BIND_ACTION(void, printExit, (const char * functionName))             \
  BIND_ACTION(void, printState, (const char * functionName, void * p_state)) \
  BIND_ACTION(void, printWrapperExchangedData, (const char * functionName, const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(void, printWrapperInformation, (const char * functionName, const struct WrapperInformation * p_info)) \
  BIND_ACTION(void, printPoint, (const char * functionName, const struct point * p_point)) \
  BIND_ACTION(void, printSample, (const char * functionName, const struct sample * p_sample)) \
  BIND_ACTION(void, printField, (const char * functionName, const struct field * p_field)) \
  BIND_ACTION(void, printMatrix, (const char * functionName, const struct matrix * p_matrix)) \
  BIND_ACTION(void, printTensor, (const char * functionName, const struct tensor * p_tensor)) \
  BIND_ACTION(unsigned long, getNumberOfVariables, (const struct WrapperExchangedData * p_exchangedData, unsigned long type)) \
  BIND_ACTION(unsigned long, getNumberOfFiles, (const struct WrapperExchangedData * p_exchangedData, unsigned long type)) \
  BIND_ACTION(long, copyWrapperExchangedData, (struct WrapperExchangedData ** p_p_new_exchangedData, const struct WrapperExchangedData * p_exchangedData, void * p_error)) \
  BIND_ACTION(void, freeWrapperExchangedData, (struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(long, appendVariableToWrapperExchangedData, (struct WrapperExchangedData * p_exchangedData, struct WrapperVariableListElement * p_variable, void * p_error)) \
  BIND_ACTION(const char *, getCommand, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(const char *, getUserPrefix, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(long, getStudyId, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(const char *, getStudyCase, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(const char *, getComponentName, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(unsigned long, getNumberOfCPUs, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(unsigned long, getOutputFileTimeout, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(unsigned long, getRunCommandRetries, (const struct WrapperExchangedData * p_exchangedData)) \
  BIND_ACTION(const struct WrapperVariableListElement *, getVariableById, (const struct WrapperExchangedData * p_exchangedData, const char * id)) \
  BIND_ACTION(const struct WrapperFileListElement *, getFileById, (const struct WrapperExchangedData * p_exchangedData, const char * id)) \
  BIND_ACTION(char *, createTemporaryDirectory, (const char * tempDirPrefix, const struct WrapperExchangedData * p_exchangedData, void * p_error)) \
  BIND_ACTION(void, deleteTemporaryDirectory, (char * tempDir, long executionStatus, void * p_error)) \
  BIND_ACTION(char *, getCurrentWorkingDirectory, (void * p_error))     \
  BIND_ACTION(long, createInputFiles, (const char * directory, const struct WrapperExchangedData * p_exchangedData, const struct point * p_point, void * p_error)) \
  BIND_ACTION(long, readOutputFiles, (const char * directory, const struct WrapperExchangedData * p_exchangedData, struct point * p_point, void * p_error)) \
  BIND_ACTION(char *, makeCommandFromTemplate, (const char * command, const struct WrapperExchangedData * p_exchangedData, const struct point * p_point, void * p_error)) \
  BIND_ACTION(char *, insulateCommand, (char * command, const char * temporaryDir, const struct WrapperExchangedData * p_exchangedData, void * p_error)) \
  BIND_ACTION(long, runInsulatedCommand, (const char * temporaryDir, const struct WrapperExchangedData * p_exchangedData, const struct point * p_point, void * p_error)) \
  BIND_ACTION_EXTRA

#ifndef WIN32
#define BIND_ACTION_EXTRA                                               \
  BIND_ACTION(char *, makeScript, (char * command, const char * directory, const struct WrapperExchangedData * p_exchangedData, void * p_error))
#else
#define BIND_ACTION_EXTRA
#endif


#endif /* OPENTURNS_WRAPPERCOMMON_MACROS_H */
