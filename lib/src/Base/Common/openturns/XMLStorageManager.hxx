//                                               -*- C++ -*-
/**
 *  @brief XMLStorageManager provides an interface for different storage classes
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
#ifndef OPENTURNS_XMLSTORAGEMANAGER_HXX
#define OPENTURNS_XMLSTORAGEMANAGER_HXX

#include <iostream>              // for std::ostream
#include <stack>                 // for std::stack
#include <algorithm>             // for std::find
#include "openturns/OTprivate.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/ResourceMap.hxx"


BEGIN_NAMESPACE_OPENTURNS

/* Forward declaration of XMLDoc */
class XMLDoc;




/* Forward declaration of PersistentObject */
class PersistentObject;

/* Forward declaration of XMLStorageManagerState */
struct XMLStorageManagerState;

class OT_API VersionList
{
  std::vector<UnsignedInteger> versions_;
public:
  VersionList() : versions_()
  {
    versions_.push_back(1);
  }

  Bool contains(UnsignedInteger v) const
  {
    return std::find(versions_.begin(), versions_.end(), v) != versions_.end();
  }
};

/**
 * @class XMLStorageManager
 * @brief Implements a storage manager that drives a XML file
 * @see StorageManager
 * @see Study
 */

class OT_API XMLStorageManager
  : public StorageManager
{
  CLASSNAME



  static const VersionList SupportedVersions;

public:

  /**
   * The numerical precision used to write out scientific numbers
   */
  static const int Precision_;

  /**
   * Constructor for a XML StorageManager that reads/writes
   * to a XML file
   */
  explicit XMLStorageManager(const FileName & filename,
                             const UnsignedInteger compressionLevel = ResourceMap::GetAsUnsignedInteger("XMLStorageManager-DefaultCompressionLevel"));

  /**
   * Virtual constructor
   */
  XMLStorageManager * clone() const override;

  /** @copydoc StorageManager::__repr__() const */
  String __repr__() const override;

  /** Get the name of the file containing the study */
  String getFileName() const;

  /** Set the name of the file containing the study */
  void setFileName(const String & fileName);

  /**
   * This method saves the PersistentObject onto the medium
   */
  void save(const PersistentObject & obj, const String & label, bool fromStudy = false) override;

  /**
   * This method reloads the PersistentObject from the medium
   */
  void load(Study & study) override;



  /** Do some administrative tasks before saving/reloading
   * @internal
   */
  void initialize(const SaveAction caller) override;
  void initialize(const LoadAction caller) override;

  /** Do some administrative tasks after saving/reloading
   * @internal
   */
  void finalize(const SaveAction caller) override;
  void finalize(const LoadAction caller) override;

  /** Read and create the internal representation
   * @internal
   */
  void read() override;

  /** Write the internal representation
   * @internal
   */
  void write() override;



#ifndef SWIG
  /* Create a new empty object that will gather all saved information
   * @internal
   */
  Pointer<InternalObject> createObject(const String & tag) const override;

  /* Append an internal object to the collection of saved ones
   * @internal
   */
  void appendObject(Pointer<InternalObject> & p_obj) override;

  /* Set the visibility attribute of the object
   * @internal
   */
  void setVisibility(Pointer<InternalObject> & p_obj, Bool visible) override;

  /* Set the label associated with the object
   * @internal
   */
  void setLabel(Pointer<InternalObject> & p_obj, const String & label) override;

  /* Returns true if the internal object has an attribute
   */
  Bool hasAttribute(Pointer<InternalObject> & p_obj, const String & name) override;

  /* Add an attribute to an internal object
   * @internal
   */
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Bool value) override;
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, UnsignedInteger value) override;
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Unsigned64BitsInteger value) override;
#endif
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Scalar value) override;
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Complex value) override;
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, const String & value) override;
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, const InterfaceObject & value) override;
  void addAttribute(Pointer<InternalObject> & p_obj, const String & name, const PersistentObject & value) override;

  /* Add an indexed value to an internal object
   * @internal
   */
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Bool value) override;
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger value) override;
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Unsigned64BitsInteger value) override;
#endif
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar value) override;
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Complex value) override;
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, const String & value) override;
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, const InterfaceObject & value) override;
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, const PersistentObject & value) override;

  /* Read an attribute
   * @internal
   */
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Bool & value) override;
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name, UnsignedInteger & value) override;
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Unsigned64BitsInteger & value) override;
#endif
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Scalar & value) override;
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Complex & value) override;
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name, String & value) override;
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name,  InterfaceObject & value) override;
  void readAttribute(Pointer<InternalObject> & p_obj, const String & name, PersistentObject & value) override;

  /* Read an indexed value
   * @internal
   */
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Bool & value) override;
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger & value) override;
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Unsigned64BitsInteger & value) override;
#endif
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar & value) override;
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Complex & value) override;
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, String & value) override;
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, InterfaceObject & value) override;
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, PersistentObject & value) override;
#endif


protected:

  /** Query the manager if the version is correct */
  Bool canManageVersion(UnsignedInteger version) const override;

  /** Return the current state of the storage manager (for those having one) */
  const StorageManager::InternalObject & getState() const override;



private:

  struct XMLReadObject
  {
    Pointer<PersistentObject> p_obj_;
    String visibility_;
    String label_;
    XMLReadObject() : p_obj_(0), visibility_(), label_() {}
  };

  /** The file we read from/write to */
  FileName fileName_;

  /** The internal state of the manager */
  Pointer<XMLStorageManagerState> p_state_;

  /** The internal document */
  Pointer<XMLDoc> p_document_;

  /** Methods to read DOM elements */
  XMLReadObject readDOMElement();

  /** The compression level for the XML export */
  UnsignedInteger compressionLevel_;

}; /* class XMLStorageManager */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_XMLSTORAGEMANAGER_HXX */
