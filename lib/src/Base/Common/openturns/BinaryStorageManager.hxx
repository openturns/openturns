//                                               -*- C++ -*-
/**
 *  @brief BinaryStorageManager provides an interface for different storage classes
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_BINARYSTORAGEMANAGER_HXX
#define OPENTURNS_BINARYSTORAGEMANAGER_HXX

#include <iostream>              // for std::ostream
#include <stdio.h>               // for std::ostream
#include <map>                   // for std::map
#include "openturns/OTprivate.hxx"
#include "openturns/StorageManager.hxx"


BEGIN_NAMESPACE_OPENTURNS




/* Forward declaration of PersistentObject */
class PersistentObject;

/* Forward declaration of BinaryStorageManagerState */
class BinaryStorageManagerState;

/**
 * @class BinaryStorageManager
 * @brief Implements a storage manager that drives a XML file
 * @see StorageManager
 * @see Study
 */

class OT_API BinaryStorageManager
  : public StorageManager
{
  CLASSNAME

  class VersionList
  {
    std::vector<UnsignedInteger> versions_;
  public:
    VersionList() : versions_()
    {
      versions_.push_back(1);
    }

    Bool contains(UnsignedInteger v) const
    {
      return find(versions_.begin(), versions_.end(), v) != versions_.end();
    }
  };

  static const VersionList SupportedVersions;


public:


  /**
   * Constructor for a XML StorageManager that reads/writes
   * to a XML file
   */
  explicit BinaryStorageManager(const FileName & filename);

  /**
   * Virtual constructor
   */
  virtual BinaryStorageManager * clone() const;

  /** @copydoc StorageManager::__repr__() const */
  virtual String __repr__() const;

  /** Get the name of the file containing the study */
  String getFileName() const;

  /** Set the name of the file containing the study */
  void setFileName(const String & fileName);

  /**
   * This method saves the PersistentObject onto the medium
   */
  virtual void save(const PersistentObject & obj, const String & label, bool fromStudy = false);

  /**
   * This method reloads the PersistentObject from the medium
   */
  virtual void load(Study & study);



  /** Do some administrative tasks before saving/reloading
   * @internal
   */
  virtual void initialize(const SaveAction caller);
  virtual void initialize(const LoadAction caller);

  /** Do some administrative tasks after saving/reloading
   * @internal
   */
  virtual void finalize(const SaveAction caller);
  virtual void finalize(const LoadAction caller);

  /** Read and create the internal representation
   * @internal
   */
  virtual void read();

  /** Write the internal representation
   * @internal
   */
  virtual void write();



#ifndef SWIG
  virtual
  Pointer<Advocate> registerObject(const PersistentObject & obj, bool fromStudy = false);

  /* Create a new empty object that will gather all saved information
   * @internal
   */
  virtual Pointer<InternalObject> createObject(const String & tag) const;

  /* Append an internal object to the collection of saved ones
   * @internal
   */
  virtual void appendObject(Pointer<InternalObject> & p_obj);

  /* Set the visibility attribute of the object
   * @internal
   */
  virtual void setVisibility(Pointer<InternalObject> & p_obj, Bool visible);

  /* Set the label associated with the object
   * @internal
   */
  virtual void setLabel(Pointer<InternalObject> & p_obj, const String & label);


  /* Add an attribute to an internal object
   * @internal
   */
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Bool value);
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, UnsignedInteger value);
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Scalar value);
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Complex value);
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, const String & value);
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, const InterfaceObject & value);
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, const PersistentObject & value);

  /* Add an indexed value to an internal object
   * @internal
   */
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Bool value);
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger value);
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar value);
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Complex value);
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, const String & value);
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, const InterfaceObject & value);
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, const PersistentObject & value);

  /* Read an attribute
   * @internal
   */
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Bool & value);
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name, UnsignedInteger & value);
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Scalar & value);
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Complex & value);
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name, String & value);
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name,  InterfaceObject & value);
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name, PersistentObject & value);

  /* Read an indexed value
   * @internal
   */
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Bool & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Complex & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, String & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, InterfaceObject & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, PersistentObject & value);
#endif


protected:

  /** Query the manager if the version is correct */
  virtual Bool canManageVersion(UnsignedInteger version) const;

  /** Return the current state of the storage manager (for those having one) */
  virtual const StorageManager::InternalObject & getState() const;



  struct BINReadObject
  {
    Pointer<PersistentObject> p_obj_;
    BINReadObject() : p_obj_(0) {}
  };

  /** Methods to read object */
  BINReadObject readBinaryObject();

  /** Erase the map of visible objects */
  void cleanVisibleObjects();

private:

  /** The file we read from/write to */
  FileName fileName_;

  /** The internal state of the manager */
  Pointer<BinaryStorageManagerState> p_state_;

  /** The internal document */
  int filefd_;

  /** Map of visible objects */
  typedef std::map<Id, std::pair<String, Bool> > VisibleObjectsMap;
  VisibleObjectsMap visibleObjects_;



}; /* class BinaryStorageManager */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BINARYSTORAGEMANAGER_HXX */
