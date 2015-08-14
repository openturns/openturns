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
#include <iostream>               // for std::ostream
#include <cstring>                // for std::strncpy, std::strdup
#include <algorithm>              // for std::copy
#include "OSS.hxx"
#include "Log.hxx"
#include "WrapperCommon_extern.h"
#include "WrapperData.hxx"
#include "ResourceMap.hxx"
#include "WrapperCommonFunctions.hxx"

BEGIN_NAMESPACE_OPENTURNS


const WrapperProvided           WrapperSymbolProvided::NO           = WRAPPER_NO;
const WrapperProvided           WrapperSymbolProvided::YES          = WRAPPER_YES;
const WrapperProvided           WrapperComputedGradient::NO         = WRAPPER_NO;
const WrapperProvided           WrapperComputedGradient::YES        = WRAPPER_YES;
const WrapperListElementType    WrapperDataFileType::IN             = WRAPPER_IN;
const WrapperListElementType    WrapperDataFileType::OUT            = WRAPPER_OUT;
const WrapperListElementType    WrapperDataVariableType::IN         = WRAPPER_IN;
const WrapperListElementType    WrapperDataVariableType::OUT        = WRAPPER_OUT;
const WrapperListElementType    WrapperDataVariableType::INTERNAL   = WRAPPER_INTERNAL;
const WrapperLocationType       WrapperDataVariableLocation::LINE   = WRAPPER_LINE;
const WrapperLocationType       WrapperDataVariableLocation::REGEXP = WRAPPER_REGEXP;
const WrapperConfigurationState WrapperState::SHARED                = WRAPPER_SHAREDSTATE;
const WrapperConfigurationState WrapperState::SPECIFIC              = WRAPPER_SPECIFICSTATE;
const WrapperConfigurationMode  WrapperMode::STATICLINK             = WRAPPER_STATICLINK;
const WrapperConfigurationMode  WrapperMode::DYNAMICLINK            = WRAPPER_DYNAMICLINK;
const WrapperConfigurationMode  WrapperMode::FORK                   = WRAPPER_FORK;
const WrapperDataTransferMode   WrapperDataTransfer::FILES          = WRAPPER_FILES;
const WrapperDataTransferMode   WrapperDataTransfer::PIPE           = WRAPPER_PIPE;
const WrapperDataTransferMode   WrapperDataTransfer::ARGUMENTS      = WRAPPER_ARGUMENTS;
const WrapperDataTransferMode   WrapperDataTransfer::SOCKET         = WRAPPER_SOCKET;
const WrapperDataTransferMode   WrapperDataTransfer::CORBA          = WRAPPER_CORBA;


WrapperFunctionDescription::WrapperFunctionDescription()
  : name_(),
    provided_(WrapperSymbolProvided::NO)
{
  // Nothing to do
}

String WrapperFunctionDescription::__repr__() const
{
  OSS oss(true);
  oss << "name="      << name_
      << " provided=" << wrapper_getProvidedAsString( provided_ );
  return oss;
}

String WrapperFunctionDescription::__str__(const String & offset) const
{
  if (provided_) return OSS(false) << name_;
  else return "Not provided";
}




WrapperDataFile::WrapperDataFile()
  : id_(),
    path_(),
    subst_(),
    type_(WrapperDataFileType::IN)
{
  // Nothing to do
}

Bool WrapperDataFile::operator == (const WrapperDataFile & other) const
{
  return (id_ == other.id_) && (path_ == other.path_) && (subst_ == other.subst_) && (type_ == other.type_);
}

String WrapperDataFile::__repr__() const
{
  OSS oss;
  oss << "id="     << id_
      << " name="  << name_
      << " path="  << path_
      << " subst=" << subst_
      << " type="  << wrapper_getListElementTypeAsString( type_ );
  return oss;
}

String WrapperDataFile::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "File '"  << id_ << "' type=" << wrapper_getListElementTypeAsString( type_ );

  if (!name_.empty())  oss << " name="  << name_;
  if (!path_.empty())  oss << " path="  << path_;
  if (!subst_.empty()) oss << " subst=" << subst_;
  return oss;
}



WrapperDataVariable::WrapperDataVariable()
  : id_(),
    comment_(),
    unit_(),
    regexp_(),
    format_(),
    type_(WrapperDataVariableType::IN),
    gradient_(WrapperComputedGradient::NO),
    fromType_(WrapperDataVariableLocation::LINE),
    from_(),
    toType_(WrapperDataVariableLocation::LINE),
    to_()
{
  // Nothing to do
}


Bool WrapperDataVariable::operator == (const WrapperDataVariable & other) const
{
  return (id_ == other.id_) && (comment_ == other.comment_) && (unit_ == other.unit_) &&
         (regexp_ == other.regexp_) && (format_ == other.format_) && (type_ == other.type_) &&
         (gradient_ == other.gradient_) &&
         (fromType_ == other.fromType_) && (from_ == other.from_) &&
         (toType_ == other.toType_) && (to_ == other.to_);
}

String WrapperDataVariable::__repr__() const
{
  OSS oss;
  oss << "id="        << id_
      << " comment="  << comment_
      << " unit="     << unit_
      << " regexp="   << regexp_
      << " format="   << format_
      << " type="     << wrapper_getListElementTypeAsString( type_ )
      << " gradient=" << wrapper_getProvidedAsString( gradient_ )
      << " from="     << from_ << " (from_ type=" << wrapper_getLocationAsString( fromType_ ) << ")"
      << " to="       << to_   << " (to_ type="   << wrapper_getLocationAsString( toType_ )   << ")";
  return oss;
}

String WrapperDataVariable::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "Variable '"  << id_ << "' type=" << wrapper_getListElementTypeAsString( type_ );

  if (!comment_.empty()) oss << " comment="  << comment_;
  if (!unit_.empty())    oss << " unit="     << unit_;
  if (!regexp_.empty())  oss << " regexp="   << regexp_;
  if (!format_.empty())  oss << " format="   << format_;
  if (!from_.empty())    oss << " from="     << from_;
  if (!to_.empty())      oss << " to="       << to_;
  return oss;
}




WrapperParameter::WrapperParameter()
  : state_(WrapperState::SHARED),
    mode_(WrapperMode::STATICLINK),
    in_(WrapperDataTransfer::FILES),
    out_(WrapperDataTransfer::FILES),
    command_(),
    userPrefix_()

{
  // Nothing to do
}


String WrapperParameter::__repr__() const
{
  OSS oss;
  oss << "state="       << wrapper_getConfigurationStateAsString( state_ )
      << " mode="       << wrapper_getConfigurationModeAsString( mode_ )
      << " in="         << wrapper_getDataTransferModeAsString( in_ )
      << " out="        << wrapper_getDataTransferModeAsString( out_ )
      << " command="    << command_
      << " userPrefix=" << userPrefix_;
  return oss;
}

String WrapperParameter::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << "state      : " << wrapper_getConfigurationStateAsString( state_ ) << "\n"
      << offset << "mode       : " << wrapper_getConfigurationModeAsString( mode_ ) << "\n"
      << offset << "in         : " << wrapper_getDataTransferModeAsString( in_ ) << "\n"
      << offset << "out        : " << wrapper_getDataTransferModeAsString( out_ ) << "\n"
      << offset << "command    : " << command_ << "\n"
      << offset << "userPrefix : " << userPrefix_;
  return oss;
}


CLASSNAMEINIT(WrapperData);

/* Default constructor */
WrapperData::WrapperData()
  : Object(),
    libraryPath_(),
    function_(),
    gradient_(),
    hessian_(),
    p_fileList_(new WrapperData::FileListType),
    p_variableList_(new WrapperData::VariableListType),
    p_parameters_(new WrapperParameter)
{
  // Nothing to do
}


/* Constructor from C structure */
WrapperData::WrapperData(const struct WrapperExchangedData * p_exchangedData)
  : Object(),
    libraryPath_(),
    function_(),
    gradient_(),
    hessian_(),
    p_fileList_(new WrapperData::FileListType),
    p_variableList_(new WrapperData::VariableListType),
    p_parameters_(new WrapperParameter)
{
  if ( p_exchangedData )
  {
    wrapper_printWrapperExchangedData( "WrapperData::WrapperData(const struct WrapperExchangedData * p_exchangedData)", p_exchangedData );

    // libraryPath, function, gradient, hessian and platform are not handled by struct WrapperExchangedData
    // so they can't be set here.

    // Set variable list
    const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
    while ( currentVariableElement )
    {
      WrapperDataVariable aVariable;
      aVariable.id_       = currentVariableElement->variable_->id_;
      aVariable.comment_  = currentVariableElement->variable_->comment_;
      aVariable.unit_     = currentVariableElement->variable_->unit_;
      aVariable.regexp_   = currentVariableElement->variable_->regexp_;
      aVariable.format_   = currentVariableElement->variable_->format_;
      aVariable.type_     = currentVariableElement->variable_->type_;
      aVariable.gradient_ = currentVariableElement->variable_->gradient_;
      aVariable.fromType_ = currentVariableElement->variable_->fromType_;
      aVariable.from_     = currentVariableElement->variable_->from_;
      aVariable.toType_   = currentVariableElement->variable_->toType_;
      aVariable.to_       = currentVariableElement->variable_->to_;
      LOGDEBUG( OSS() << "Add aVariable = " << aVariable );
      p_variableList_->add( aVariable );

      currentVariableElement = currentVariableElement->next_;
    } // end while (currentVariableElement)

    // Set file list
    const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
    while ( currentFileElement )
    {
      WrapperDataFile aFile;
      aFile.id_    = currentFileElement->file_->id_;
      aFile.name_  = currentFileElement->file_->name_;
      aFile.path_  = currentFileElement->file_->path_;
      aFile.subst_ = currentFileElement->file_->subst_;
      aFile.type_  = currentFileElement->file_->type_;
      LOGDEBUG( OSS() << "Add aFile = " << aFile );
      p_fileList_->add( aFile );

      currentFileElement = currentFileElement->next_;
    } // end while (currentFileElement)

    // Set parameters
    if ( p_exchangedData->parameters_ )
    {
      p_parameters_->state_      = p_exchangedData->parameters_->state_;
      p_parameters_->mode_       = p_exchangedData->parameters_->mode_;
      p_parameters_->in_         = p_exchangedData->parameters_->in_;
      p_parameters_->out_        = p_exchangedData->parameters_->out_;
      p_parameters_->command_    = p_exchangedData->parameters_->command_;
      p_parameters_->userPrefix_ = p_exchangedData->parameters_->userPrefix_;
      LOGDEBUG( OSS() << "Add * p_parameters = " << *p_parameters_ );
    }

    // Framework isn't copied : not justified for the moment
  }
}



/* String converter */
String WrapperData::__repr__() const
{
  OSS oss;
  oss << "libraryPath=" << libraryPath_
      << " function={"  << function_
      << "} gradient={" << gradient_
      << "} hessian={"  << hessian_
      << "} fileList=[";

  std::copy( p_fileList_->begin(), p_fileList_->end(), OSS_iterator<WrapperDataFile>(oss, ", ") );

  oss << "] variableList=[";

  std::copy( p_variableList_->begin(), p_variableList_->end(), OSS_iterator<WrapperDataVariable>(oss, ", ") );

  oss << "] parameters={" << *p_parameters_
      << "}";
  return oss;

}

/* String converter */
String WrapperData::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "libraryPath  : " << libraryPath_ << "\n"
      << offset << "function     : " << function_    << "\n"
      << offset << "gradient     : " << gradient_    << "\n"
      << offset << "hessian      : " << hessian_     << "\n"
      << offset << "fileList     : [";

  String offset_fl = offset + "                ";
  std::copy( p_fileList_->begin(), p_fileList_->end(), OSS_iterator<WrapperDataFile>(oss,
             String(",\n") + offset_fl) );

  oss << "]/" << p_fileList_->getSize() << "\n"
      << offset << "variableList : [";

  String offset_vl = offset + "                ";
  std::copy( p_variableList_->begin(), p_variableList_->end(), OSS_iterator<WrapperDataVariable>(oss,
             String(",\n") + offset_vl) );

  oss << "]/" << p_variableList_->getSize() << "\n"
      << offset << "parameters :\n" << p_parameters_->__str__( offset + "            " );
  return oss;

}


/* Library path accessor */
void WrapperData::setLibraryPath(const FileName & path)
{
  libraryPath_ = path;
}

/* Library path accessor */
FileName WrapperData::getLibraryPath() const
{
  return libraryPath_;
}

/* Function description accessor */
void WrapperData::setFunctionDescription(const WrapperFunctionDescription & funcDescription)
{
  function_ = funcDescription;
}

/* Function description accessor */
WrapperFunctionDescription WrapperData::getFunctionDescription() const
{
  return function_;
}

/* Gradient description accessor */
void WrapperData::setGradientDescription(const WrapperFunctionDescription & gradDescription)
{
  gradient_ = gradDescription;
}

/* Gradient description accessor */
WrapperFunctionDescription WrapperData::getGradientDescription() const
{
  return gradient_;
}

/* Hessian description accessor */
void WrapperData::setHessianDescription(const WrapperFunctionDescription & hessDescription)
{
  hessian_ = hessDescription;
}

/* Hessian description accessor */
WrapperFunctionDescription WrapperData::getHessianDescription() const
{
  return hessian_;
}

/* Accessor */
void WrapperData::setFileList(const WrapperData::FileListType & fileList)
{
  p_fileList_.reset(new WrapperData::FileListType(fileList));
}



/* Accessor */
const WrapperData::FileListType & WrapperData::getFileList() const
{
  return *p_fileList_;
}


/* Conversion method for C interface */
struct WrapperFileList * WrapperData::getNewFileListForCInterface() const
{
  struct WrapperFileList * fileList = 0;

  // we convert the data into the exchange format
  struct WrapperFileList ** previousElementPointer = & fileList;
  FileListType::const_iterator fileList_iterator;
  for(fileList_iterator  = getFileList().begin();
      fileList_iterator != getFileList().end();
      ++fileList_iterator)
  {
    const WrapperDataFile & file = *fileList_iterator;

    struct WrapperFileList * p_linkOnFile = newC<WrapperFileList>();
    p_linkOnFile->next_ = 0;
    p_linkOnFile->file_ = newC<WrapperFileListElement>();

    p_linkOnFile->file_->id_    = strdup( file.id_.c_str() );
    p_linkOnFile->file_->name_  = strdup( file.name_.c_str() );
    p_linkOnFile->file_->path_  = strdup( file.path_.c_str() );
    p_linkOnFile->file_->subst_ = strdup( file.subst_.c_str() );
    p_linkOnFile->file_->type_  = file.type_;

    // we insert the element into the list...
    *previousElementPointer = p_linkOnFile;

    // ... then we jump to the next element
    previousElementPointer = & p_linkOnFile->next_;

  } /* end for */

  return fileList;
}


/* Frees the memory allocated by getNewFileListForCInterface() method */
void WrapperData::FreeFileListForCInterface(const struct WrapperFileList * fileList)
{
  const struct WrapperFileList * currentFileElement = fileList;
  while (currentFileElement)
  {
    const struct WrapperFileList * nextElement = currentFileElement->next_;
    free( currentFileElement->file_->id_ );
    free( currentFileElement->file_->name_ );
    free( currentFileElement->file_->path_ );
    free( currentFileElement->file_->subst_ );
    deleteC( currentFileElement->file_ );
    deleteC( currentFileElement );
    currentFileElement = nextElement;
  }
}



/* Accessor */
void WrapperData::setVariableList(const WrapperData::VariableListType & variableList)
{
  p_variableList_.reset(new WrapperData::VariableListType(variableList));
}



/* Accessor */
const WrapperData::VariableListType & WrapperData::getVariableList() const
{
  return *p_variableList_;
}


/* Conversion method for C interface */
struct WrapperVariableList * WrapperData::getNewVariableListForCInterface() const
{
  struct WrapperVariableList * variableList = 0;

  // we convert the data into the exchange format
  struct WrapperVariableList ** previousElementPointer = & variableList;
  VariableListType::const_iterator variableList_iterator;
  for(variableList_iterator  = getVariableList().begin();
      variableList_iterator != getVariableList().end();
      ++variableList_iterator)
  {
    const WrapperDataVariable & variable = *variableList_iterator;

    struct WrapperVariableList * p_linkOnVariable = newC<WrapperVariableList>();
    p_linkOnVariable->next_ = 0;
    p_linkOnVariable->variable_ = newC<WrapperVariableListElement>();

    p_linkOnVariable->variable_->id_       = strdup( variable.id_.c_str() );
    p_linkOnVariable->variable_->comment_  = strdup( variable.comment_.c_str() );
    p_linkOnVariable->variable_->unit_     = strdup( variable.unit_.c_str() );
    p_linkOnVariable->variable_->regexp_   = strdup( variable.regexp_.c_str() );
    p_linkOnVariable->variable_->format_   = strdup( variable.format_.c_str() );
    p_linkOnVariable->variable_->type_     = variable.type_;
    p_linkOnVariable->variable_->gradient_ = variable.gradient_;
    p_linkOnVariable->variable_->fromType_ = variable.fromType_;
    p_linkOnVariable->variable_->from_     = strdup( variable.from_.c_str() );
    p_linkOnVariable->variable_->toType_   = variable.toType_;
    p_linkOnVariable->variable_->to_       = strdup( variable.to_.c_str() );

    // we insert the element into the list...
    *previousElementPointer = p_linkOnVariable;

    // ... then we jump to the next element
    previousElementPointer = & p_linkOnVariable->next_;

  } /* end for */

  return variableList;
}


/* Frees the memory allocated by getNewVariableListForCInterface() method */
void WrapperData::FreeVariableListForCInterface(const struct WrapperVariableList * variableList)
{
  const struct WrapperVariableList * currentVariableElement = variableList;
  while (currentVariableElement)
  {
    const struct WrapperVariableList * nextElement = currentVariableElement->next_;
    free( currentVariableElement->variable_->id_ );
    free( currentVariableElement->variable_->comment_ );
    free( currentVariableElement->variable_->unit_ );
    free( currentVariableElement->variable_->regexp_ );
    free( currentVariableElement->variable_->format_ );
    free( currentVariableElement->variable_->from_ );
    free( currentVariableElement->variable_->to_ );
    deleteC( currentVariableElement->variable_ );
    deleteC( currentVariableElement );
    currentVariableElement = nextElement;
  }
}



/* Accessor */
void WrapperData::setParameters(const WrapperParameter & parameters)
{
  p_parameters_.reset(new WrapperParameter(parameters));
}

const WrapperParameter & WrapperData::getParameters() const
{
  return *p_parameters_;
}

/* Conversion method for C interface */
struct WrapperConfiguration * WrapperData::getNewParametersForCInterface() const
{
  struct WrapperConfiguration * parameters = newC<WrapperConfiguration>();

  parameters->state_      = p_parameters_->state_;
  parameters->mode_       = p_parameters_->mode_;
  parameters->in_         = p_parameters_->in_;
  parameters->out_        = p_parameters_->out_;
  parameters->command_    = strdup( p_parameters_->command_.c_str() );
  parameters->userPrefix_ = strdup( p_parameters_->userPrefix_.c_str() );

  return parameters;
}

/* Frees the memory allocated by getNewParametersForCInterface() method */
void WrapperData::FreeParametersForCInterface(const struct WrapperConfiguration * parameters)
{
  free( parameters->command_ );
  free( parameters->userPrefix_ );
  deleteC( parameters );
}


/* Builds a newly allocated WrapperExchangedData structure */
struct WrapperExchangedData * WrapperData::getNewWrapperExchangedDataForCInterface() const
{
  // Create and initialize the structure of data exchanged with the wrapper
  struct WrapperExchangedData * p_exchangedData = newC<WrapperExchangedData>();

  p_exchangedData->fileList_     = getNewFileListForCInterface();
  p_exchangedData->variableList_ = getNewVariableListForCInterface();
  p_exchangedData->parameters_   = getNewParametersForCInterface();
  p_exchangedData->platform_     = newC<PlatformConfiguration>();

  p_exchangedData->platform_->generalTemporaryDirectory_ = strdup(Path::GetTemporaryDirectory().c_str());
  p_exchangedData->platform_->realRegexpShortcut_        = strdup(ResourceMap::Get("real-regexp-shortcut").c_str());
  p_exchangedData->platform_->integerRegexpShortcut_     = strdup(ResourceMap::Get("integer-regexp-shortcut").c_str());
  p_exchangedData->platform_->separatorRegexpShortcut_   = strdup(ResourceMap::Get("separator-regexp-shortcut").c_str());

  p_exchangedData->platform_->nbCpus_             = ResourceMap::GetAsUnsignedInteger("parallel-threads");
  p_exchangedData->platform_->outputFilesTimeout_ = ResourceMap::GetAsUnsignedInteger("output-files-timeout");
  p_exchangedData->platform_->retries_            = ResourceMap::GetAsUnsignedInteger("run-command-retries");
  p_exchangedData->platform_->command_path_       = 0; // malloc() here because command_path_ allocated in C

  // Initialize the embedded mutex
  initMutex(p_exchangedData);

  return p_exchangedData;
}

/* Frees the memory allocated by getNewWrapperExchangedDataForCInterface() method */
void WrapperData::FreeWrapperExchangedDataForCInterface(const struct WrapperExchangedData * exchangedData)
{
  // At last we clean up everything that was allocated
  // ... files
  WrapperData::FreeFileListForCInterface( exchangedData->fileList_ );

  // ... variables
  WrapperData::FreeVariableListForCInterface( exchangedData->variableList_ );

  // ... parameters
  WrapperData::FreeParametersForCInterface( exchangedData->parameters_ );

  // ... platform
  free( exchangedData->platform_->generalTemporaryDirectory_ );
  free( exchangedData->platform_->realRegexpShortcut_ );
  free( exchangedData->platform_->integerRegexpShortcut_ );
  free( exchangedData->platform_->separatorRegexpShortcut_ );
  free( exchangedData->platform_->command_path_ ); // free() here because command_path_ allocated in C
  deleteC( exchangedData->platform_ );

  // Destroy the embedded mutex
  destroyMutex( exchangedData );

  // Free the structure
  deleteC( exchangedData );
}


/* Check the correctness of the stored data */
Bool WrapperData::isValid() const
{
  Bool valid( true );

  if (libraryPath_.empty())
  {
    LOGWARN( OSS() << "Wrapper data validation failed: library path is empty" );
    valid = false;
  }

  if ( (function_.provided_ == WrapperSymbolProvided::YES) &&
       (function_.name_.empty() ) )
  {
    LOGWARN( OSS() << "Wrapper data validation failed: function name is set but empty" );
    valid = false;
  }

  if ( (gradient_.provided_ == WrapperSymbolProvided::YES) &&
       (gradient_.name_.empty() ) )
  {
    LOGWARN( OSS() << "Wrapper data validation failed: gradient name is set but empty" );
    valid = false;
  }

  if ( (hessian_.provided_ == WrapperSymbolProvided::YES) &&
       (hessian_.name_.empty() ) )
  {
    LOGWARN( OSS() << "Wrapper data validation failed: hessian name is set but empty" );
    valid = false;
  }

  for (VariableListType::const_iterator vit = p_variableList_->begin() ;
       vit != p_variableList_->end() ;
       ++vit )
  {
    if ( (*vit).id_.empty() )
    {
      LOGWARN( OSS() << "Wrapper data validation failed: a variable has no id" );
      valid = false;
    }
  }

  for (FileListType::const_iterator fit = p_fileList_->begin() ;
       fit != p_fileList_->end() ;
       ++fit )
  {
    if ( (*fit).id_.empty() )
    {
      LOGWARN( OSS() << "Wrapper data validation failed: a file has no id" );
      valid = false;
    }
    if ( (*fit).path_.empty() )
    {
      LOGWARN( OSS() << "Wrapper data validation failed: a file has no path (id='" << (*fit).id_ << "')" );
      valid = false;
    }
  }

  for (FileListType::const_iterator fit = p_fileList_->begin(); fit != p_fileList_->end(); ++fit)
    CheckSubstitutedVariables( *fit, *p_variableList_ );

  return valid;
}


/* Check if variables listed in subst_ are actually defined */
void WrapperData::CheckSubstitutedVariables(const WrapperDataFile & file,
    const VariableListType & variableList)
{
  std::set<String> aSet = getSetFromSubst( file.subst_.c_str() );
  std::set<String> aSetCopy( aSet );
  Bool found = false;
  for (WrapperData::VariableListType::const_iterator it = variableList.begin();
       !found && it != variableList.end(); ++it )
  {
    if ( inSubst( aSet, it->id_.c_str() ) ) aSetCopy.erase( it->id_ );;
  } // end for variableList

  // Warn for variables that do NOT start with prefix
  const String prefix( "OPENTURNS_" );
  if ( !aSetCopy.empty() )
  {
    for( std::set<String>::const_iterator it = aSetCopy.begin();
         it != aSetCopy.end(); ++it)
      if ( String(*it, 0, prefix.size()) != prefix )
        LOGWARN(OSS() << "The name '" << *it << "' in <subst> tag for file id '" << file.id_ << "' does NOT refer to a defined variable. No substitution will occur for this name. Check syntax");
  }
}

/* Convert void pointer to exchanged data structure pointer */
WrapperData::ExchangedDataPointer WrapperData::ConvertOpaquePointer(void * ptr)
{
  return reinterpret_cast<ExchangedDataPointer>( ptr );
}

/* Convert void pointer to exchanged data structure pointer */
WrapperData::ConstExchangedDataPointer WrapperData::ConvertOpaquePointer(const void * ptr)
{
  return reinterpret_cast<ConstExchangedDataPointer>( ptr );
}


END_NAMESPACE_OPENTURNS
