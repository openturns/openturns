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
#ifndef OPENTURNS_STORAGEMANAGER_HXX
#define OPENTURNS_STORAGEMANAGER_HXX

#include <iostream>              // for std::ostream
#include <vector>                // for std::vector
#include <set>                   // for std::set
#include <map>                   // for std::map
#include <cstddef>               // for ptrdiff_t
#include "openturns/Object.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Forward declaration of Advocate */
class Advocate;



/* Forward declaration of PersistentObject */
class PersistentObject;

/* Forward declaration of InterfaceObject */
class InterfaceObject;

/* Forward declaration of Study */
class Study;


/** Helper classes for StorageManager
 *
 * Allow correct selection of method
 */
struct OT_API SaveAction {};
struct OT_API LoadAction {};

/**
 * @class StorageManager
 * @brief Abstract class defining the way OpenTURNS' objects are saved and reloaded
 *
 * StorageManager defines a common interface implemented by
 * different classes that must store and reload object onto
 * a persistent medium
 * @see Study
 */

class OT_API StorageManager
  : public Object
{
  CLASSNAME
public:

#ifndef SWIG

  struct InternalObject
  {
    virtual ~InternalObject() throw() {}
    virtual InternalObject * clone() const
    {
      return new InternalObject(*this);
    }
    virtual void first() {}
    virtual void next() {}
  };

  virtual
  Pointer<Advocate> registerObject(const PersistentObject & obj, bool fromStudy = false);

  virtual
  Advocate readObject();
#endif

  /**
   * Default constructor
   *
   * The default constructor allows the object to be
   * stored in STL containers like vector or map.
   * It takes an optional argument interpreted as the
   * name of the object in a user point of view. This
   * name is never used in another way than for user
   * information. This name has no meaning to the platform.
   */
  explicit StorageManager(UnsignedInteger defaultVersion = 1);

  /**
   * Virtual constructor
   */
  virtual StorageManager * clone() const;

  /** @copydoc Object::__repr__() const */
  String __repr__() const override;

  /**
   * This method saves the PersistentObject onto the medium
   * @param obj The object to be saved
   * @param fromStudy Tell if the object was explicitely put in the study or not
   */
  virtual void save(const PersistentObject & obj, const String & label, bool fromStudy = false);

  /**
   * This method reloads the whole study from the medium
   * @param study The study to reload objects from
   */
  virtual void load(Study & study);

  /** Study accessor */
  Study * getStudy() const;

  /** Defines the study bound to the manager */
  void setStudy(Study * p_study);

  /** Returns the version of the study that the manager can read/write */
  UnsignedInteger getDefaultStudyVersion() const;

  /** Study version accessor */
  UnsignedInteger getStudyVersion() const;

  /** Study version accessor */
  void setStudyVersion(UnsignedInteger version);


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

  /* Returns true if the internal object has an attribute
   */
  virtual Bool hasAttribute(Pointer<InternalObject> & p_obj, const String & name);

  /* Add an attribute to an internal object
   * @internal
   */
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Bool value);
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, UnsignedInteger value);
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  virtual void addAttribute(Pointer<InternalObject> & p_obj, const String & name, Unsigned64BitsInteger value);
#endif
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
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  virtual void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Unsigned64BitsInteger value);
#endif
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
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  virtual void readAttribute(Pointer<InternalObject> & p_obj, const String & name, Unsigned64BitsInteger & value);
#endif
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
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Unsigned64BitsInteger & value);
#endif
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Complex & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, String & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, InterfaceObject & value);
  virtual void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, PersistentObject & value);
#endif

  /** Returns true if an object is already saved
   * @internal
   */
  Bool isSavedObject(Id id) const;

  /* Tells that an object is saved
   * @internal
   */
  void markObjectAsSaved(Id id) const;

protected:

  /** Refresh the status of saved objects */
  void cleanSavedObjects() const;

  /** Query the manager if the version is correct */
  virtual Bool canManageVersion(UnsignedInteger version) const;

  /** Return the current state of the storage manager (for those having one) */
  virtual const InternalObject & getState() const;

  /** Label element in study */
  void defineLabel(Id id, const String & label);

  /** Define element visibility in study */
  void defineVisibility(Id id, Bool visible);



private:

  /** A reference to the current study */
  Study * p_study_;

  /** The default version of the study */
  const UnsignedInteger defaultVersion_;

  /** The version of the study actually used */
  UnsignedInteger version_;

  /** The map that tells if objects wera already saved */
  mutable std::map<Id, Bool> savedObjects_;

}; /* class StorageManager */




#ifndef SWIG

class OT_API Advocate
{
public:


  Advocate(StorageManager & mgr, const StorageManager::InternalObject & state)
    : mgr_(mgr),
      p_state_(state.clone()),
      p_obj_(0),
      studyVisible_(false),
      label_(),
      savedAttributes_()
  {}

  Advocate(StorageManager & mgr, const StorageManager::InternalObject & state, const String & tag)
    : mgr_(mgr),
      p_state_(state.clone()),
      p_obj_(mgr.createObject(tag)),
      studyVisible_(false),
      label_(),
      savedAttributes_()
  {}

  Advocate(const Advocate & other)
    : mgr_(other.mgr_),
      p_state_(other.p_state_->clone()),
      p_obj_(other.p_obj_),
      studyVisible_(other.studyVisible_),
      label_(other.label_),
      savedAttributes_(other.savedAttributes_)
  {}

  void saveObject()
  {
    mgr_.setVisibility( p_obj_, studyVisible_ );
    mgr_.setLabel( p_obj_, label_ );
    mgr_.appendObject( p_obj_ );
  }

  void setLabel(const String & label)
  {
    label_ = label;
    studyVisible_ = true;
  }

  void firstValue()
  {
    p_state_->first();
  }

  void nextValue()
  {
    p_state_->next();
  }

  Bool hasAttribute(const String & attributeName)
  {
    return mgr_.hasAttribute( p_state_, attributeName );
  }

  template <typename _Tp>
  inline
  Advocate & saveAttribute(const String & attributeName, const _Tp & attributeValue)
  {
    checkSavedAttribute( attributeName );
    mgr_.addAttribute( p_obj_, attributeName, attributeValue );
    return *this;
  }

  template <typename _Tp>
  inline
  Advocate & saveIndexedValue(long attributeIndex, const _Tp & attributeValue)
  {
    mgr_.addIndexedValue( p_obj_, attributeIndex, attributeValue );
    return *this;
  }

  template <typename _Tp>
  inline
  Advocate & loadAttribute(const String & attributeName, _Tp & attributeValue)
  {
    mgr_.readAttribute( p_state_, attributeName, attributeValue );
    return *this;
  }

  template <typename _Tp>
  inline
  Advocate & loadIndexedValue(long attributeIndex, _Tp & attributeValue)
  {
    mgr_.readIndexedValue( p_state_, attributeIndex, attributeValue );
    return *this;
  }

private:

  StorageManager & mgr_;
  Pointer<StorageManager::InternalObject> p_state_;
  Pointer<StorageManager::InternalObject> p_obj_;
  Bool studyVisible_;
  String label_;
  std::set<String> savedAttributes_;

  void checkSavedAttribute(const String & attributeName)
  {
#ifdef SAVELOAD_CHECKING
    if (savedAttributes_.insert(attributeName).second == false)
      throw InternalException(HERE) << "Attribute '" << attributeName << "' is already saved. Report bug";
#else
    (void)attributeName;
#endif
  }
}; /* end class Advocate */


template <typename _Tp>
struct PtrTraits
{
  typedef _Tp       value_type;
  typedef ptrdiff_t difference_type;
  typedef _Tp *     pointer;
  typedef _Tp &     reference;
};


template <typename _Tp>
class AdvocateIterator
{
  Advocate advocate_;
  long index_;
  Bool first_;
public:
  typedef typename PtrTraits<_Tp>::value_type      value_type;
  typedef typename PtrTraits<_Tp>::difference_type difference_type;
  typedef typename PtrTraits<_Tp>::pointer         pointer;
  typedef typename PtrTraits<_Tp>::reference       reference;
  typedef std::output_iterator_tag                 iterator_category;

  explicit AdvocateIterator(Advocate & advocate) : advocate_(advocate), index_(0), first_(true) {}

  AdvocateIterator(const AdvocateIterator & other)
    : advocate_(other.advocate_)
    , index_(other.index_)
    , first_(other.first_)
  {
    // Nothing to do
  }

  AdvocateIterator &
  operator = (const _Tp & value)
  {
    advocate_.saveIndexedValue( index_, value );
    ++index_;
    return *this;
  }

  AdvocateIterator &
  operator = (const AdvocateIterator & other)
  {
    if (*this != other)
    {
      // Copy-assignment does not make sense on different advocate_
      if (&advocate_ != &other.advocate_)
        throw InternalException(HERE) << "Wrong advocates in copy assignment. Report bug";
      index_ = other.index_;
      first_ = other.first_;
    }
    return *this;
  }

  _Tp operator () ()
  {
    _Tp value;
    if (first_)
    {
      advocate_.firstValue();
      first_ = false;
    }
    advocate_.loadIndexedValue( index_, value );
    advocate_.nextValue();
    ++index_;
    return value;
  }
  AdvocateIterator & operator *  ()
  {
    return *this;
  }
  AdvocateIterator & operator ++ ()
  {
    return *this;
  }
  AdvocateIterator operator ++ (int)
  {
    return *this;
  }
}; /* end class AdvocateIterator */


#endif /* SWIG */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STORAGEMANAGER_HXX */
