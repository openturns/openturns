//                                               -*- C++ -*-
/**
 *  @brief The class Pointer implements a shared pointer strategy
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
#ifndef OPENTURNS_POINTER_HXX
#define OPENTURNS_POINTER_HXX

#include "openturns/OTprivate.hxx"

#include <memory> // for std::shared_ptr


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Pointer
 * @brief Implements a shared pointer strategy
 * @see InterfaceObject
 * @tparam T The class to point to
 * @tparam IMPL The class used to actually implement the shared pointer
 *
 * Pointer implements a shared pointer strategy. The pointee
 * object is shared among all pointers so none has pointee
 * ownership. When one of them wants to modify the object, it
 * MUST deep-copy it (copy-on-write strategy).
 */

template < class T,
           template <class> class IMPL = std::shared_ptr >
class Pointer
{

  template <class Y, template <class> class I> friend class Pointer;

public:
  /**
   * Pointer_type gives access to the shared pointer
   * implementation type
   */

  typedef IMPL<T> pointer_type;

  /**
   * The actual shared pointer
   */
  pointer_type ptr_;


  typedef T ElementType;
  typedef T ValueType;

  /**
   * Default constructor.
   *
   * Constructed like this, the underlying pointer is NULL
   */
  Pointer() : ptr_()
  {
  }

  /**
   * Constructor from T * type
   *
   * The object pointed t is from now owned and taken in charge
   * by the shared pointer. It MUST NOT be manually deleted
   */
  Pointer(T * ptr) : ptr_(ptr)
  {
  }


  /**
   * Destructor does nothing
   */
  ~Pointer()
  {
  }


  /**
   * Copy constructor adds one more reference
   * on the underying object so its reference counter is
   * incremented by one
   */
  template <class Derived>
  Pointer(const Pointer<Derived> & ref) : ptr_(ref.ptr_)
  {
  }

#ifndef SWIG
  /**
   * Equality operator adds one more reference
   * on the underying object so its reference counter is
   * incremented by one
   */
  template <class Derived>
  Pointer & operator = (const Pointer<Derived> & ref)
  {
    ptr_ = ref.ptr_;
    return *this;
  }
#endif

  /**
   * Method to cast objects passed as base class into
   * derived class
   */
  template <class Base>
  Pointer & assign(const Pointer<Base> & ref)
  {
    pointer_type ptr(std::dynamic_pointer_cast<T>(ref.ptr_));
    ptr_.swap(ptr);
    return *this;
  }


  /**
   * Reset forsakes its reference on the pointed-to object.
   * If the shared pointer is the only owner of the object,
   * reset leads to its deletion
   */
  inline void reset()
  {
    ptr_.reset();
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
    ptr_.reset(p);
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
    return ptr_.get();
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
    return ptr_.get();
  }

  /**
   * Method isNull returns true if there is no underlying
   * object pointed to
   */
  inline Bool isNull() const
  {
    return ptr_.get() == 0;
  }

  /**
   * Operator bool() allows null pointer checking is assertions or tests
   */
  inline operator bool() const
  {
    return ptr_.get() != 0;
  }

  /**
   * Method get gives access to the underlying object
   * pointed to
   */
  inline T * get() const
  {
    return ptr_.get();
  }

  /**
   * Method getImplementation() gives access to the
   * underlying implementation object (for copy
   * constructor needs)
   */
  inline const pointer_type & getImplementation() const
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
    return ptr_.unique();
  }

  /**
   * Use_count returns the number of shared pointers that
   * share the pointed-to object
   */
  inline UnsignedInteger use_count() const
  {
    return ptr_.use_count();
  }

  /**
   * Swap exchanges the pointed-to objects between two
   * shared pointers
   */
  inline void swap(Pointer<T> & other)
  {
    ptr_.swap(other.ptr_);
  }

}; /* class Pointer */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTER_HXX */
