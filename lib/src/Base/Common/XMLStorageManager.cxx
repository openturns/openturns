//                                               -*- C++ -*-
/**
 *  @brief XMLStorageManager provides an interface for different storage classes
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <iomanip>
#include <limits>
#include "openturns/XMLStorageManager.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/InterfaceObject.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Catalog.hxx"
#include "openturns/Study.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PlatformInfo.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS



#if defined OPENTURNS_HAVE_LIBXML2




const int XMLStorageManager::Precision_ = 17;

CLASSNAMEINIT(XMLStorageManager)

/* Default constructor */
XMLStorageManager::XMLStorageManager(const FileName & filename,
                                     const UnsignedInteger compressionLevel)
  : StorageManager(OPENTURNS_VERSION),
    p_state_(new XMLStorageManagerState),
    p_document_(),
    fileName_(filename),
    compressionLevel_(compressionLevel)
{
  if (compressionLevel > 9)
    throw InvalidArgumentException(HERE) << "Compression level should be in [0; 9]";
}


/*
 * Virtual constructor
 */
XMLStorageManager * XMLStorageManager::clone() const
{
  return new XMLStorageManager(*this);
}

/* String converter */
String XMLStorageManager::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


/* Filename accessors */
String XMLStorageManager::getFileName() const
{
  return fileName_;
}

void XMLStorageManager::setFileName(const String & fileName)
{
  fileName_ = fileName;
}


/* Return the current state of the storage manager (for those having one) */
const StorageManager::InternalObject & XMLStorageManager::getState() const
{
  assert(p_state_);
  return *p_state_;
}



/* Query the manager if the version is correct */
Bool XMLStorageManager::canManageVersion(UnsignedInteger version) const
{
  // backward compatibility
  return (version <= getDefaultStudyVersion());
}


/* Do some administrative tasks before saving/reloading */
void XMLStorageManager::initialize(const SaveAction )
{
  cleanSavedObjects();
  OSS oss;
  oss << getStudyVersion();
  p_document_.reset(new XMLDoc);
  assert(p_document_);
  p_document_->setCompressionLevel(compressionLevel_);
  assert(p_state_);
  p_state_->root_ = XML::NewNode( XML_STMGR::root_tag::Get() );
  XML::SetAttribute(p_state_->root_, XML_STMGR::version_attribute::Get(), oss);
  XML::SetRootNode(*p_document_, p_state_->root_);
  setStorageManager();
}


void XMLStorageManager::setStorageManager()
{
  XML::SetAttribute(p_state_->root_, XML_STMGR::manager_attribute::Get(), "XMLStorageManager");
}


void XMLStorageManager::checkStorageManager()
{
  if (XML::GetAttributeByName( p_state_->root_, XML_STMGR::manager_attribute::Get()) !=
      "XMLStorageManager")
    throw StudyFileParsingException(HERE) << XML::GetAttributeByName( p_state_->root_, XML_STMGR::manager_attribute::Get())
                                          << " is used in study file. XMLStorageManager is expected";
}


/* Do some administrative tasks before saving/reloading */
void XMLStorageManager::initialize(const LoadAction )
{
  p_document_.reset(new XMLDoc( fileName_ ));
  p_document_->setCompressionLevel(compressionLevel_);
}

/* Do some administrative tasks after saving/reloading */
void XMLStorageManager::finalize(const SaveAction )
{
  // Nothing to do
}

/* Do some administrative tasks after saving/reloading */
void XMLStorageManager::finalize(const LoadAction )
{
  // Nothing to do
}



/* Read and create the internal representation */
void XMLStorageManager::read()
{
  assert(p_document_);
  assert(p_state_);
  p_state_->root_ = XML::GetRootNode( *p_document_ );
  if (! p_state_->root_) throw StudyFileParsingException(HERE) << "Study file has no root element (" << fileName_ << ")";
  if (! XML::IsElement( p_state_->root_, XML_STMGR::root_tag::Get() ))
    throw StudyFileParsingException(HERE) << "Can NOT find root element '" << XML_STMGR::root_tag::Get() << "' in file '" << fileName_ << "'. Got '" << XML::GetNodeName( p_state_->root_ ) << "'";
  UnsignedInteger version = 0;
  String stul = XML::GetAttributeByName( p_state_->root_, XML_STMGR::version_attribute::Get() );
  std::istringstream iss (stul);
  iss >> version;
  setStudyVersion(version);
  if (!XML::ElementHasAttribute( p_state_->root_, XML_STMGR::manager_attribute::Get()))
    setStorageManager();
  checkStorageManager();
}

/* Write the internal representation */
void XMLStorageManager::write()
{
  assert(p_document_);
  p_document_->save( fileName_ );
}


/*
 * This method saves the PersistentObject onto the medium
 */
void XMLStorageManager::save(const PersistentObject & obj, const String & label, bool fromStudy)
{
#ifdef SAVELOAD_CHECKING
  try
  {
#endif
    if (! isSavedObject( obj.getId() ))
    {
      obj.save(*this, label, fromStudy);
      markObjectAsSaved( obj.getId() );
    }
#ifdef SAVELOAD_CHECKING
  }
  catch ( const Exception & ex )
  {
    LOGERROR( OSS() << "When saving " << obj.getClassName() << " : " << ex );
  }
#endif
}

/*
 * This method reloads the PersistentObject from the medium
 */
void XMLStorageManager::load(Study & study)
{
  setStudy( &study );
  assert(p_state_);
  XML::Node node = XML::GetFirstChild( p_state_->root_ );
  while( node )
  {
    p_state_->current_ = node;
    try
    {
      XMLReadObject ro = readDOMElement();
      if (ro.p_obj_)
      {
        study.add( ro.label_, *(ro.p_obj_) );
      }
    }
    catch (const InternalException & ex)
    {
      LOGINFO(OSS() << ex);
    }
    node = XML::GetNextNode(node);
  }
}




/* Methods to read DOM elements */
XMLStorageManager::XMLReadObject XMLStorageManager::readDOMElement()
{
  assert(p_state_);
  XMLReadObject ro;
  if ( p_state_->current_ )
  {
    if (XML::IsElement(p_state_->current_, XML_STMGR::object_tag::Get() ) )
    {
      String className = XML::GetAttributeByName(p_state_->current_, XML_STMGR::class_attribute::Get());
      ro.label_      = XML::GetAttributeByName(p_state_->current_, XML_STMGR::StudyLabel_attribute::Get());
      ro.visibility_ = XML::GetAttributeByName(p_state_->current_, XML_STMGR::StudyVisible_attribute::Get());
      try
      {
        ro.p_obj_      = Catalog::Get(className).build(*this);
        if (! ro.visibility_.empty())
          ro.p_obj_->setVisibility(ro.visibility_ == "true");
      }
      catch(...)
      {
        throw InternalException(HERE) << "Error trying to load " << ro.label_ << ", skipped.";
      }
    }
  }

  return ro;
}





/* Create a new empty object that will gather all saved information */
Pointer<StorageManager::InternalObject> XMLStorageManager::createObject(const String & tag) const
{
  return new XMLInternalObject( XML::NewNode(tag) );
}


/* Append an internal object to the collection of saved ones */
void XMLStorageManager::appendObject(Pointer<InternalObject> & p_obj)
{
  assert(p_state_);
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  XML::AddChild( p_state_->root_, node );
}


void XMLStorageManager::setVisibility(Pointer<InternalObject> & p_obj, Bool visible)
{
  addAttribute( p_obj, XML_STMGR::StudyVisible_attribute::Get(), visible);
}

void XMLStorageManager::setLabel(Pointer<InternalObject> & p_obj, const String & label)
{
  addAttribute( p_obj, XML_STMGR::StudyLabel_attribute::Get(), label);
}



/************ Helper Functions ************/


/* toStringConverter */

static inline
void toStringConverter(const String & value, String & st)
{
  st = value;
}

template <typename _Tp>
static inline
void toStringConverter(const _Tp & value, String & st)
{
  const int oldPrecision(PlatformInfo::GetNumericalPrecision());
  PlatformInfo::SetNumericalPrecision(XMLStorageManager::Precision_);
  st = ( OSS() << value );
  PlatformInfo::SetNumericalPrecision(oldPrecision);

  if (SpecFunc::IsNaN(value))
  {
    st = "nan";
  }
  else if (SpecFunc::IsInf(value))
  {
    st = (value < 0.0) ? "-inf" : "inf";
  }
}



/* fromStringConverter */

static inline
void fromStringConverter(const String & st, Bool & value)
{
  value = (st == "true") ? true : false;
}

static inline
void fromStringConverter(const String & st, String & value)
{
  value = st;
}


template <typename _Tp>
static inline
void fromStringConverter(const String & st, _Tp & value)
{
  std::istringstream iss (st);
  iss >> value;

  if (st == "nan")
  {
    value = std::numeric_limits<_Tp>::quiet_NaN();
  }
  else if (st == "inf")
  {
    value = std::numeric_limits<_Tp>::infinity();
  }
  else if (st == "-inf")
  {
    value = -std::numeric_limits<_Tp>::infinity();
  }
}



/* getValueToConvert */

template <typename TAG>
static inline
String getValueToConvert(XML::Node node)
{
  return XML::GetNodeValue( node );
}

template <>
inline
String getValueToConvert<XML_STMGR::object_tag>(XML::Node node)
{
  return XML::GetAttributeByName( node, XML_STMGR::id_attribute::Get() );
}



/* fromNodeConverter */

template <typename TAG, typename _Tp>
static inline
void fromNodeConverter(XML::Node node, _Tp & value)
{
  fromStringConverter( getValueToConvert<TAG>( node ), value );
}


template <>
inline
void fromNodeConverter<XML_STMGR::numericalcomplex_tag, Complex>(XML::Node node,
    Complex & value)
{
  XML::Node node_real = XML::FindElementByName( node, XML_STMGR::real_tag::Get() );
  XML::Node node_imag = XML::FindElementByName( node, XML_STMGR::imag_tag::Get() );
  Scalar real = -1.0;
  Scalar imag = -1.0;
  fromStringConverter( getValueToConvert<XML_STMGR::real_tag>( node_real ), real );
  fromStringConverter( getValueToConvert<XML_STMGR::imag_tag>( node_imag ), imag );
  value = Complex( real, imag );
}




/************ Generic Functions ************/


/* AttributeWriter */

template <typename _Tp>
static inline
void AttributeWriter(Pointer<StorageManager::InternalObject> & p_obj,
                     const String & name,
                     _Tp value)
{
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  String attrValue;
  toStringConverter( value, attrValue );
  XML::SetAttribute( node, name, attrValue );
}


template <>
inline
void AttributeWriter<Complex>(Pointer<StorageManager::InternalObject> & p_obj,
                              const String & name,
                              Complex value)
{
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  XML::Node child = XML::NewNode( XML_STMGR::numericalcomplex_tag::Get() );
  assert(child);
  XML::SetAttribute( child, XML_STMGR::name_attribute::Get(), name );
  int oldPrecision(PlatformInfo::GetNumericalPrecision());
  PlatformInfo::SetNumericalPrecision(XMLStorageManager::Precision_);
  XML::Node real = XML::NewNode( XML_STMGR::real_tag::Get(), OSS() << value.real() );
  PlatformInfo::SetNumericalPrecision(oldPrecision);
  assert(real);
  XML::AddChild( child, real );
  oldPrecision = PlatformInfo::GetNumericalPrecision();
  PlatformInfo::SetNumericalPrecision(XMLStorageManager::Precision_);
  XML::Node imag = XML::NewNode( XML_STMGR::imag_tag::Get(), OSS() << value.imag() );
  PlatformInfo::SetNumericalPrecision(oldPrecision);
  assert(imag);
  XML::AddChild( child, imag );
  XML::AddChild( node, child );
}




/* AttributeReader */

template <typename TAG, typename _Tp>
static inline
void AttributeReader(TAG,
                     Pointer<XMLStorageManagerState> & p_state,
                     const String & name,
                     _Tp & value)
{
  assert(p_state);
  String st = XML::GetAttributeByName( p_state->current_, name );
  fromStringConverter( st, value );
}




/* IndexedValueWriter */

template <typename TAG, typename _Tp>
static inline
void IndexedValueWriter(TAG tag,
                        Pointer<StorageManager::InternalObject> & p_obj,
                        UnsignedInteger index,
                        _Tp value)
{
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  String attrValue;
  toStringConverter( value, attrValue );
  XML::Node child = XML::NewNode( tag.Get(), attrValue );
  assert(child);
  XML::SetAttribute( child, XML_STMGR::index_attribute::Get(), OSS() << index );
  XML::AddChild( node, child );
}


template <>
inline
void IndexedValueWriter<XML_STMGR::numericalcomplex_tag, Complex>(XML_STMGR::numericalcomplex_tag,
    Pointer<StorageManager::InternalObject> & p_obj,
    UnsignedInteger index,
    Complex value)
{
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  XML::Node child = XML::NewNode( XML_STMGR::numericalcomplex_tag::Get() );
  assert(child);
  XML::SetAttribute( child, XML_STMGR::index_attribute::Get(), OSS() << index );
  int oldPrecision(PlatformInfo::GetNumericalPrecision());
  PlatformInfo::SetNumericalPrecision(XMLStorageManager::Precision_);
  XML::Node real = XML::NewNode( XML_STMGR::real_tag::Get(), OSS() << value.real() );
  PlatformInfo::SetNumericalPrecision(oldPrecision);
  assert(real);
  XML::AddChild( child, real );
  oldPrecision = PlatformInfo::GetNumericalPrecision();
  PlatformInfo::SetNumericalPrecision(XMLStorageManager::Precision_);
  XML::Node imag = XML::NewNode( XML_STMGR::imag_tag::Get(), OSS() << value.imag() );
  PlatformInfo::SetNumericalPrecision(oldPrecision);
  assert(imag);
  XML::AddChild( child, imag );
  XML::AddChild( node, child );
}





/* IndexedValueReader */

template <typename TAG, typename _Tp>
static inline
void IndexedValueReader(TAG tag,
                        Pointer<StorageManager::InternalObject> & p_obj,
                        UnsignedInteger index,
                        _Tp & value)
{
  assert(p_obj);
  XMLStorageManagerState & state = dynamic_cast<XMLStorageManagerState &>(*p_obj);

  XML::Node node;
  while (( node = XML::FindNextElementByName( state.current_, tag.Get() ) ))
  {
    UnsignedInteger idx = 0;
    fromStringConverter( XML::GetAttributeByName(node, XML_STMGR::index_attribute::Get()), idx );
    // Necessary, -though counter-intuitive- otherwise the XMLH5StoManSta::next can be called
    state.XMLStorageManagerState::next();
    if (idx == index)
    {
      fromNodeConverter<TAG, _Tp>( node, value );
      return;
    }
  }

  LOGWARN( OSS() << "Failed when reading indexed value. Expected tag '" << tag.Get()
           << "'. Got '" << XML::GetNodeName(state.current_) << "'" );
}



/* NamedObjectWriter */

template <typename TAG, typename _Tp>
static inline
void NamedObjectWriter(TAG tag,
                       Pointer<StorageManager::InternalObject> & p_obj,
                       const String & name,
                       _Tp & value)
{
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  XML::Node child = XML::NewNode( tag.Get() );
  assert(child);
  XML::SetAttribute( child, XML_STMGR::member_attribute::Get(), name);
  XML::SetAttribute( child, XML_STMGR::id_attribute::Get(), OSS() << value.getId() );
  XML::AddChild( node, child );
}



/* NamedObjectReader */

template <typename TAG, typename _Tp>
static inline
int NamedObjectReader(TAG tag,
                      Pointer<StorageManager::InternalObject> & p_obj,
                      const String & name,
                      _Tp & value)
{
  assert(p_obj);
  XMLStorageManagerState state = dynamic_cast<XMLStorageManagerState &>(*p_obj);

  XML::Node node = XML::FindElementByName( state.current_, tag.Get() );
  while ( node )
  {
    String nm;
    fromStringConverter( XML::GetAttributeByName(node, XML_STMGR::member_attribute::Get()), nm );
    if (nm == name)
    {
      fromNodeConverter<TAG, _Tp>( node, value );
      return 1;
    }
    node = XML::FindNextElementByName( node, tag.Get() );
  }

  return 0;
}





/************ Type = Bool ************/

/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     Bool value)
{
  AttributeWriter( p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> &,
                                      const String & name,
                                      Bool & value)
{
  AttributeReader( XML_STMGR::bool_tag(), p_state_, name, value );
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        Bool value)
{
  IndexedValueWriter( XML_STMGR::bool_tag(), p_obj, index, value );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Bool & value)
{
  IndexedValueReader( XML_STMGR::bool_tag(), p_obj, index, value );
}




/************ Type = UnsignedInteger ************/


/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     UnsignedInteger value)
{
  AttributeWriter( p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> &,
                                      const String & name,
                                      UnsignedInteger & value)
{
  AttributeReader( XML_STMGR::unsignedlong_tag(), p_state_, name, value );
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        UnsignedInteger value)
{
  IndexedValueWriter( XML_STMGR::unsignedlong_tag(), p_obj, index, value );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    UnsignedInteger & value)
{
  IndexedValueReader( XML_STMGR::unsignedlong_tag(), p_obj, index, value );
}


#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
/************ Type = Unsigned64BitsInteger ************/

/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     Unsigned64BitsInteger value)
{
  AttributeWriter( p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> &,
                                      const String & name,
                                      Unsigned64BitsInteger & value)
{
  AttributeReader( XML_STMGR::unsignedlong_tag(), p_state_, name, value );
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        Unsigned64BitsInteger value)
{
  IndexedValueWriter( XML_STMGR::unsignedlong_tag(), p_obj, index, value );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Unsigned64BitsInteger & value)
{
  IndexedValueReader( XML_STMGR::unsignedlong_tag(), p_obj, index, value );
}

#endif

/************ Type = Scalar ************/


/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     Scalar value)
{
  AttributeWriter( p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> &,
                                      const String & name,
                                      Scalar & value)
{
  AttributeReader( XML_STMGR::numericalscalar_tag(), p_state_, name, value );
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        Scalar value)
{
  IndexedValueWriter( XML_STMGR::numericalscalar_tag(), p_obj, index, value );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Scalar & value)
{
  IndexedValueReader( XML_STMGR::numericalscalar_tag(), p_obj, index, value );
}




/************ Type = Complex ************/


/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     Complex value)
{
  AttributeWriter( p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> &,
                                      const String & name,
                                      Complex & value)
{
  AttributeReader( XML_STMGR::numericalcomplex_tag(), p_state_, name, value );
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        Complex value)
{
  IndexedValueWriter( XML_STMGR::numericalcomplex_tag(), p_obj, index, value );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Complex & value)
{
  IndexedValueReader( XML_STMGR::numericalcomplex_tag(), p_obj, index, value );
}




/************ Type = String ************/


/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     const String & value)
{
  AttributeWriter( p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> &,
                                      const String & name,
                                      String & value)
{
  AttributeReader( XML_STMGR::string_tag(), p_state_, name, value );
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        const String & value)
{
  IndexedValueWriter( XML_STMGR::string_tag(), p_obj, index, value );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    String & value)
{
  IndexedValueReader( XML_STMGR::string_tag(), p_obj, index, value );
}




/************ Type = InterfaceObject ************/


/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     const InterfaceObject & value)
{
  value.save( *this );
  NamedObjectWriter( XML_STMGR::object_tag(), p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
                                      const String & name,
                                      InterfaceObject & value)
{
  Id shadowedId;
  if (NamedObjectReader( XML_STMGR::object_tag(), p_obj, name, shadowedId ))
  {
    if (! getStudy()->hasObject(shadowedId))
      throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
                                            << " in file '" << getFileName()
                                            << "' referenced before used";
    value.setImplementationAsPersistentObject(getStudy()->getObject(shadowedId));
  }
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        const InterfaceObject & value)
{
  value.save( *this );

  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  XML::Node child = XML::NewNode( XML_STMGR::object_tag::Get() );
  assert(child);
  XML::SetAttribute( child, XML_STMGR::index_attribute::Get(), OSS() << index );
  XML::SetAttribute( child, XML_STMGR::id_attribute::Get(), OSS() << value.getId());
  XML::AddChild( node, child );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    InterfaceObject & value)
{
  Id shadowedId;
  IndexedValueReader( XML_STMGR::object_tag(), p_obj, index, shadowedId );
  if (! getStudy()->hasObject(shadowedId))
    throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
                                          << " in file '" << getFileName()
                                          << "' referenced before used";
  value.setImplementationAsPersistentObject(getStudy()->getObject(shadowedId));
}


/* Returns true if the internal object has an attribute
 */
Bool XMLStorageManager::hasAttribute(Pointer<InternalObject> & p_obj, const String & name)
{
  assert(p_obj);
  XMLStorageManagerState & state = dynamic_cast<XMLStorageManagerState &>(*p_obj);
  XML::Node node = state.current_;
  assert(node);
  // check simple attributes
  if (XML::ElementHasAttribute(node, name)) return true;
  // check object attributes
  node = XML::GetFirstChild(node);
  while (node)
  {
    if (XML::GetAttributeByName(node, XML_STMGR::member_attribute::Get()) == name) return true;
    node = XML::GetNextNode(node);
  }
  return false;
}


/************ Type = PersistentObject ************/


/* Add an attribute to an internal object */
void XMLStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                     const String & name,
                                     const PersistentObject & value)
{
  value.save( *this );
  NamedObjectWriter( XML_STMGR::object_tag(), p_obj, name, value );
}

/* Read an attribute */
void XMLStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
                                      const String & name,
                                      PersistentObject & value)
{
  Id shadowedId;
  if (NamedObjectReader( XML_STMGR::object_tag(), p_obj, name, shadowedId ))
  {
    if (! getStudy()->hasObject(shadowedId))
      throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
                                            << " in file '" << getFileName()
                                            << "' referenced before used";
    Study * p_study = getStudy();
    Pointer<PersistentObject> p_po = p_study->getObject(shadowedId);
    Catalog::Get(value.getClassName()).assign(value, *p_po);
  }
}

/* Add an indexed value to an internal object */
void XMLStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
                                        UnsignedInteger index,
                                        const PersistentObject & value)
{
  value.save( *this );

  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  XML::Node child = XML::NewNode( XML_STMGR::object_tag::Get() );
  assert(child);
  XML::SetAttribute( child, XML_STMGR::index_attribute::Get(), OSS() << index );
  XML::SetAttribute( child, XML_STMGR::id_attribute::Get(), OSS() << value.getId());
  XML::AddChild( node, child );
}

/* Read an indexed value */
void XMLStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    PersistentObject & value)
{
  Id shadowedId;
  IndexedValueReader( XML_STMGR::object_tag(), p_obj, index, shadowedId );
  if (! getStudy()->hasObject(shadowedId))
    throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
                                          << " in file '" << getFileName()
                                          << "' referenced before used";
  Study * p_study = getStudy();
  Pointer<PersistentObject> p_po = p_study->getObject(shadowedId);
  Catalog::Get(value.getClassName()).assign(value, *p_po);
}




#endif /* OPENTURNS_HAVE_LIBXML2 */

END_NAMESPACE_OPENTURNS
