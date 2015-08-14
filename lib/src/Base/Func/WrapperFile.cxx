//                                               -*- C++ -*-
/**
 *  @brief This class provides all the treatments for wrapper file manipulation
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
#include <vector>                 // for std::vector
#include <map>                    // for std::map
#include <string>                 // for std::string
#include <sstream>                // for std::istringstream
#include <cstdlib>                // for getenv
#include <sys/types.h>            // for stat
#include <sys/stat.h>             // for stat
#include <unistd.h>               // for stat
#include <string.h>               // for strspn
#include "OSS.hxx"
#include "WrapperFile.hxx"
#include "ResourceMap.hxx"
#include "Path.hxx"
#include "Exception.hxx"
#include "Log.hxx"
#include "WrapperCommon_extern.h"
#include "WrapperCommonFunctions.hxx"
#include "XMLToolbox.hxx"

#ifdef XML_SUPPORTED
#include "XMLTags.hxx"
#endif

/* Import functions for wrapper file version management */
#include "WrapperFile_version1.hxx"
#include "WrapperFile_version2.hxx"

BEGIN_NAMESPACE_OPENTURNS


/* The file name extension */
#ifdef XML_SUPPORTED
const String WrapperFile::extension_ = ".xml";
#else
const String WrapperFile::extension_ = ".txt";
#endif



CLASSNAMEINIT(WrapperFile);

/* Default constructor */
WrapperFile::WrapperFile()
  : PersistentObject(),
    descriptionFilePath_(),
    data_(),
    version_(ResourceMap::GetAsUnsignedInteger( "WrapperFile-version" ))
{
  // Nothing to do
}

/* Constructor from file */
WrapperFile::WrapperFile(const FileName & pathToFile)
  : PersistentObject(),
    descriptionFilePath_(pathToFile),
    data_(),
    version_(ResourceMap::GetAsUnsignedInteger( "WrapperFile-version" ))
{
  init();
  LOGINFO(OSS() << "Working on wrapper description file " << pathToFile);
  parseFile(pathToFile);
  done();
}

/* Virtual constructor */
WrapperFile * WrapperFile::clone() const
{
  return new WrapperFile(*this);
}


/* String converter */
String WrapperFile::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " path=" << getDescriptionFilePath()
      << " version=" << version_
      << " data={" << data_
      << "}";
  return oss;
}

/* String converter */
String WrapperFile::__str__(const String & offset) const
{
  return OSS(false) << offset << "WrapperFile at " << getDescriptionFilePath()
         << " version " << version_ << "\n"
         << data_.__str__( offset + "  " );
}

/* Accessor to version number */
WrapperFile::Version WrapperFile::getVersion() const
{
  return version_;
}

void WrapperFile::setVersion(Version v)
{
  if (v > 0)
    version_ = v;
  else
    throw InvalidArgumentException(HERE) << "Version must be greater or equal than 1. Got version=" << v;
}



#ifdef XML_SUPPORTED

typedef FileName (*DTDPathFunction)();
typedef XMLDoc (*WriteFunction)(const WrapperFile & wrapperFile);
typedef WrapperData (*ReadFunction)(const XMLDoc & doc);

struct Driver
{
  DTDPathFunction dtdpath_;
  ReadFunction read_;
  WriteFunction write_;
  Driver() : dtdpath_(0), read_(0), write_(0) {}
  Driver(DTDPathFunction dtdpath, ReadFunction read, WriteFunction write)
    : dtdpath_(dtdpath), read_(read), write_(write) {}
};

typedef std::map< WrapperFile::Version, Driver > DriverMapType;
static const DriverMapType DriverMap;


/* Standard Initialization */
void WrapperFile::init() const
{
  // Nothing to do
}

/* Standard finalization */
void WrapperFile::done() const
{
  // Nothing to do
}


/* Document parsing function */
static WrapperData ParseDocument(const XMLDoc & doc, WrapperFile::Version & wrapperVersion)
{
  WrapperData data;

  // Check it is an OpenTURNS' one
  XML::Node wrapperElt = XML::GetRootNode( doc );
  if (wrapperElt == NULL) throw WrapperFileParsingException(HERE) << "Wrapper description has no root element";
  if (! XML::IsElement( wrapperElt, XMLTag_wrapper ))
    throw WrapperFileParsingException(HERE) << "Wrapper description has an invalid root element (" << XML::GetNodeName( wrapperElt )
                                            << ") at line " << XML::GetNodeLineNumber( wrapperElt );

  // Get wrapper DTD version
  String wrapperVersionSt  = XML::GetAttributeByName( wrapperElt, XMLTag_version );
  if ( wrapperVersionSt.size() != strspn( wrapperVersionSt.c_str(), "0123456789" ) )
    throw WrapperFileParsingException(HERE) << "Wrapper description has an invalid version for wrapper element (" << wrapperVersionSt
                                            << ") at line " << XML::GetNodeLineNumber( wrapperElt );
  std::istringstream iss( wrapperVersionSt );
  iss >> wrapperVersion;

  LOGDEBUG(OSS() << "Read wrapper version                  : " << wrapperVersion );

  DriverMapType::const_iterator drvIt = DriverMap.find( wrapperVersion );
  if (drvIt == DriverMap.end())
  {
    OSS oss;
    const char * sep = "";
    for ( drvIt = DriverMap.begin(); drvIt != DriverMap.end(); ++drvIt, sep = ", " ) oss << sep << drvIt->first;
    throw WrapperFileParsingException(HERE) << "Wrapper stored in version " << wrapperVersion
                                            << " but only the following versions are supported: " << oss.str();
  }
  Driver drv = drvIt->second;

  const FileName DTDPath = (*drv.dtdpath_)();
  Bool ok = doc.hasDTD() ? doc.validate() : doc.validate( XMLTag_wrapper, DTDPath );
  if (!ok) throw WrapperFileParsingException(HERE) << "The wrapper description does not conform to DTD. See log for detailed information.";
  else LOGDEBUG( OSS() << "The wrapper description is valid according to DTD (" << DTDPath << ")" );

  return (*drv.read_)( doc );
}

/* Stream parsing function */
void WrapperFile::parseStream(const String & stream)
{
  // @todo: write stream parsing function
  LOGDEBUG( "Try parsing stream" );

  // Open the stream...
  XMLDoc doc( stream.c_str(), stream.size() );

  // ... and parse it
  data_ = ParseDocument( doc, version_ );

  LOGDEBUG( "Stream successfully parsed" );
}

/* Standard parsing function */
void WrapperFile::parseFile(const FileName & pathToFile)
{
  LOGDEBUG(OSS() << "Try parsing file " << pathToFile);

  // Load the wrapper file...
  XMLDoc doc( pathToFile );

  // ... and parse it
  data_ = ParseDocument( doc, version_ );

  LOGDEBUG(OSS() << "File " << pathToFile << " successfully parsed");
}


/* Make a new document from the internal data */
static XMLDoc MakeDocument(const WrapperFile & wrapperFile)
{
  // TODO : get the correct version number
  WrapperFile::Version wrapperVersion = wrapperFile.getVersion();

  DriverMapType::const_iterator drvIt = DriverMap.find( wrapperVersion );
  if (drvIt == DriverMap.end())
  {
    OSS oss;
    const char * sep = "";
    for ( drvIt = DriverMap.begin(); drvIt != DriverMap.end(); ++drvIt, sep = ", " ) oss << sep << drvIt->first;
    throw WrapperFileParsingException(HERE) << "Can't write wrapper in version " << wrapperVersion
                                            << ". Only the following versions are supported: " << oss.str();
  }
  Driver drv = drvIt->second;
  return (*drv.write_)( wrapperFile );
}

/* Write the internal data to a wrapper file */
void WrapperFile::writeFile(const FileName & pathToFile)
{
  XMLDoc doc = MakeDocument( *this );

  // Write out the document to a file
  doc.save( pathToFile );

  setDescriptionFilePath( pathToFile );
}

/* Stream out the internal data */
String WrapperFile::toString() const
{
  XMLDoc doc = MakeDocument( *this );

  // Stream out the data
  return doc.__repr__();
}

/* DriverMap initialization */
static void WrapperFile_Initialization()
{
  DriverMapType & map = const_cast<DriverMapType &>( DriverMap );
  map[1] = Driver( GetDTDFileName_version1, ParseDocument_version1, MakeDocument_version1 );
  map[2] = Driver( GetDTDFileName_version2, ParseDocument_version2, MakeDocument_version2 );
}

struct DriverMap_init
{
  DriverMap_init()
  {
    static pthread_once_t DriverMap_InstanceMutex_once = PTHREAD_ONCE_INIT;
    int rc = pthread_once( &DriverMap_InstanceMutex_once, WrapperFile_Initialization );
    if (rc != 0)
    {
      perror("DriverMap_init::DriverMap_init once Initialization failed");
      exit(1);
    }
  }
};

static DriverMap_init _DriverMap_initializer;

#else

/* Standard Initialization */
void WrapperFile::init() const
{
  // Nothing to do
}

/* Standard finalization */
void WrapperFile::done() const
{
  // Nothing to do
}

/* Stream parsing function */
void WrapperFile::parseStream(const String & stream)
{
  // @todo: write stream parsing function
  throw WrapperFileParsingException(HERE) << "In WrapperFile::parseStream(const String & stream)";
}

/* Standard parsing function */
void WrapperFile::parseFile(const FileName & pathToFile)
{
  // @todo: write file parsing function
  throw WrapperFileParsingException(HERE) << "In WrapperFile::parseFile(const FileName & pathToFile)";
}

/* Write the internal data to a wrapper file */
void WrapperFile::writeFile(const FileName & pathToFile)
{
  // @todo: write function
  throw NotYetImplementedException(HERE) << "In WrapperFile::writeFile(const FileName & pathToFile)";
}

/* Stream out the internal data */
String WrapperFile::toString() const
{
  // @todo: write function
  throw NotYetImplementedException(HERE) << "In WrapperFile::toString() const";
}
#endif

/* Description file path accessor */
void WrapperFile::setDescriptionFilePath(const FileName & path)
{
  descriptionFilePath_ = path;
}

/* Description file path accessor */
FileName WrapperFile::getDescriptionFilePath() const
{
  return descriptionFilePath_;
}

/* Wrapper data accessor */
void WrapperFile::setWrapperData(const WrapperData & data)
{
  data_ = data;
}

/* Wrapper data accessor */
const WrapperData & WrapperFile::getWrapperData() const
{
  return data_;
}





/*
 * Find the path of a wrapper file from its name.
 */
FileName WrapperFile::FindWrapperPathByName(const String & name)
{
  // Append the extension to the name
  FileName wrapperFileName = name + WrapperFile::extension_;
  LOGDEBUG( OSS() << "Transform '" << name << "' into '" << wrapperFileName << "'" );

  // Get the directory list...
  std::vector<FileName> directoryList = Path::GetWrapperDirectoryList();

  // ... and search it for the file
  FileName wrapperFullPath;
  try
  {
    wrapperFullPath = Path::FindFileByNameInDirectoryList(wrapperFileName, directoryList);

  }
  catch (FileNotFoundException & ex)
  {
    throw NoWrapperFileFoundException(HERE) << ex;
  }

  return wrapperFullPath;

} /* end findWrapperPathByName */



/*
 * Build a wrapper from a name
 */
WrapperFile WrapperFile::FindWrapperByName(const String & name)
{
  // We need to constructs object in the body because of some useful temporary
  // It also speeds the creation of the object because the wrapper file is parsed only once
  FileName wrapperPath;

  // Find the path of the wrapper file that describes the numerical
  // math function interface
  LOGDEBUG(OSS() << "Try loading a wrapper for function '" << name << "'");
  wrapperPath = WrapperFile::FindWrapperPathByName( name );

  // Open the wrapper file and get the name of the library and the names of
  // the function, its gradient and its hessian if defined
  // If the gradient or the hessian are not defined, ask for default ones
  // Then create the actual functions
  // Get also the data read inside the wrapper file and pass them to the wrapper
  WrapperFile wrapperFile( wrapperPath );
  wrapperFile.setName( name );

  return wrapperFile;
} /* end FindWrapperByName */


/*
 * Build a wrapper from a stream
 */
WrapperFile WrapperFile::BuildWrapperFromStream(const String & xmlStream)
{
  WrapperFile wrapper;
  wrapper.parseStream( xmlStream );
  return wrapper;
}


END_NAMESPACE_OPENTURNS
