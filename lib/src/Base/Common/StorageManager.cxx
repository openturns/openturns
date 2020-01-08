//                                               -*- C++ -*-
/**
 *  @brief StorageManager provides an interface for different storage classes
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/InterfaceObject.hxx"
#include "openturns/Study.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Log.hxx"
#include "openturns/OTconfig.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(StorageManager)


Pointer<Advocate> StorageManager::registerObject(const PersistentObject &, bool )
{
  return new Advocate(*this, getState(), "object");
}

Advocate StorageManager::readObject()
{
  return Advocate(*this, getState());
}


/* Return the current state of the storage manager (for those having one) */
const StorageManager::InternalObject & StorageManager::getState() const
{
  static const InternalObject State = StorageManager::InternalObject();
  return State;
}


/* Default constructor */
StorageManager::StorageManager(UnsignedInteger defaultVersion)
  : Object(),
    p_study_(0),
    defaultVersion_(defaultVersion),
    version_(defaultVersion),
    savedObjects_()
{
  // Nothing to do
}

/*
 * Virtual constructor
 */
StorageManager * StorageManager::clone() const
{
  return new StorageManager(*this);
}


/* String converter */
String StorageManager::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


/* Do some administrative tasks before saving/reloading */
void StorageManager::initialize(const SaveAction)
{
  // Nothing to do
}

/* Do some administrative tasks before saving/reloading */
void StorageManager::initialize(const LoadAction)
{
  // Nothing to do
}

/* Do some administrative tasks after saving/reloading */
void StorageManager::finalize(const SaveAction)
{
  // Nothing to do
}


/* Do some administrative tasks after saving/reloading */
void StorageManager::finalize(const LoadAction)
{
  // Nothing to do
}


/* Read and create the internal representation */
void StorageManager::read()
{
  // Nothing to do
}

/* Write the internal representation */
void StorageManager::write()
{
  // Nothing to do
}



/* Create a new empty object that will gather all saved information */
Pointer<StorageManager::InternalObject> StorageManager::createObject(const String & ) const
{
  return new InternalObject;
}


/* Append an internal object to the collection of saved ones */
void StorageManager::appendObject(Pointer<InternalObject> & )
{
  // Nothing to do
}

Bool StorageManager::hasAttribute(Pointer<InternalObject> &, const String& )
{
  throw NotYetImplementedException(HERE) << "In StorageManager::hasAttribute(Pointer<InternalObject> & p_obj, const String& name)";
}

/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, Bool )
{
  // Nothing to do
}


/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, UnsignedInteger )
{
  // Nothing to do
}

#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, Unsigned64BitsInteger )
{
  // Nothing to do
}
#endif

/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, Scalar )
{
  // Nothing to do
}


/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, Complex )
{
  // Nothing to do
}


/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, const String & )
{
  // Nothing to do
}


/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, const InterfaceObject & value)
{
  // Nothing to do
  value.save( *this );
}


/* Add an attribute to an internal object */
void StorageManager::addAttribute(Pointer<InternalObject> &, const String &, const PersistentObject & value)
{
  // Nothing to do
  value.save( *this );
}


/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Bool )
{
  // Nothing to do
}

/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, UnsignedInteger )
{
  // Nothing to do
}

#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Unsigned64BitsInteger )
{
  // Nothing to do
}
#endif

/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Scalar )
{
  // Nothing to do
}

/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Complex )
{
  // Nothing to do
}

/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, const String & )
{
  // Nothing to do
}

/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, const InterfaceObject & value)
{
  // Nothing to do
  value.save( *this );
}

/* Add an indexed value to an internal object */
void StorageManager::addIndexedValue(Pointer<InternalObject> &, UnsignedInteger, const PersistentObject & value)
{
  // Nothing to do
  value.save( *this );
}





/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &, Bool & )
{
  // Nothing to do
}

/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &, UnsignedInteger & )
{
  // Nothing to do
}

#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &, Unsigned64BitsInteger & )
{
  // Nothing to do
}
#endif

/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &, Scalar & )
{
  // Nothing to do
}

/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &, Complex & )
{
  // Nothing to do
}

/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &, String & )
{
  // Nothing to do
}

/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &,  InterfaceObject & )
{
  // Nothing to do
}

/* Read an attribute */
void StorageManager::readAttribute(Pointer<InternalObject> &, const String &, PersistentObject & )
{
  // Nothing to do
}



/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Bool & )
{
  // Nothing to do
}

/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, UnsignedInteger & )
{
  // Nothing to do
}

#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Unsigned64BitsInteger & )
{
  // Nothing to do
}
#endif

/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Scalar & )
{
  // Nothing to do
}

/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, Complex & )
{
  // Nothing to do
}

/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, String & )
{
  // Nothing to do
}

/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, InterfaceObject & )
{
  // Nothing to do
}

/* Read an indexed value */
void StorageManager::readIndexedValue(Pointer<InternalObject> &, UnsignedInteger, PersistentObject & )
{
  // Nothing to do
}

/* Returns true if an object is already saved */
Bool StorageManager::isSavedObject(Id id) const
{
  return savedObjects_[id];
}

/* Tells that an object is saved */
void StorageManager::markObjectAsSaved(Id id) const
{
  savedObjects_[id] = true;
}

/* Refresh the status of saved objects */
void StorageManager::cleanSavedObjects() const
{
  savedObjects_.erase(savedObjects_.begin(), savedObjects_.end());
}


/*
 * This method saves the PersistentObject onto the medium
 *
 * @todo implement StorageManager::save method
 */
void StorageManager::save(const PersistentObject & obj, const String & label, bool fromStudy)
{
#ifdef SAVELOAD_CHECKING
  try
  {
#endif
    obj.save(*this, label, fromStudy);
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
 *
 * @todo implement StorageManager::load method
 */
void StorageManager::load(Study & study)
{
  setStudy( &study );
}


/* Study accessor */
Study * StorageManager::getStudy() const
{
  return p_study_;
}

void StorageManager::setStudy(Study * p_study)
{
  p_study_ = p_study;
}


/* Returns the version of the study that the manager can read/write */
UnsignedInteger StorageManager::getDefaultStudyVersion() const
{
  return defaultVersion_;
}

/* Query the manager if the version is correct */
Bool StorageManager::canManageVersion(UnsignedInteger version) const
{
  return (version == defaultVersion_);
}

/* Study version accessor */
UnsignedInteger StorageManager::getStudyVersion() const
{
  return version_;
}

void StorageManager::setStudyVersion(UnsignedInteger version)
{
  if (canManageVersion(version)) version_ = version;
  else throw InvalidArgumentException(HERE) << "Study version " << version << " is NOT supported";
}


void StorageManager::setVisibility(Pointer<InternalObject> &, Bool )
{
  // Nothing to do
}

void StorageManager::setLabel(Pointer<InternalObject> &, const String & )
{
  // Nothing to do
}

/* Label element in study */
void StorageManager::defineLabel(Id id, const String & label)
{
  p_study_->defineLabel( id, label );
}

/* Define element visibility in study */
void StorageManager::defineVisibility(Id id, Bool visible)
{
  p_study_->defineVisibility( id, visible );
}

END_NAMESPACE_OPENTURNS
