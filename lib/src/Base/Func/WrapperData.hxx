//                                               -*- C++ -*-
/**
 *  @brief This class declares the wrapper data that are exchanged with the platform
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
#ifndef OPENTURNS_WRAPPERDATA_HXX
#define OPENTURNS_WRAPPERDATA_HXX

#include "OTprivate.hxx"
#include "Object.hxx"
#include "Pointer.hxx"           // for shared pointer
#include "WrapperInterface.h"
#include "Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @struct WrapperSymbolProvided
 * @brief  Stores information about the presence or no of symbols in the wrapper
 */
struct OT_API WrapperSymbolProvided
{
  static const WrapperProvided NO;  ///< No symbol is provided
  static const WrapperProvided YES; ///< The symbol is provided
};


/**
 * @struct WrapperComputedGradient
 * @brief  Stores information about the computation of gradient for a variable
 * @internal
 * @deprecated
 * This class is deprecated and should not be used anymore.
 */
struct OT_API WrapperComputedGradient
{
  static const WrapperProvided NO;  ///< No need to compute the gradient within the numerical function
  static const WrapperProvided YES; ///< We need to compute the gradient within the numerical function
};


/**
 * @struct WrapperFunctionDescription
 * @brief  Describes what is the interface of a numerical function
 */
struct OT_API WrapperFunctionDescription
    : public Object
{
  String name_;               ///< The name of the numerical function
  WrapperProvided provided_;  ///< Is the symbol present in the wrapper ?

  WrapperFunctionDescription();
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;
};


/**
 * @struct WrapperDataFileType
 * @brief  Stores the type (input or output) of the file
 */
struct OT_API WrapperDataFileType
{
  static const WrapperListElementType IN;  ///< The file is an input file
  static const WrapperListElementType OUT; ///< The file is an output file
};

/**
 * @struct WrapperDataFile
 * @brief  Holds the name and the path of any file passed as argument to the wrapper
 */
struct OT_API WrapperDataFile
    : public Object
{
  String id_;                   ///< The id of the file (any string to distinguish each file from another)
  String name_;                 ///< The name of the file (stdin, stdout, log, etc.)
  FileName path_;               ///< The path of the file (/tmp/stdin, /var/log/mylog, etc.)
  String subst_;                ///< The list of variable's ids to be substituted in the file
  WrapperListElementType type_; ///< The type of the file (input or output)

  WrapperDataFile();
  Bool operator == (const WrapperDataFile & other) const;
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;
}; /* struct WrapperDataFile */


/**
 * @struct WrapperDataVariableType
 * @brief  Stores the type (input or output) of the variable
 */
struct OT_API WrapperDataVariableType
{
  static const WrapperListElementType IN;       ///< The variable is an input variable
  static const WrapperListElementType OUT;      ///< The variable is an output variable
  static const WrapperListElementType INTERNAL; ///< The variable is internal to the platform (not user defined)
};

/**
 * @struct WrapperDataVariableLocation
 * @brief  Stores the line number or the regular expression that locates the variable in the file
 */
struct OT_API WrapperDataVariableLocation
{
  static const WrapperLocationType LINE;   ///< The location is a line number
  static const WrapperLocationType REGEXP; ///< The location is a regular expression
};

/**
 * @struct WrapperDataVariable
 * @brief  Holds information of any variable passed as argument to the wrapper
 */
struct OT_API WrapperDataVariable
    : public Object
{
  String id_;                    ///< The id of the variable (any string to distinguish each variable from another)
  String comment_;               ///< A comment to inform on the variable
  String unit_;                  ///< The unit which the variable is expressed in
  String regexp_;                ///< The regular expression used to find the variable location in files
  String format_;                ///< The format in which the variable must be printed in files
  WrapperListElementType type_;  ///< The type of the variable (input or output)
  WrapperProvided gradient_;     ///< The gradient of this variable is computed if true (@deprecated)
  WrapperLocationType fromType_; ///< The type of the from_ member
  String from_;                  ///< The location where substitution should start
  WrapperLocationType toType_;   ///< The type of the to_ member
  String to_;                    ///< The location where substitution should stop

  WrapperDataVariable();
  Bool operator == (const WrapperDataVariable & other) const;
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;
}; /* struct WrapperDataVariable */




/**
 * @struct WrapperState
 * @brief  Stores the mode of invocation of the external code
 */
struct OT_API WrapperState
{
  static const WrapperConfigurationState SHARED;   ///< The intenal state is shared amoung function, gradient and hessian
  static const WrapperConfigurationState SPECIFIC; ///< Function, gradient and hessian have specific internal state
};


/**
 * @struct WrapperMode
 * @brief  Stores the mode of invocation of the external code
 */
struct OT_API WrapperMode
{
  static const WrapperConfigurationMode STATICLINK;  ///< The external code is statically linked with the wrapper
  static const WrapperConfigurationMode DYNAMICLINK; ///< The external code is dynamically linked with the wrapper
  static const WrapperConfigurationMode FORK;        ///< the external code is a separate shell command
};


/**
 * @struct WrapperDataTransfer
 * @brief  Stores the mode of transmission for the arguments
 */
struct OT_API WrapperDataTransfer
{
  static const WrapperDataTransferMode FILES;     ///< The values are transmitted through files
  static const WrapperDataTransferMode PIPE;      ///< The values are transmitted through a pipe
  static const WrapperDataTransferMode ARGUMENTS; ///< The values are transmitted as command line arguments
  static const WrapperDataTransferMode SOCKET;    ///< The values are transmitted through a socket
  static const WrapperDataTransferMode CORBA;     ///< The values are transmitted through CORBA
};


/**
 * struct WrapperParameter
 * @brief Holds the configuration of the wrapper
 */
struct OT_API WrapperParameter
    : public Object
{
  WrapperConfigurationState state_; ///< The sharing mode of internal state
  WrapperConfigurationMode  mode_;  ///< The mode of invocation of the external code
  WrapperDataTransferMode   in_;    ///< The transmission mode for input arguments
  WrapperDataTransferMode   out_;   ///< The transmission mode for output arguments
  String command_;                  ///< The command that should invoque the external code according to mode
  String userPrefix_;               ///< The prefix that helps the user to find its compute dir

  WrapperParameter();
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;
}; /* struct WrapperParameter */



/**
 * @class WrapperData
 * @brief Declares the wrapper data that are exchanged with the platform
 */
class OT_API WrapperData
  : public Object
{
  CLASSNAME;
public:

  typedef       struct WrapperExchangedData        ExchangedData;
  typedef       struct WrapperExchangedData *      ExchangedDataPointer;
  typedef const struct WrapperExchangedData * ConstExchangedDataPointer;

  typedef Collection<WrapperDataFile>     FileListType;
  typedef Collection<WrapperDataVariable> VariableListType;

  /** Default constructor */
  WrapperData();

  /** Constructor from C structure */
  WrapperData(const struct WrapperExchangedData * p_exchangedData);

  /* String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Library path accessor
   * @param path The path of the dynamic library of the wrapper
   */
  void setLibraryPath(const FileName & path);
  /** Library path accessor
   * @return The path of the dynamic library of the wrapper
   */
  FileName getLibraryPath() const;

  /** Function description accessor
   * @param funcDescription The complete description of the numerical function inside the wrapper
   */
  void setFunctionDescription(const WrapperFunctionDescription & funcDescription);
  /** Function description accessor
   * @return The complete description of the numerical function inside the wrapper
   */
  WrapperFunctionDescription getFunctionDescription() const;

  /** Gradient description accessor
   * @param gradDescription The complete description of the numerical function gradient inside the wrapper
   */
  void setGradientDescription(const WrapperFunctionDescription & gradDescription);
  /** Gradient description accessor
   * @return The complete description of the numerical function gradient inside the wrapper
   */
  WrapperFunctionDescription getGradientDescription() const;

  /** Hessian description accessor
   * @param hessDescription The complete description of the numerical function hessian inside the wrapper
   */
  void setHessianDescription(const WrapperFunctionDescription & hessDescription);
  /** Hessian description accessor
   * @return The complete description of the numerical function hessian inside the wrapper
   */
  WrapperFunctionDescription getHessianDescription() const;

  /** Accessor to file list
   * @param fileList The collection of file description for the wrapper
   */
  void setFileList(const FileListType & fileList);
  /** Accessor to file list
   * @return The collection of file description for the wrapper
   */
  const FileListType & getFileList() const;

  /** Conversion method for C interface
   * @return A pointer to a newly allocated C structure that contains the same information as the file list
   * @see FreeFileListForCInterface
   */
  struct WrapperFileList * getNewFileListForCInterface() const;

  /** Frees the memory allocated by getNewFileListForCInterface() method
   * @param fileList A pointer to a newly allocated C structure that contains the same information as the file list
   * @see getNewFileListForCInterface
   */
  static void FreeFileListForCInterface(const struct WrapperFileList * fileList);

  /** Accessor to variable list
   * @param variableList The collection of variable description for the wrapper
   */
  void setVariableList(const VariableListType & variableList);
  /** Accessor to variable list
   * @return The collection of variable description for the wrapper
   */
  const VariableListType & getVariableList() const;

  /** Conversion method for C interface
   * @return A pointer to a newly allocated C structure that contains the same information as the variable list
   * @see FreeVariableListForCInterface
   */
  struct WrapperVariableList * getNewVariableListForCInterface() const;

  /** Frees the memory allocated by getNewVariableListForCInterface() method
   * @param fileList A pointer to a newly allocated C structure that contains the same information as the variable list
   * @see getNewVariableListForCInterface
   */
  static void FreeVariableListForCInterface(const struct WrapperVariableList * variableList);

  /** Accessor to wrapper parameters
   * @param parameters The data needed by the wrapper to link to the external code
   */
  void setParameters(const WrapperParameter & parameters);
  /** Accessor to wrapper parameters
   * @return The data needed by the wrapper to link to the external code
   */
  const WrapperParameter & getParameters() const;

  /** Conversion method for C interface
   * @return A pointer to a newly allocated C structure that contains the same information as the wrapper parameters
   * @see FreeParametersForCInterface
   */
  struct WrapperConfiguration * getNewParametersForCInterface() const;

  /** Frees the memory allocated by getNewParametersForCInterface() method
   * @param parameters A pointer to a newly allocated C structure that contains the same information as the wrapper parameters
   * @see getNewParametersForCInterface
   */
  static void FreeParametersForCInterface(const struct WrapperConfiguration * parameters);

  /** Builds a newly allocated WrapperExchangedData structure
   * @see FreeWrapperExchangedDataForCInterface
   */
  struct WrapperExchangedData * getNewWrapperExchangedDataForCInterface() const;

  /** Frees the memory allocated by getNewWrapperExchangedDataForCInterface() method
   * @param A pointer to a newly allocated WrapperExchangedData structure
   * @see getNewWrapperExchangedDataForCInterface
   */
  static void FreeWrapperExchangedDataForCInterface(const struct WrapperExchangedData * exchangedData);

  /** Check the correctness of the stored data */
  Bool isValid() const;

  /** Check if variables listed in subst_ are actually defined */
  static void CheckSubstitutedVariables(const WrapperDataFile & file, const VariableListType & variableList);

  /** Convert void pointer to exchanged data structure pointer */
  static      ExchangedDataPointer ConvertOpaquePointer(      void * ptr);
  static ConstExchangedDataPointer ConvertOpaquePointer(const void * ptr);

protected:

private:

  /** Where the library that hold the function is located */
  FileName libraryPath_;

  /** The description of the function to be bound to */
  WrapperFunctionDescription function_;

  /** The description of the gradient of the function */
  WrapperFunctionDescription gradient_;

  /** The description of the hessian of the function */
  WrapperFunctionDescription hessian_;

  /** A shared pointer on the list of files that are exchanged between the platform and the wrapper */
  Pointer<FileListType> p_fileList_;

  /** A shared pointer on the list of variables that are exchanged between the platform and the wrapper */
  Pointer<VariableListType> p_variableList_;

  /** A shared pointer on the parameters for the wrapper */
  Pointer<WrapperParameter> p_parameters_;

}; /* class WrapperData */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WRAPPERDATA_HXX */
