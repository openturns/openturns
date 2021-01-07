//                                               -*- C++ -*-
/**
 *  @brief BinaryStorageManager provides an interface for different storage classes
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
// For lseek64(3)
#define _LARGEFILE64_SOURCE
#include <iomanip>
#include <sys/stat.h>  // for open(3)
#include <fcntl.h>     // for open(3)
#include <sys/types.h> // for lseek64(3)
#include <unistd.h>    // for lseek64(3)
#include <errno.h>     // for errno(3)
#include <string.h>    // for strerror(3)
#include "openturns/BinaryStorageManager.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/InterfaceObject.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Catalog.hxx"
#include "openturns/Study.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



#define MAGIC "OpenTURNS-binary"

/************ Attributes ************/

#define DEFINE_ATTRIBUTE(name,value)                                    \
  static char name ## Attribute = value;                                \
  struct name ## _attribute { typedef short Type; static inline Type Get() { return name ## Attribute ; } }

namespace BIN_STMGR
{
DEFINE_ATTRIBUTE( StudyVisible, 0 );
DEFINE_ATTRIBUTE( StudyLabel, 1 );
DEFINE_ATTRIBUTE( version, 2 );
DEFINE_ATTRIBUTE( class, 3 );
DEFINE_ATTRIBUTE( id, 4 );
DEFINE_ATTRIBUTE( name, 5 );
DEFINE_ATTRIBUTE( index, 6 );
DEFINE_ATTRIBUTE( member, 7 );
} // namespace BIN_STMGR

/** @todo: ensure that reads and writes are actually done */

static inline
void doWrite( int fd, const void * buf, size_t count )
{
  ssize_t bytes;
  while ( (bytes = write( fd, buf, count )) > -1 )
  {
    if (bytes == count) break;
    reinterpret_cast<const char *&>(buf) += bytes;
    count -= bytes;
  }
  if (bytes < 0) throw InternalException(HERE) << "Error in writing file. Reason: " << strerror(errno);
}

static inline
void doRead( int fd, void * buf, size_t count )
{
  ssize_t bytes;
  while ( (bytes = read( fd, buf, count )) > -1 )
  {
    if (bytes == count) break;
    reinterpret_cast<char *&>(buf) += bytes;
    count -= bytes;
  }
  if (bytes < 0) throw InternalException(HERE) << "Error in reading file. Reason: " << strerror(errno);
}

template <typename T>
static inline
void writeToFile( int fd, const T & obj )
{
  doWrite( fd, static_cast<const void *>(&obj), sizeof(obj) );
}


template <>
inline
void writeToFile<String>( int fd, const String & st )
{
  writeToFile( fd, st.size() );
  doWrite( fd, static_cast<const void *>(st.c_str()), st.size() );
}

template <>
inline
void writeToFile<Complex>( int fd, const Complex & cpx )
{
  writeToFile( fd, cpx.real() );
  writeToFile( fd, cpx.imag() );
}

template <typename T>
static inline
void readFromFile( int fd, T & obj )
{
  doRead( fd, static_cast<void *>(&obj), sizeof(obj) );
}


template <>
inline
void readFromFile<String>( int fd, String & st )
{
  String::size_type sz;
  readFromFile( fd, sz );
  char * tmp = new char[sz + 1];
  doRead( fd, static_cast<void *>(tmp), sz );
  tmp[sz] = 0;
  st = tmp;
  delete[] tmp;
}

template <>
inline
void readFromFile<Complex>( int fd, Complex & cpx )
{
  readFromFile( fd, cpx.real() );
  readFromFile( fd, cpx.imag() );
}


/* ====================================================================== */


struct BinaryInternalObject : public StorageManager::InternalObject
{
  Id id_;
  String label_;
  Bool visibility_;
  size_t size_;
  void * buf_;
  BinaryInternalObject() : id_(0), label_(), visibility_(false), size_(0), buf_(0) {}
  virtual ~BinaryInternalObject() throw()
  {
    free( buf_ );
  }
  template <typename T>
  void add(T val)
  {
    buf_ = realloc( buf_, size_ + sizeof( T ) );
    memcpy( buf_ + size_, &val, sizeof(T) );
    size_ += sizeof( T );
  }
};

struct BinaryStorageManagerState : public StorageManager::InternalObject
{
  Id id_;
  Bool visibility_;
  BinaryStorageManagerState() : id_(0), visibility_(false) {}
  virtual ~BinaryStorageManagerState() throw() {}
};


/* ====================================================================== */


CLASSNAMEINIT(BinaryStorageManager)
const BinaryStorageManager::VersionList BinaryStorageManager::SupportedVersions;

/* Default constructor */
BinaryStorageManager::BinaryStorageManager(const FileName & filename)
  : StorageManager(1),
    fileName_(filename),
    p_state_(new BinaryStorageManagerState),
    filefd_(-1),
    visibleObjects_()
{
  // Nothing to do
}

/*
 * Virtual constructor
 */
BinaryStorageManager * BinaryStorageManager::clone() const
{
  return new BinaryStorageManager(*this);
}

/* String converter */
String BinaryStorageManager::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


/* Filename accessors */
String BinaryStorageManager::getFileName() const
{
  return fileName_;
}

void BinaryStorageManager::setFileName(const String & fileName)
{
  fileName_ = fileName;
}


/* Return the current state of the storage manager (for those having one) */
const StorageManager::InternalObject & BinaryStorageManager::getState() const
{
  assert(p_state_);
  return *p_state_;
}



/* Query the manager if the version is correct */
Bool BinaryStorageManager::canManageVersion(UnsignedInteger version) const
{
  return BinaryStorageManager::SupportedVersions.contains(version);
}

/* Erase the map of visible objects */
void BinaryStorageManager::cleanVisibleObjects()
{
  visibleObjects_.erase( visibleObjects_.begin(), visibleObjects_.end() );
}


/* Do some administrative tasks before saving/reloading */
void BinaryStorageManager::initialize(const SaveAction caller)
{
  cleanSavedObjects();
  cleanVisibleObjects();
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  filefd_ = open( fileName_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mode);
  if (filefd_ < 0) throw FileOpenException(HERE) << "Can NOT open binary file " << fileName_
        << ". Reason: " << strerror(errno);
  // Save header
  doWrite( filefd_, static_cast<const void *>(MAGIC), strlen(MAGIC) );
}

/* Do some administrative tasks before saving/reloading */
void BinaryStorageManager::initialize(const LoadAction caller)
{
  cleanVisibleObjects();
  filefd_ = open( fileName_.c_str(), O_RDONLY );
  if (filefd_ < 0) throw FileNotFoundException(HERE) << "Can NOT open binary file " << fileName_
        << ". Reason: " << strerror(errno);
  // Read header
  char * buf = new char[strlen(MAGIC)];
  if (buf != 0)
  {
    doRead( filefd_, static_cast<void *>(buf), strlen(MAGIC) );
    if (strncmp(buf, MAGIC, strlen(MAGIC))) throw InternalException(HERE) << "Error in reading binary file header";
    delete[] buf;
  }
  else throw InternalException(HERE) << "Memory error";
}

/* Do some administrative tasks after saving/reloading */
void BinaryStorageManager::finalize(const SaveAction caller)
{
  // We write the end marker (zero)
  off64_t objsize = 0x0000000000000000;
  writeToFile( filefd_, objsize );
  // we save the position where the size will be written (0 at first)
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  writeToFile( filefd_, objsize );
  // create file footer
  for( VisibleObjectsMap::const_iterator it = visibleObjects_.begin();
       it != visibleObjects_.end(); ++it, ++objsize )
  {
    std::cout << "id=" << it->first << " label=" << it->second.first << " visible=" << it->second.second << std::endl;
    writeToFile( filefd_, it->first ); // id
    writeToFile( filefd_, it->second.first ); // label
    writeToFile( filefd_, it->second.second ); // visibility
  }
  // we rewind to the saved position and save the actual size
  lseek64( filefd_, pos, SEEK_SET );
  writeToFile( filefd_, objsize );

  close( filefd_ );
}

/* Do some administrative tasks after saving/reloading */
void BinaryStorageManager::finalize(const LoadAction caller)
{
  // we read the size of the footer
  off64_t objsize = 0x0000000000000000;
  readFromFile( filefd_, objsize );
  // we read the footer content
  Id id;
  String label;
  Bool visibility;
  for ( ; objsize; --objsize )
  {
    readFromFile( filefd_, id );
    readFromFile( filefd_, label );
    readFromFile( filefd_, visibility );
    defineLabel( id, label );
    defineVisibility( id, visibility );
    std::cout << "Got label for id=" << id << " label=" << label << " visible=" << visibility << std::endl;
  }
  close( filefd_ );
}



/* Read and create the internal representation */
void BinaryStorageManager::read()
{
  assert(filefd_ > -1);
  // TODO : do NOT forget study version
}

/* Write the internal representation */
void BinaryStorageManager::write()
{
  assert(filefd_ > -1);
}


/*
 * This method saves the PersistentObject onto the medium
 */
void BinaryStorageManager::save(const PersistentObject & obj, const String & label, bool fromStudy)
{
  assert(filefd_ > -1);
#ifdef SAVELOAD_CHECKING
  try
  {
#endif
    if (! isSavedObject( obj.getId() ))
    {
      // we save the current offset
      off64_t before = lseek64( filefd_, 0, SEEK_CUR );
      off64_t objsize = 0x0000000000000000;
      writeToFile( filefd_, objsize );

      // we actually save the object
      obj.save(*this, label, fromStudy);

      // we compute the real object size as written in the file
      off64_t after = lseek64( filefd_, 0, SEEK_CUR );
      objsize = after - before;

      // we rewrite the actual size infront of the object in file
      lseek( filefd_, before, SEEK_SET);
      writeToFile( filefd_, objsize );
      lseek( filefd_, after, SEEK_SET);

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
void BinaryStorageManager::load(Study & study)
{
  setStudy( &study );
  assert(p_state_);
  // get first element -> node
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  off64_t objsize = 0x0000000000000000;
  readFromFile( filefd_, objsize );
  while( objsize > 0 )
  {
    BINReadObject ro = readBinaryObject();
    if (ro.p_obj_)
    {
      study.add( *(ro.p_obj_) );
    }
    // get next element
    pos = lseek64( filefd_, pos + objsize, SEEK_SET );
    readFromFile( filefd_, objsize );
  }
}




/* Methods to read object */
BinaryStorageManager::BINReadObject BinaryStorageManager::readBinaryObject()
{
  assert(p_state_);
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  String token;
  readFromFile( filefd_, token );
  if (token != "class") throw InternalException(HERE) << "File format error. Expected 'class' token at position " << pos;
  String className;
  readFromFile( filefd_, className );

  BINReadObject ro;
  ro.p_obj_ = Catalog::Get(className).build(*this);

  //         pos = lseek64( filefd_, 0, SEEK_CUR );
  //      BIN_STMGR::StudyVisible_attribute::Type tokenV;
  //      readFromFile( filefd_, tokenV );
  //      if (tokenV != BIN_STMGR::StudyVisible_attribute::Get()) throw InternalException(HERE) << "File format error. Expected '"
  //                                                                                            << std::hex
  //                                                                                            << BIN_STMGR::StudyVisible_attribute::Get()
  //                                                                                            << "' token at position " << pos;
  //      readFromFile( filefd_, ro.visibility_ );

  //      pos = lseek64( filefd_, 0, SEEK_CUR );
  //      BIN_STMGR::StudyLabel_attribute::Type tokenL;
  //      readFromFile( filefd_, tokenL );
  //      if (tokenL != BIN_STMGR::StudyLabel_attribute::Get()) throw InternalException(HERE) << "File format error. Expected '"
  //                                                                                          << std::hex
  //                                                                                          << BIN_STMGR::StudyLabel_attribute::Get()
  //                                                                                          << "' token at position " << pos;
  //      readFromFile( filefd_, ro.label_ );

  //ro.p_obj_->setVisibility( ro.visibility_ );

  return ro;
}




Pointer<Advocate> BinaryStorageManager::registerObject(const PersistentObject & obj, bool fromStudy)
{
  p_state_->id_         = obj.getId();
  p_state_->visibility_ = fromStudy;
  return new Advocate(*this, getState(), String());
}



/* Create a new empty object that will gather all saved information */
Pointer<StorageManager::InternalObject> BinaryStorageManager::createObject(const String & tag) const
{
  BinaryInternalObject * p_obj = new BinaryInternalObject;
  assert(p_obj);
  p_obj->id_ = p_state_->id_;
  p_obj->visibility_ = p_state_->visibility_;
  return p_obj;
}


/* Append an internal object to the collection of saved ones */
void BinaryStorageManager::appendObject(Pointer<InternalObject> & p_obj)
{
  assert(p_obj);
  BinaryInternalObject & obj = static_cast<BinaryInternalObject &>(*p_obj);

  doWrite( filefd_, obj.buf_, obj.size_ );

  if (obj.visibility_)
  {
    visibleObjects_[ obj.id_ ] = std::make_pair( obj.label_, obj.visibility_ );
    std::cout << "Add visible object id=" << obj.id_ << " label=" << obj.label_ << std::endl;
  }
}


void BinaryStorageManager::setVisibility(Pointer<InternalObject> & p_obj, Bool visible)
{
  assert(p_obj);
  BinaryInternalObject & obj = static_cast<BinaryInternalObject &>(*p_obj);
  obj.visibility_ = visible;
}

void BinaryStorageManager::setLabel(Pointer<InternalObject> & p_obj, const String & label)
{
  // @todo: Append this to file footer
  assert(p_obj);
  BinaryInternalObject & obj = static_cast<BinaryInternalObject &>(*p_obj);
  obj.label_ = label;
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
  st = ( OSS() << value );
}



/* fromStringConverter */

static inline
void fromStringConverter(const String & st, Bool & value)
{
  value = (st == "true" ) ? true : false;
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
}



/* getValueToConvert */

//       template <typename TAG>
//       static inline
//       String getValueToConvert(XML::Node node)
//       {
//      return XML::GetNodeValue( node );
//       }

//       template <>
//       inline
//       String getValueToConvert<BIN_STMGR::object_tag>(XML::Node node)
//       {
//      return XML::GetAttributeByName( node, BIN_STMGR::id_attribute::Get() );
//       }



/* fromNodeConverter */

//       template <typename TAG, typename _Tp>
//       static inline
//       void fromNodeConverter(XML::Node node, _Tp & value)
//       {
//      fromStringConverter( getValueToConvert<TAG>( node ), value );
//       }


//       template <>
//       inline
//       void fromNodeConverter<BIN_STMGR::numericalcomplex_tag,Complex>(XML::Node node,
//                                                                  Complex & value)
//       {
//      XML::Node node_real = XML::FindElementByName( node, BIN_STMGR::real_tag::Get() );
//      XML::Node node_imag = XML::FindElementByName( node, BIN_STMGR::imag_tag::Get() );
//      Scalar real = -1.0;
//      Scalar imag = -1.0;
//      fromStringConverter( getValueToConvert<BIN_STMGR::real_tag>( node_real ), real );
//      fromStringConverter( getValueToConvert<BIN_STMGR::imag_tag>( node_imag ), imag );
//      value = Complex( real,imag );
//       }




/************ Generic Functions ************/


/* AttributeWriter */

template <typename _Tp>
static inline
void AttributeWriter(Pointer<StorageManager::InternalObject> & p_obj,
                     const String & name,
                     _Tp value)
{
  assert(p_obj);
  //      XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  //      XML::Node node = obj.node_;
  //      assert(node);
  //      String attrValue;
  //      toStringConverter( value, attrValue );
  //      XML::SetAttribute( node, name, attrValue );
}


template <>
inline
void AttributeWriter<Complex>(Pointer<StorageManager::InternalObject> & p_obj,
                              const String & name,
                              Complex value)
{
  assert(p_obj);
  //      XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  //      XML::Node node = obj.node_;
  //      assert(node);
  //      XML::Node child = XML::NewNode( BIN_STMGR::numericalcomplex_tag::Get() );
  //      assert(child);
  //      XML::SetAttribute( child, BIN_STMGR::name_attribute::Get(), name );
  //      XML::Node real = XML::NewNode( BIN_STMGR::real_tag::Get(), OSS() << value.real() );
  //      assert(real);
  //      XML::AddChild( child, real );
  //      XML::Node imag = XML::NewNode( BIN_STMGR::imag_tag::Get(), OSS() << value.imag() );
  //      assert(imag);
  //      XML::AddChild( child, imag );
  //      XML::AddChild( node, child );
}




/* AttributeReader */

template <typename TAG, typename _Tp>
static inline
void AttributeReader(TAG tag,
                     Pointer<BinaryStorageManagerState> & p_state,
                     const String & name,
                     _Tp & value)
{
  assert(p_state);
  //      String st = XML::GetAttributeByName( p_state->current_, name );
  //      fromStringConverter( st, value );
}




/* IndexedValueWriter */

//       template <typename TAG, typename _Tp>
//       static inline
//       void IndexedValueWriter(TAG tag,
//                            Pointer<StorageManager::InternalObject> & p_obj,
//                            UnsignedInteger index,
//                            _Tp value)
//       {
//      assert(p_obj);
//      XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
//      XML::Node node = obj.node_;
//      assert(node);
//      String attrValue;
//      toStringConverter( value, attrValue );
//      XML::Node child = XML::NewNode( tag.Get(), attrValue );
//      assert(child);
//      XML::SetAttribute( child, BIN_STMGR::index_attribute::Get(), OSS() << index );
//      XML::AddChild( node, child );
//       }


//       template <>
//       inline
//       void IndexedValueWriter<BIN_STMGR::numericalcomplex_tag,Complex>(BIN_STMGR::numericalcomplex_tag tag,
//                                                                   Pointer<StorageManager::InternalObject> & p_obj,
//                                                                   UnsignedInteger index,
//                                                                   Complex value)
//       {
//      assert(p_obj);
//      XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
//      XML::Node node = obj.node_;
//      assert(node);
//      XML::Node child = XML::NewNode( BIN_STMGR::numericalcomplex_tag::Get() );
//      assert(child);
//      XML::SetAttribute( child, BIN_STMGR::index_attribute::Get(), OSS() << index );
//      XML::Node real = XML::NewNode( BIN_STMGR::real_tag::Get(), OSS() << value.real() );
//      assert(real);
//      XML::AddChild( child, real );
//      XML::Node imag = XML::NewNode( BIN_STMGR::imag_tag::Get(), OSS() << value.imag() );
//      assert(imag);
//      XML::AddChild( child, imag );
//      XML::AddChild( node, child );
//       }





/* IndexedValueReader */

//       template <typename TAG, typename _Tp>
//       static inline
//       void IndexedValueReader(TAG tag,
//                            Pointer<StorageManager::InternalObject> & p_obj,
//                            UnsignedInteger index,
//                            _Tp & value)
//       {
//      assert(p_obj);
//      BinaryStorageManagerState & state = dynamic_cast<BinaryStorageManagerState &>(*p_obj);

//      XML::Node node;
//      while ( node = XML::FindNextElementByName( state.current_, tag.Get() ) ) {
//        UnsignedInteger idx = 0;
//        fromStringConverter( XML::GetAttributeByName(node, BIN_STMGR::index_attribute::Get()), idx );
//        state.next();
//        if (idx == index) {
//          fromNodeConverter<TAG,_Tp>( node, value );
//          return;
//        }
//      }

//      LOGWARN( OSS() << "Failed when reading indexed value. Expected tag '" << tag.Get()
//                        << "'. Got '" << XML::GetNodeName(state.current_) << "'" );
//       }



/* NamedObjectWriter */

//       template <typename TAG, typename _Tp>
//       static inline
//       void NamedObjectWriter(TAG tag,
//                           Pointer<StorageManager::InternalObject> & p_obj,
//                           const String & name,
//                           _Tp & value)
//       {
//      assert(p_obj);
//      XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
//      XML::Node node = obj.node_;
//      assert(node);
//      XML::Node child = XML::NewNode( tag.Get() );
//      assert(child);
//      XML::SetAttribute( child, BIN_STMGR::member_attribute::Get(), name);
//      XML::SetAttribute( child, BIN_STMGR::id_attribute::Get(), OSS() << value.getId() );
//      XML::AddChild( node, child );
//       }



/* NamedObjectReader */

//       template <typename TAG, typename _Tp>
//       static inline
//       int NamedObjectReader(TAG tag,
//                          Pointer<StorageManager::InternalObject> & p_obj,
//                          const String & name,
//                          _Tp & value)
//       {
//      assert(p_obj);
//      BinaryStorageManagerState state = dynamic_cast<BinaryStorageManagerState &>(*p_obj);

//      XML::Node node = XML::FindElementByName( state.current_, tag.Get() );
//      while ( node ) {
//        String nm;
//        fromStringConverter( XML::GetAttributeByName(node, BIN_STMGR::member_attribute::Get()), nm );
//        if (nm == name) {
//          fromNodeConverter<TAG,_Tp>( node, value );
//          return 1;
//        }
//        node = XML::FindNextElementByName( node, tag.Get() );
//      }

//      return 0;
//       }





/************ Type = Bool ************/

/* Add an attribute to an internal object */
void BinaryStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                        const String & name,
                                        Bool value)
{
  writeToFile( filefd_, name );
  writeToFile( filefd_, value );
}

/* Read an attribute */
void BinaryStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
    const String & name,
    Bool & value)
{
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  String token;
  readFromFile( filefd_, token );
  if (token == name)
    readFromFile( filefd_, value );
  else lseek64( filefd_, pos, SEEK_SET );
}

/* Add an indexed value to an internal object */
void BinaryStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Bool value)
{
  writeToFile( filefd_, value );
}

/* Read an indexed value */
void BinaryStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Bool & value)
{
  readFromFile( filefd_, value );
}




/************ Type = UnsignedInteger ************/


/* Add an attribute to an internal object */
void BinaryStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                        const String & name,
                                        UnsignedInteger value)
{
  writeToFile( filefd_, name );
  writeToFile( filefd_, value );
}

/* Read an attribute */
void BinaryStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
    const String & name,
    UnsignedInteger & value)
{
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  String token;
  readFromFile( filefd_, token );
  if (token == name)
    readFromFile( filefd_, value );
  else lseek64( filefd_, pos, SEEK_SET );
}

/* Add an indexed value to an internal object */
void BinaryStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    UnsignedInteger value)
{
  writeToFile( filefd_, value );
}

/* Read an indexed value */
void BinaryStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    UnsignedInteger & value)
{
  readFromFile( filefd_, value );
}




/************ Type = Scalar ************/


/* Add an attribute to an internal object */
void BinaryStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                        const String & name,
                                        Scalar value)
{
  writeToFile( filefd_, name );
  writeToFile( filefd_, value );
}

/* Read an attribute */
void BinaryStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
    const String & name,
    Scalar & value)
{
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  String token;
  readFromFile( filefd_, token );
  if (token == name)
    readFromFile( filefd_, value );
  else lseek64( filefd_, pos, SEEK_SET );
}

/* Add an indexed value to an internal object */
void BinaryStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Scalar value)
{
  writeToFile( filefd_, value );
}

/* Read an indexed value */
void BinaryStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Scalar & value)
{
  readFromFile( filefd_, value );
}




/************ Type = Complex ************/


/* Add an attribute to an internal object */
void BinaryStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                        const String & name,
                                        Complex value)
{
  writeToFile( filefd_, name );
  writeToFile( filefd_, value );
}

/* Read an attribute */
void BinaryStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
    const String & name,
    Complex & value)
{
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  String token;
  readFromFile( filefd_, token );
  if (token == name)
    readFromFile( filefd_, value );
  else lseek64( filefd_, pos, SEEK_SET );
}

/* Add an indexed value to an internal object */
void BinaryStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Complex value)
{
  writeToFile( filefd_, value );
}

/* Read an indexed value */
void BinaryStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Complex & value)
{
  readFromFile( filefd_, value );
}




/************ Type = String ************/


/* Add an attribute to an internal object */
void BinaryStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                        const String & name,
                                        const String & value)
{
  writeToFile( filefd_, name );
  writeToFile( filefd_, value );
}

/* Read an attribute */
void BinaryStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
    const String & name,
    String & value)
{
  off64_t pos = lseek64( filefd_, 0, SEEK_CUR );
  String token;
  readFromFile( filefd_, token );
  if (token == name)
    readFromFile( filefd_, value );
  else lseek64( filefd_, pos, SEEK_SET );
}

/* Add an indexed value to an internal object */
void BinaryStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    const String & value)
{
  writeToFile( filefd_, value );
}

/* Read an indexed value */
void BinaryStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    String & value)
{
  readFromFile( filefd_, value );
}




/************ Type = InterfaceObject ************/


/* Add an attribute to an internal object */
void BinaryStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                        const String & name,
                                        const InterfaceObject & value)
{
  value.save( *this );
  assert(p_obj);
  writeToFile( filefd_, BIN_STMGR::member_attribute::Get() );
  writeToFile( filefd_, name );
  writeToFile( filefd_, BIN_STMGR::id_attribute::Get() );
  writeToFile( filefd_, value.getId() );
}

/* Read an attribute */
void BinaryStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
    const String & name,
    InterfaceObject & value)
{
  //      Id shadowedId;
  //      if (NamedObjectReader( BIN_STMGR::object_tag(), p_obj, name, shadowedId )) {
  //        if (! getStudy()->hasObject(shadowedId))
  //          throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
  //                                                << " in file '" << getFileName()
  //                                                << "' referenced before used";
  //        value.setImplementationAsPersistentObject(getStudy()->getObject(shadowedId));
  //      }
}

/* Add an indexed value to an internal object */
void BinaryStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    const InterfaceObject & value)
{
  value.save( *this );
  assert(p_obj);
  writeToFile( filefd_, BIN_STMGR::index_attribute::Get() );
  writeToFile( filefd_, index );
  writeToFile( filefd_, BIN_STMGR::id_attribute::Get() );
  writeToFile( filefd_, value.getId() );
}

/* Read an indexed value */
void BinaryStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    InterfaceObject & value)
{
  //      Id shadowedId;
  //      IndexedValueReader( BIN_STMGR::object_tag(), p_obj, index, shadowedId );
  //      if (! getStudy()->hasObject(shadowedId))
  //        throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
  //                                              << " in file '" << getFileName()
  //                                              << "' referenced before used";
  //      value.setImplementationAsPersistentObject(getStudy()->getObject(shadowedId));
}




/************ Type = PersistentObject ************/


/* Add an attribute to an internal object */
void BinaryStorageManager::addAttribute(Pointer<InternalObject> & p_obj,
                                        const String & name,
                                        const PersistentObject & value)
{
  value.save( *this );
  assert(p_obj);
  writeToFile( filefd_, BIN_STMGR::member_attribute::Get() );
  writeToFile( filefd_, name );
  writeToFile( filefd_, BIN_STMGR::id_attribute::Get() );
  writeToFile( filefd_, value.getId() );
}

/* Read an attribute */
void BinaryStorageManager::readAttribute(Pointer<InternalObject> & p_obj,
    const String & name,
    PersistentObject & value)
{
  //      Id shadowedId;
  //      if (NamedObjectReader( BIN_STMGR::object_tag(), p_obj, name, shadowedId )) {
  //        if (! getStudy()->hasObject(shadowedId))
  //          throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
  //                                                << " in file '" << getFileName()
  //                                                << "' referenced before used";
  //        Study * p_study = getStudy();
  //        Pointer<PersistentObject> p_po = p_study->getObject(shadowedId);
  //        Catalog::GetInstance().get(value.getClassName()).assign(value, *p_po);
  //      }
}

/* Add an indexed value to an internal object */
void BinaryStorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    const PersistentObject & value)
{
  value.save( *this );
  assert(p_obj);
  writeToFile( filefd_, BIN_STMGR::index_attribute::Get() );
  writeToFile( filefd_, index );
  writeToFile( filefd_, BIN_STMGR::id_attribute::Get() );
  writeToFile( filefd_, value.getId() );
}

/* Read an indexed value */
void BinaryStorageManager::readIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    PersistentObject & value)
{
  //      Id shadowedId;
  //      IndexedValueReader( BIN_STMGR::object_tag(), p_obj, index, shadowedId );
  //      if (! getStudy()->hasObject(shadowedId))
  //        throw StudyFileParsingException(HERE) << "Element of id = " << shadowedId
  //                                              << " in file '" << getFileName()
  //                                              << "' referenced before used";
  //      Study * p_study = getStudy();
  //      Pointer<PersistentObject> p_po = p_study->getObject(shadowedId);
  //      Catalog::GetInstance().get(value.getClassName()).assign(value, *p_po);
}


END_NAMESPACE_OPENTURNS
