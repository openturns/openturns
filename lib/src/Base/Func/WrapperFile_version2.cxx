//                                               -*- C++ -*-
/**
 *  @brief This class provides the elementary functions for wrapper file management (version 2)
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
#include "WrapperFile_version2.hxx"
#include "WrapperFile.hxx"
#include "Exception.hxx"
#include "Log.hxx"
#include "Path.hxx"
#include "Os.hxx"
#include "WrapperCommon_extern.h"
#include "XMLToolbox.hxx"

#ifdef XML_SUPPORTED
#include "XMLTags.hxx"

BEGIN_NAMESPACE_OPENTURNS


static const FileName DTDFileName = "wrapper-v2.dtd";

/* Get wrapper DTD file path */
FileName GetDTDFileName_version2()
{
  // Get the directory list...
  std::vector<FileName> directoryList = Path::GetWrapperDirectoryList();

  // ... and search it for the file
  FileName DTDPath;
  try
  {
    DTDPath = Path::FindFileByNameInDirectoryList(DTDFileName, directoryList);

  }
  catch (FileNotFoundException & ex)
  {
    DTDPath = DTDFileName;
  }
  return DTDPath;
}

/* Document parsing function */
WrapperData ParseDocument_version2(const XMLDoc & doc)
{
  WrapperData data;

  XML::Node wrapperElt     = XML::GetRootNode( doc );
  XML::Node libraryElt     = XML::FindElementByName( wrapperElt, XMLTag_library );
  XML::Node libraryPathElt = XML::FindElementByName( libraryElt, XMLTag_path );
  String  libraryPath      = XML::GetNodeValue( libraryPathElt );
  data.setLibraryPath( libraryPath );

  LOGDEBUG(OSS() << "Read library path                     : " << libraryPath );



  XML::Node descriptionElt  = XML::FindElementByName( libraryElt, XMLTag_description );
  XML::Node variableListElt = XML::FindElementByName( descriptionElt, XMLTag_variable_list );
  WrapperData::VariableListType variableList;
  Bool hasOneOrMoreInputVariables  = false;
  Bool hasOneOrMoreOutputVariables = false;
  if (XML::IsElement(variableListElt))
  {
    for (XML::Node current = XML::GetFirstChild(variableListElt); current; current = XML::GetNextNode(current))
    {
      if (XML::IsElement(current, XMLTag_variable))
      {
        WrapperDataVariable variable;

        XML::Node fromElt = XML::FindElementByName( current, XMLTag_from );
        if (fromElt)
        {
          String tp = XML::GetAttributeByName( fromElt, XMLTag_type );

          if (tp == XMLAttr_regexp)
          {
            variable.fromType_ = WrapperDataVariableLocation::REGEXP;
            variable.from_ = XML::GetNodeValue( fromElt );

          }
          else if (tp == XMLAttr_line)
          {
            variable.fromType_ = WrapperDataVariableLocation::LINE;
            variable.from_ = XML::GetNodeValue( fromElt );

          }
          else throw WrapperFileParsingException(HERE) << "Error in parsing element at line "
                << XML::GetNodeLineNumber( current );
        }

        XML::Node toElt = XML::FindElementByName( current, XMLTag_to );
        if (fromElt)
        {
          String tp = XML::GetAttributeByName( toElt, XMLTag_type );

          if (tp == XMLAttr_regexp)
          {
            variable.toType_ = WrapperDataVariableLocation::REGEXP;
            variable.to_ = XML::GetNodeValue( toElt );

          }
          else if (tp == XMLAttr_line)
          {
            variable.toType_ = WrapperDataVariableLocation::LINE;
            variable.to_ = XML::GetNodeValue( toElt );

          }
          else throw WrapperFileParsingException(HERE) << "Error in parsing element at line "
                << XML::GetNodeLineNumber( current );
        }

        XML::Node commentElt = XML::FindElementByName( current, XMLTag_comment );
        if (commentElt) variable.comment_ = XML::GetNodeValue( commentElt );

        XML::Node unitElt    = XML::FindElementByName( current, XMLTag_unit );
        if (unitElt) variable.unit_ = XML::GetNodeValue( unitElt );

        XML::Node regexpElt  = XML::FindElementByName( current, XMLTag_regexp );
        if (regexpElt) variable.regexp_ = XML::GetNodeValue( regexpElt );

        XML::Node formatElt  = XML::FindElementByName( current, XMLTag_format );
        if (formatElt) variable.format_ = XML::GetNodeValue( formatElt );

        variable.id_  = XML::GetAttributeByName( current, XMLTag_id );

        String type_ = XML::GetAttributeByName( current, XMLTag_type );
        if (type_ == XMLAttr_in)
        {
          variable.type_ = WrapperDataVariableType::IN;
          hasOneOrMoreInputVariables = true;
        }
        else if (type_ == XMLAttr_out)
        {
          variable.type_ = WrapperDataVariableType::OUT;
          hasOneOrMoreOutputVariables = true;
        }
        else
        {
          /* We should never go here if the wrapper file has been validated according to the DTD */
          throw WrapperFileParsingException(HERE) << "Unknown type (" << type_
                                                  << ") for variable in wrapper description at line "
                                                  << XML::GetNodeLineNumber( current );
        }

        // @deprecated
        // @todo: remove obsolete computed-gradient attribute
        // Forced to NO since computed gradient is now obsolete and scheduled for removal
        variable.gradient_ = WrapperComputedGradient::NO;

        LOGDEBUG(OSS() << "Read data variable id                 : " << variable.id_);
        LOGDEBUG(OSS() << "               ... type               : " << wrapper_getListElementTypeAsString( variable.type_ ));
        if (fromElt)
          LOGDEBUG(OSS() << "               ... from               : " << variable.from_ << " (type=" << wrapper_getLocationAsString( variable.fromType_ ) << ")");
        if (toElt)
          LOGDEBUG(OSS() << "               ... to                 : " << variable.to_ << " (type=" << wrapper_getLocationAsString( variable.toType_ ) << ")");
        if (commentElt)
          LOGDEBUG(OSS() << "               ... comment            : " << variable.comment_);
        if (unitElt)
          LOGDEBUG(OSS() << "               ... unit               : " << variable.unit_);
        if (regexpElt)
          LOGDEBUG(OSS() << "               ... regexp             : " << variable.regexp_);
        if (formatElt)
          LOGDEBUG(OSS() << "               ... format             : " << variable.format_);

        variableList.add( variable );
      }
    }
  }
  data.setVariableList( variableList );

  if (! hasOneOrMoreInputVariables)  LOGINFO( "Wrapper description doesn't define any input  variable" );
  if (! hasOneOrMoreOutputVariables) LOGINFO( "Wrapper description doesn't define any output variable" );

  WrapperFunctionDescription functionDesc;
  XML::Node functionElt = XML::FindElementByName( descriptionElt, XMLTag_function );
  functionDesc.name_     = XML::GetNodeValue( functionElt );

  String functionProvided_  = XML::GetAttributeByName( functionElt, XMLTag_provided );
  if (functionProvided_ == XMLAttr_yes) functionDesc.provided_ = WrapperSymbolProvided::YES;
  else if (functionProvided_ == XMLAttr_no) functionDesc.provided_ = WrapperSymbolProvided::NO;
  else if (functionProvided_.empty()) functionDesc.provided_ = WrapperSymbolProvided::NO;
  else
  {
    /* We should never go here if the wrapper file has been validated according to the DTD */
    throw WrapperFileParsingException(HERE) << "Unknown " << XMLTag_provided << " attribute (" << functionProvided_
                                            << ") for variable in wrapper description at line "
                                            << XML::GetNodeLineNumber( functionElt );
  }
  data.setFunctionDescription( functionDesc );

  LOGDEBUG(OSS() << "Read function name                    : " << functionDesc.name_);
  LOGDEBUG(OSS() << "               ... provided           : " << wrapper_getProvidedAsString( functionDesc.provided_ ));




  WrapperFunctionDescription gradientDesc;
  XML::Node gradientElt = XML::FindElementByName( descriptionElt, XMLTag_gradient );
  gradientDesc.name_    = XML::GetNodeValue( gradientElt );

  String gradientProvided_  = XML::GetAttributeByName( gradientElt, XMLTag_provided );
  if (gradientProvided_ == XMLAttr_yes) gradientDesc.provided_ = WrapperSymbolProvided::YES;
  else if (gradientProvided_ == XMLAttr_no) gradientDesc.provided_ = WrapperSymbolProvided::NO;
  else if (gradientProvided_.empty()) gradientDesc.provided_ = WrapperSymbolProvided::NO;
  else
  {
    /* We should never go here if the wrapper file has been validated according to the DTD */
    throw WrapperFileParsingException(HERE) << "Unknown " << XMLTag_provided << " attribute (" << gradientProvided_
                                            << ") for variable in wrapper description at line "
                                            << XML::GetNodeLineNumber( gradientElt );
  }
  data.setGradientDescription( gradientDesc );

  LOGDEBUG(OSS() << "Read gradient name                    : " << gradientDesc.name_);
  LOGDEBUG(OSS() << "               ... provided           : " << wrapper_getProvidedAsString( gradientDesc.provided_ ));




  WrapperFunctionDescription hessianDesc;
  XML::Node hessianElt = XML::FindElementByName( descriptionElt, XMLTag_hessian );
  hessianDesc.name_     = XML::GetNodeValue( hessianElt );

  String hessianProvided_  = XML::GetAttributeByName( hessianElt, XMLTag_provided );
  if (hessianProvided_ == XMLAttr_yes) hessianDesc.provided_ = WrapperSymbolProvided::YES;
  else if (hessianProvided_ == XMLAttr_no) hessianDesc.provided_ = WrapperSymbolProvided::NO;
  else if (hessianProvided_.empty()) hessianDesc.provided_ = WrapperSymbolProvided::NO;
  else
  {
    /* We should never go here if the wrapper file has been validated according to the DTD */
    throw WrapperFileParsingException(HERE) << "Unknown " << XMLTag_provided << " attribute (" << hessianProvided_
                                            << ") for variable in wrapper description at line "
                                            << XML::GetNodeLineNumber( hessianElt );
  }
  data.setHessianDescription( hessianDesc );

  LOGDEBUG(OSS() << "Read hessian  name                    : " << hessianDesc.name_);
  LOGDEBUG(OSS() << "               ... provided           : " << wrapper_getProvidedAsString( hessianDesc.provided_ ));



  XML::Node externalCodeElt = XML::FindElementByName( wrapperElt, XMLTag_external_code );
  XML::Node dataElt         = XML::FindElementByName( externalCodeElt, XMLTag_data );
  WrapperData::FileListType fileList;
  Bool hasOneOrMoreInputFiles  = false;
  Bool hasOneOrMoreOutputFiles = false;
  if (XML::IsElement(dataElt))
  {
    for (XML::Node current = XML::GetFirstChild(dataElt); current; current = XML::GetNextNode(current))
    {
      if (XML::IsElement(current, XMLTag_file))
      {
        WrapperDataFile file;

        XML::Node nameElt         = XML::FindElementByName( current, XMLTag_name );
        if (nameElt) file.name_   = XML::GetNodeValue( nameElt );

        XML::Node pathElt         = XML::FindElementByName( current, XMLTag_path );
        file.path_                = XML::GetNodeValue( pathElt );

        XML::Node substElt        = XML::FindElementByName( current, XMLTag_subst );
        if (substElt) file.subst_ = XML::GetNodeValue( substElt );

        file.id_                  = XML::GetAttributeByName( current, XMLTag_id );

        String type_              = XML::GetAttributeByName( current, XMLTag_type );
        if (type_ == XMLAttr_in)
        {
          file.type_ = WrapperDataFileType::IN;
          hasOneOrMoreInputFiles = true;
        }
        else if (type_ == XMLAttr_out)
        {
          file.type_ = WrapperDataFileType::OUT;
          hasOneOrMoreOutputFiles = true;
        }
        else
        {
          /* We should never go here if the wrapper file has been validated according to the DTD */
          throw WrapperFileParsingException(HERE) << "Unknown type (" << type_
                                                  << ") for file in wrapper description at line "
                                                  << XML::GetNodeLineNumber( current );
        }

        LOGDEBUG(OSS() << "Read data file id                     : " << file.id_);
        LOGDEBUG(OSS() << "           ... type                   : " << wrapper_getListElementTypeAsString( file.type_ ));
        LOGDEBUG(OSS() << "           ... name                   : " << file.name_);
        LOGDEBUG(OSS() << "           ... path                   : " << file.path_);
        LOGDEBUG(OSS() << "           ... subst                  : " << file.subst_);

        // Check if variables listed in subst_ are actually defined
        WrapperData::CheckSubstitutedVariables( file, variableList );

        fileList.add( file );

      }
    }
  }
  data.setFileList( fileList );


  WrapperParameter parameters;
  XML::Node wrapModeElt = XML::FindElementByName( externalCodeElt, XMLTag_wrap_mode );
  String  wrapType_      = XML::GetAttributeByName( wrapModeElt, XMLTag_type );
  if (wrapType_ == XMLAttr_static_link) parameters.mode_ = WrapperMode::STATICLINK;
  else if (wrapType_ == XMLAttr_dynamic_link) parameters.mode_ = WrapperMode::DYNAMICLINK;
  else if (wrapType_ == XMLAttr_fork) parameters.mode_ = WrapperMode::FORK;
  else
  {
    /* We should never go here if the wrapper file has been validated according to the DTD */
    throw WrapperFileParsingException(HERE) << "Unknown type (" << wrapType_
                                            << ") for " << XMLTag_wrap_mode << " in wrapper description at line "
                                            << XML::GetNodeLineNumber( wrapModeElt );
  }

  String  wrapState_    = XML::GetAttributeByName( wrapModeElt, XMLTag_state );
  if (wrapState_ == XMLAttr_shared) parameters.state_ = WrapperState::SHARED;
  else if (wrapState_ == XMLAttr_specific) parameters.state_ = WrapperState::SPECIFIC;
  else if (wrapState_.empty()) parameters.state_ = WrapperState::SHARED;
  else
  {
    /* We should never go here if the wrapper file has been validated according to the DTD */
    throw WrapperFileParsingException(HERE) << "Unknown state (" << wrapState_
                                            << ") for " << XMLTag_wrap_mode << " in wrapper description at line "
                                            << XML::GetNodeLineNumber( wrapModeElt );
  }


  XML::Node inDataTransferElt = XML::FindElementByName( wrapModeElt, XMLTag_in_data_transfer );
  String  inMode_              = XML::GetAttributeByName( inDataTransferElt, XMLTag_mode );
  if (inMode_ == XMLAttr_files) parameters.in_ = WrapperDataTransfer::FILES;
  else if (inMode_ == XMLAttr_pipe) parameters.in_ = WrapperDataTransfer::PIPE;
  else if (inMode_ == XMLAttr_arguments) parameters.in_ = WrapperDataTransfer::ARGUMENTS;
  else if (inMode_ == XMLAttr_socket) parameters.in_ = WrapperDataTransfer::SOCKET;
  else if (inMode_ == XMLAttr_corba) parameters.in_ = WrapperDataTransfer::CORBA;
  else
  {
    /* We should never go here if the wrapper file has been validated according to the DTD */
    throw WrapperFileParsingException(HERE) << "Unknown mode (" << inMode_
                                            << ") for " << XMLTag_in_data_transfer << " in wrapper description at line "
                                            << XML::GetNodeLineNumber( inDataTransferElt );
  }


  XML::Node outDataTransferElt = XML::FindElementByName( wrapModeElt, XMLTag_out_data_transfer );
  String  outMode_              = XML::GetAttributeByName( outDataTransferElt, XMLTag_mode );
  if (outMode_ == XMLAttr_files) parameters.out_ = WrapperDataTransfer::FILES;
  else if (outMode_ == XMLAttr_pipe) parameters.out_ = WrapperDataTransfer::PIPE;
  else if (outMode_ == XMLAttr_arguments) parameters.out_ = WrapperDataTransfer::ARGUMENTS;
  else if (outMode_ == XMLAttr_socket) parameters.out_ = WrapperDataTransfer::SOCKET;
  else if (outMode_ == XMLAttr_corba) parameters.out_ = WrapperDataTransfer::CORBA;
  else
  {
    /* We should never go here if the wrapper file has been validated according to the DTD */
    throw WrapperFileParsingException(HERE) << "Unknown mode (" << outMode_
                                            << ") for " << XMLTag_out_data_transfer << " in wrapper description at line "
                                            << XML::GetNodeLineNumber( outDataTransferElt );
  }


  XML::Node commandElt = XML::FindElementByName( externalCodeElt, XMLTag_command );
  parameters.command_  = XML::GetNodeValue( commandElt );


  XML::Node userPrefixElt = XML::FindElementByName( externalCodeElt, XMLTag_user_prefix );
  String pfx = userPrefixElt ? XML::GetNodeValue( userPrefixElt ) : ResourceMap::Get("user-prefix");
  parameters.userPrefix_ = (! pfx.empty()) ? pfx : ResourceMap::Get("user-prefix");

  data.setParameters( parameters );

  LOGDEBUG(OSS() << "Read wrapper mode                     : " << wrapper_getConfigurationModeAsString( parameters.mode_ ));
  LOGDEBUG(OSS() << "         ... state                    : " << wrapper_getConfigurationStateAsString( parameters.state_ ));
  LOGDEBUG(OSS() << "         ... input  transfer mode     : " << wrapper_getDataTransferModeAsString( parameters.in_ ));
  LOGDEBUG(OSS() << "         ... output transfer mode     : " << wrapper_getDataTransferModeAsString( parameters.out_ ));
  LOGDEBUG(OSS() << "         ... command                  : " << parameters.command_);
  LOGDEBUG(OSS() << "         ... user prefix              : " << ( userPrefixElt ? parameters.userPrefix_ : "" ));

  if ( (parameters.in_ == WrapperDataTransfer::FILES) &&
       (! hasOneOrMoreInputFiles) ) LOGWARN( OSS() << "Wrapper description defines input transfer mode as '"
             << wrapper_getDataTransferModeAsString( parameters.in_ )
             << "' but doesn't define any input file" );
  if ( (parameters.out_ == WrapperDataTransfer::FILES) &&
       (! hasOneOrMoreOutputFiles) ) LOGWARN( OSS() << "Wrapper description defines output transfer mode as '"
             << wrapper_getDataTransferModeAsString( parameters.out_ )
             << "' but doesn't define any output file" );

  LOGDEBUG( "Wrapper description successfully parsed" );

  return data;
}


/* Make a new document from the internal data */
XMLDoc MakeDocument_version2(const WrapperFile & wrapperFile)
{
  WrapperData theData = wrapperFile.getWrapperData();
  XMLDoc doc;

  // Set the DTD of the wrapper file
  const FileName DTDPath = GetDTDFileName_version2();
  //XML::SetDTD( doc, XMLTag_wrapper, DTDPath );

  // Create <wrappper> node
  XML::Node wrapper = XML::NewNode( XMLTag_wrapper );
  XML::SetAttribute( wrapper, XMLTag_version, OSS() << wrapperFile.getVersion() );
  XML::SetRootNode( doc, wrapper );

  // Create <library> node
  XML::Node library = XML::NewNode( XMLTag_library );
  XML::AddChild( wrapper, library );

  // Create <path> node
  XML::Node libraryPath = XML::NewNode( XMLTag_path, theData.getLibraryPath() );
  XML::AddChild( library, libraryPath );

  // Create <description> node
  XML::Node description = XML::NewNode( XMLTag_description );
  XML::AddChild( library, description );

  // Create <variable-list> node
  XML::Node variableList = XML::NewNode( XMLTag_variable_list );
  XML::AddChild( description, variableList );

  // Create <variable> node
  const WrapperData::VariableListType vList = theData.getVariableList();
  for (WrapperData::VariableListType::const_iterator it = vList.begin(); it != vList.end(); ++it)
  {
    const WrapperDataVariable & theVar = (*it);
    XML::Node variable = XML::NewNode( XMLTag_variable );
    XML::SetAttribute( variable, XMLTag_id, theVar.id_ );
    XML::SetAttribute( variable, XMLTag_type, (theVar.type_ != WrapperDataVariableType::OUT) ? XMLAttr_in : XMLAttr_out );
    if (theVar.gradient_ == WrapperComputedGradient::YES) XML::SetAttribute( variable, XMLTag_computed_gradient, XMLAttr_yes );
    XML::AddChild( variableList, variable );

    // Create <comment> node
    if (! theVar.comment_.empty())
    {
      XML::Node comment = XML::NewNode( XMLTag_comment, theVar.comment_ );
      XML::AddChild( variable, comment );
    }

    // Create <unit> node
    if (! theVar.unit_.empty())
    {
      XML::Node unit = XML::NewNode( XMLTag_unit, theVar.unit_ );
      XML::AddChild( variable, unit );
    }

    // Create <regexp> node
    if (! theVar.regexp_.empty())
    {
      XML::Node regexp = XML::NewNode( XMLTag_regexp, theVar.regexp_ );
      XML::AddChild( variable, regexp );
    }

    // Create <format> node
    if (! theVar.format_.empty())
    {
      XML::Node format = XML::NewNode( XMLTag_format, theVar.format_ );
      XML::AddChild( variable, format );
    }
  } /* end for ( it = vList.begin() ; ... ) */


  // Create <function> node
  const WrapperFunctionDescription funcDescription = theData.getFunctionDescription();
  XML::Node function = XML::NewNode( XMLTag_function, funcDescription.name_ );
  XML::SetAttribute( function, XMLTag_provided, (funcDescription.provided_ == WrapperSymbolProvided::YES) ? XMLAttr_yes : XMLAttr_no );
  XML::AddChild( description, function );

  // Create <gradient> node
  const WrapperFunctionDescription gradDescription = theData.getGradientDescription();
  XML::Node gradient = XML::NewNode( XMLTag_gradient, gradDescription.name_ );
  XML::SetAttribute( gradient, XMLTag_provided, (gradDescription.provided_ == WrapperSymbolProvided::YES) ? XMLAttr_yes : XMLAttr_no );
  XML::AddChild( description, gradient );

  // Create <hessian> node
  const WrapperFunctionDescription hessDescription = theData.getHessianDescription();
  XML::Node hessian = XML::NewNode( XMLTag_hessian, hessDescription.name_ );
  XML::SetAttribute( hessian, XMLTag_provided, (hessDescription.provided_ == WrapperSymbolProvided::YES) ? XMLAttr_yes : XMLAttr_no );
  XML::AddChild( description, hessian );



  // Create <external-code> node
  XML::Node externalCode = XML::NewNode( XMLTag_external_code );
  XML::AddChild( wrapper, externalCode );

  // Create <data> node
  XML::Node data = XML::NewNode( XMLTag_data );
  XML::AddChild( externalCode, data );

  // Create <file> node
  const WrapperData::FileListType fList = theData.getFileList();
  for (WrapperData::FileListType::const_iterator it = fList.begin(); it != fList.end(); ++it)
  {
    const WrapperDataFile & theFile = (*it);
    XML::Node file = XML::NewNode( XMLTag_file );
    XML::SetAttribute( file, XMLTag_id, theFile.id_ );
    XML::SetAttribute( file, XMLTag_type, (theFile.type_ != WrapperDataFileType::OUT) ? XMLAttr_in : XMLAttr_out );
    XML::AddChild( data, file );

    // Create <name> node
    if (! theFile.name_.empty())
    {
      XML::Node name = XML::NewNode( XMLTag_name, theFile.name_ );
      XML::AddChild( file, name );
    }

    // Create <path> node
    if (! theFile.path_.empty())
    {
      XML::Node path = XML::NewNode( XMLTag_path, theFile.path_ );
      XML::AddChild( file, path );
    }

    // Create <subst> node
    if (! theFile.subst_.empty())
    {
      XML::Node subst = XML::NewNode( XMLTag_subst, theFile.subst_ );
      XML::AddChild( file, subst );
    }
  } /* end for ( it = fList.begin() ; ... ) */

  // Create <wrap-mode> node
  const WrapperParameter & parameters = theData.getParameters();
  XML::Node wrapMode = XML::NewNode( XMLTag_wrap_mode );
  XML::SetAttribute( wrapMode, XMLTag_type, wrapper_getConfigurationModeAsString(  parameters.mode_  ));
  XML::SetAttribute( wrapMode, XMLTag_state, wrapper_getConfigurationStateAsString(  parameters.state_  ));
  XML::AddChild( externalCode, wrapMode );

  // Create <in-data-transfer> node
  XML::Node inDataTransfer = XML::NewNode( XMLTag_in_data_transfer );
  XML::SetAttribute( inDataTransfer, XMLTag_mode, wrapper_getDataTransferModeAsString(  parameters.in_  ));
  XML::AddChild( wrapMode, inDataTransfer );

  // Create <out-data-transfer> node
  XML::Node outDataTransfer = XML::NewNode( XMLTag_out_data_transfer );
  XML::SetAttribute( outDataTransfer, XMLTag_mode, wrapper_getDataTransferModeAsString(  parameters.out_  ));
  XML::AddChild( wrapMode, outDataTransfer );

  // Create <command> node
  XML::Node command = XML::NewNode( XMLTag_command, parameters.command_ );
  XML::AddChild( externalCode, command );

  // Create <user-prefix> node
  XML::Node userPrefix = XML::NewNode( XMLTag_user_prefix, parameters.userPrefix_ );
  XML::AddChild( externalCode, userPrefix );

  Bool ok = doc.validate( XMLTag_wrapper, DTDPath );
  if (!ok) throw InternalException(HERE) << "The generated wrapper does not conform to DTD (" << DTDPath << "). Report bug.";

  return doc;
}

END_NAMESPACE_OPENTURNS

#endif /* XML_SUPPORTED */
