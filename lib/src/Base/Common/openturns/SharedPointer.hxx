//                                               -*- C++ -*-
/**
 *  @brief The class SharedPointer implements a shared pointer
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
#ifndef OPENTURNS_SHAREDPOINTER_HXX
#define OPENTURNS_SHAREDPOINTER_HXX

#include "openturns/OTconfig.hxx"
#include <algorithm>        // for std::swap
#include <iostream>
#include "openturns/AtomicFunctions.hxx"
#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

#ifndef SWIG

/*
 * class GenericCounterImplementation
 *
 * brief This class is in charge of the reference counter for the shared pointer.
 *
 * This class implements the base mechanisms of reference counting.
 * It keeps track of the number of references to an hypothetic pointee (not defined here
 * because the class is abstract).
 *
 * This class, its interface and its implementation are rewrittings of the BOOST shared_ptr
 * template class. We forgived using BOOST because of some pain in compiling OpenTURNS on
 * some architectures (with the help of Debian autobuilder) but the implementation was
 * so smart we decided to reimplement ours (with lesser dependencies) by cloning the interface
 * and part of the implementation of the BOOST one. Many thanks to BOOST for its work.
 */

class OT_API GenericCounterImplementation
{
  /* The number of references to the pointee */
  AtomicInt use_count_;

public:

  /*
   * brief Default constructor.
   *
   * It initializes the reference counter to one.
   */
  GenericCounterImplementation() : use_count_()
  {
    use_count_.increment();
  }

  /* Destructor */
  virtual ~GenericCounterImplementation() {}

  /*
   * brief Delete the pointee.
   *
   * This method diposes of the pointee by deleting it because it isn't referenced any more.
   * This method is pure because the allocation and deallocation are not handled here. The
   * derived class is in charge ot this functionnality.
   * See also: release()
   */
  virtual void dispose() = 0;

  /*
   * brief Remove one reference.
   *
   * This method removes one reference to the counter. If it is the last one, it calls
   * dispose() to delete the pointee.
   * See also: dispose() and add_ref_copy()
   */
  void release()
  {
    if ( use_count_.fetchAndAdd( -1 ) == 1 ) dispose();
  }

  /*
   * brief Add one reference.
   *
   * This method adds one reference to the counter.
   * See also: release()
   */
  void add_ref_copy()
  {
    use_count_.increment();
  }

  /*
   * brief Get the number of references.
   *
   * This methods returns the number of references currently owned by the counter.
   * Return: the number of references
   * See also: release() and add_ref_copy()
   */
  UnsignedInteger use_count()
  {
    return use_count_.get();
  }

}; /* end class GenericCounterImplementation */



/*
 * class CounterImplementation
 *
 * brief This class is in charge of the pointer allocation/deallocation
 */

template <class T>
class CounterImplementation
  : public GenericCounterImplementation
{
  T * ptr_;

public:

  CounterImplementation(T * p) : ptr_( p ) {}

  void dispose()
  {
    delete ptr_;
    ptr_ = 0;
  }

}; /* end class CounterImplementation */



class OT_API Counter
{
  GenericCounterImplementation * p_impl_;

public:

  Counter() : p_impl_(0) {}

  ~Counter()
  {
    if ( p_impl_ != 0 ) p_impl_->release();
    if (use_count() == 0)
    {
      delete p_impl_;
      p_impl_ = 0;
    }
  }

  Counter( const Counter & other ) : p_impl_( other.p_impl_ )
  {
    if ( p_impl_ != 0 ) p_impl_->add_ref_copy();
  }

  template <class T> Counter(T * p) : p_impl_(0)
  {
    p_impl_ = new CounterImplementation< T >( p );
  }

  Counter & operator =( const Counter & other )
  {
    Counter newCounter( other );
    swap( newCounter );
    return *this;
  }

  Bool unique() const
  {
    return use_count() == 1;
  }

  UnsignedInteger use_count() const
  {
    return ( p_impl_ != 0 ) ? p_impl_->use_count() : 0 ;
  }

  void swap(Counter & other)
  {
    std::swap( p_impl_, other.p_impl_ );
  }
};


#endif /* SWIG */



class OT_API dynamic_cast_tag {};

/**
 * @class SharedPointer
 *
 * @brief This class implements a shared pointer strategy
 */

template <class T>
class SharedPointer
{

  template <class Y> friend class SharedPointer;

  /**
   * The actual pointer is \em ptr_
   */
  T * ptr_;
  Counter count_;

public:

  /**
   * Default constructor
   *
   * Constructed like this, the underlying pointer is NULL
   */
  SharedPointer() : ptr_(0), count_()
  {
    // Nothing to do
  }

  /**
   * Constructor from T * type
   *
   * The object pointed t is from now owned and taken in charge
   * by the shared pointer. It MUST NOT be manually deleted
   */
  explicit SharedPointer(T * ptr) : ptr_(ptr), count_(ptr)
  {
    // Nothing to do
  }

  /**
   * Copy constructor adds one more reference
   * on the underying object so its reference counter is
   * incremented by one
   */
  template <class Y>
  SharedPointer(const SharedPointer<Y> & ref) : ptr_(ref.ptr_), count_(ref.count_)
  {
    // Nothing to do
  }

  template <class Y>
  SharedPointer(const SharedPointer<Y> & ref, dynamic_cast_tag) : ptr_(dynamic_cast<T*>(ref.ptr_)), count_(ref.count_)
  {
    if (ptr_ == 0) count_ = Counter();
  }

  /**
   * Method to cast objects passed as base class into
   * derived class
   */
  template <class Base>
  SharedPointer & assign(const SharedPointer<Base> & ref)
  {
    // We want to do : ptr_ = ref.ptr_
    // but ref.ptr_ is a base class of ptr_
    // so we need to dynamic cast it...

    // Dynamic cast using a temporary
    //          pointer_type tmp(ref.ptr_, dynamic_cast_tag());
    //          ptr_ = tmp;

    SharedPointer( ref, dynamic_cast_tag() ).swap( *this );
    return *this;
  }



  /**
   * Reset forsakes its reference on the pointed-to object.
   * If the shared pointer is the only owner of the object,
   * reset leads to its deletion
   */
  inline void reset()
  {
    SharedPointer().swap( *this);
  }

  /**
   * Reset forsakes its reference on the pointed-to object.
   * If the shared pointer is the only owner of the object,
   * reset leads to its deletion. This method takes the new
   * pointed-to object and takes it in charge inside the
   * shared pointer
   */
  template <class Y>
  inline void reset(Y * p)
  {
    SharedPointer( p ).swap( *this);
  }

  /**
   * Operator * dereferences the const shared pointer and gives
   * access to the underlying object
   */
  inline const T & operator * () const
  {
    return *ptr_;
  }

  /**
   * Operator -> dereferences the const shared pointer and gives
   * access to the underlying object
   */
  inline const T * operator -> () const
  {
    return ptr_;
  }

  /**
   * Operator * dereferences the shared pointer and gives
   * access to the underlying object
   */
  inline T & operator * ()
  {
    return *ptr_;
  }

  /**
   * Operator -> dereferences the shared pointer and gives
   * access to the underlying object
   */
  inline T * operator -> ()
  {
    return ptr_;
  }

  /**
   * Get returns a pointer to the underlying object
   */
  inline T * get() const
  {
    return ptr_;
  }

  /**
   * Method getImplementation() gives access to the
   * underlying implementation object (for copy
   * constructor needs)
   */
  inline const T * getImplementation() const
  {
    return ptr_;
  }

  /**
   * Unique returns true when the shared pointer is the only
   * one that takes in charge the pointed-to object
   * It is semantically equivalent to use_count() == 1
   */
  inline Bool unique() const
  {
    return count_.unique();
  }

  /**
   * Use_count returns the number of shared pointers that
   * share the pointed-to object
   */
  inline UnsignedInteger use_count() const
  {
    return count_.use_count();
  }

  /**
   * Swap exchanges the pointed-to objects between two
   * shared pointers
   */
  inline void swap(SharedPointer<T> & other)
  {
    std::swap( ptr_, other.ptr_ );
    count_.swap( other.count_ );
  }

} ; /* end class SharedPointer */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SHAREDPOINTER_HXX */
