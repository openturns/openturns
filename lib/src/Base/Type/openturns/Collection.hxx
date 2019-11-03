//                                               -*- C++ -*-
/**
 *  @brief Collection defines top-most collection strategies
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_COLLECTION_HXX
#define OPENTURNS_COLLECTION_HXX

#include <vector>
#include <algorithm>         // for std::copy
#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/OSS.hxx"
#include "openturns/OStream.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class Collection
 *
 * Collection defines top-most collection strategies
 */

/* Forward declaration */
template <class T> class Collection;


/** Comparison operator (friend) */
template <class T> inline
Bool operator == (const Collection<T> & lhs,
                  const Collection<T> & rhs)
{
  return lhs.coll__ == rhs.coll__;
}


/** Comparison operator (friend) */
template <class T> inline
Bool operator != (const Collection<T> & lhs,
                  const Collection<T> & rhs)
{
  return !(lhs == rhs);
}


/** Ordering operator (friend) */
template <class T> inline
Bool operator < (const Collection<T> & lhs,
                 const Collection<T> & rhs)
{
  return lhs.coll__ < rhs.coll__;
}


/** Stream operator */
template <class T> inline
std::ostream & operator << (std::ostream & os,
                            const Collection<T> & collection)
{
  return os << collection.__repr__();
}

template <class T> inline
OStream & operator << (OStream & OS,
                       const Collection<T> & collection)
{
  return OS << collection.__str__();
}





template <class T>
class Collection
{
public:

  typedef T ElementType;
  typedef T ValueType;

  typedef typename std::vector<T>                       InternalType;
  typedef typename InternalType::iterator               iterator;
  typedef typename InternalType::const_iterator         const_iterator;
  typedef typename InternalType::reverse_iterator       reverse_iterator;
  typedef typename InternalType::const_reverse_iterator const_reverse_iterator;

protected:

  /* The actual collection is a STL vector */
  InternalType coll__;

public:

  /** Default constructor */
  Collection()
    : coll__()
  {
    // Nothing to do
  }

  /** Constructor that pre-allocate size elements */
  Collection(const UnsignedInteger size)
    : coll__(size)
  {
    // Nothing to do
  }

  /** Constructor that pre-allocate size elements with value */
  Collection(const UnsignedInteger size,
             const T & value)
    : coll__(size, value)
  {
    // Nothing to do
  }

  /* Virtual destructor to allow dynamic polymorphism*/
  virtual ~Collection()
  {
    // Nothing to do
  }

  /** Constructor from a range of elements */
  template <typename InputIterator>
  Collection(const InputIterator first,
             const InputIterator last)
    : coll__(first, last)
  {
    // Nothing to do
  }

#ifndef SWIG
  /** Erase the elements between first and last */
  inline
  virtual iterator erase(const iterator first,
                         const iterator last)
  {
    if ( (first < begin()) ||
         (first > end())   ||
         (last  < begin()) ||
         (last  > end())   )
      throw OutOfBoundException(HERE) << "Can NOT erase value outside of collection";
    return coll__.erase(first, last);
  }

  /** Erase the elements pointed by position */
  inline
  virtual iterator erase(iterator position)
  {
    if ( (position < begin()) ||
         (position > end())   )
      throw OutOfBoundException(HERE) << "Can NOT erase value outside of collection";
    return coll__.erase(position);
  }
#endif

  /** Clear all elements of the collection */
  inline
  virtual void clear()
  {
    coll__.clear();
  }

  /** Assign elements to the collection */
  template <typename InputIterator>
  inline
  void assign(const InputIterator first,
              const InputIterator last)
  {
    coll__.assign(first, last);
  }

#ifndef SWIG
  /** Operator[]() gives access to the elements of the collection */
  inline
  virtual T & operator [] (const UnsignedInteger i)
  {
    return coll__[i];
  }

  /** Operator[]() gives access to the elements of the const collection */
  inline
  virtual const T & operator [] (const UnsignedInteger i) const
  {
    return coll__[i];
  }
#endif

  /* Method __len__() is for Python */
  inline
  UnsignedInteger __len__() const
  {
    return coll__.size();
  }

  /* Method __eq__() is for Python */
  inline
  Bool __eq__(const Collection & rhs) const
  {
    return (*this == rhs);
  }

  /* Whether the list contains the value val */
  inline
  Bool contains(T val) const
  {
    for (UnsignedInteger i = 0; i < coll__.size(); ++i) if ( coll__[i] == val ) return true;
    return false;
  }

  /* Method __getitem__() is for Python */
  inline
  T __getitem__(SignedInteger i) const
  {
    if (i < 0)
    {
      i += coll__.size();
    }
    return coll__.at(i);
  }

  /* Method __setitem__() is for Python */
  inline
  virtual void __setitem__(SignedInteger i,
                           const T & val)
  {
    if (i < 0)
    {
      i += coll__.size();
    }
    coll__.at(i) = val;
  }

  /* Method __delitem__() is for Python */
  inline
  virtual void __delitem__(const UnsignedInteger i)
  {
    if (i < coll__.size())
      coll__.erase( coll__.begin() + i );
    else
      throw OutOfBoundException(HERE) << "Index i is out of range. Got " << i << " (size=" << coll__.size() << ")";
  }

  /** At() gives access to the elements of the collection but throws an exception if bounds are overcome */
  inline
  virtual T & at(const UnsignedInteger i)
  {
    return coll__.at(i);
  }

  /** At() gives access to the elements of the const collection but throws an exception if bounds are overcome */
  inline
  virtual const T & at(const UnsignedInteger i) const
  {
    return coll__.at(i);
  }

  /** Method add() appends an element to the collection */
  inline
  virtual void add(const T & elt)
  {
    coll__.push_back(elt);
  }

  /** Method add() appends a collection to the collection */
  inline
  virtual void add(const Collection< T > & coll)
  {
    coll__.insert(coll__.end(), coll.begin(), coll.end());
  }

  /** Method getSize() returns the number of elements of the collection */
  inline
  UnsignedInteger getSize() const
  {
    return static_cast<UnsignedInteger>(coll__.size());
  }

  /** Method resize() changes the size of the Collection. If the new size is smaller than the older one, the last elements are thrown away, else the new elements are setted to the default value of the element type */
  inline
  virtual void resize(const UnsignedInteger newSize)
  {
    coll__.resize(newSize);
  }

  /** Method empty() returns true if there is no element in the collection */
  inline
  Bool isEmpty() const
  {
    return coll__.empty();
  }

#ifndef SWIG
  /** Method begin() points to the first element of the collection */
  inline
  iterator begin()
  {
    return coll__.begin();
  }
  inline
  const_iterator begin() const
  {
    return coll__.begin();
  }

  /** Method end() points beyond the last element of the collection */
  inline
  iterator end()
  {
    return coll__.end();
  }
  inline
  const_iterator end() const
  {
    return coll__.end();
  }

  /** Method rbegin() points to the last element of the collection */
  inline
  reverse_iterator rbegin()
  {
    return coll__.rbegin();
  }
  inline
  const_reverse_iterator rbegin() const
  {
    return coll__.rbegin();
  }

  /** Method rend() points before the first element of the collection */
  inline
  reverse_iterator rend()
  {
    return coll__.rend();
  }
  inline
  const_reverse_iterator rend() const
  {
    return coll__.rend();
  }
#endif

protected:

  inline
  String toString(Bool full) const
  {
    OSS oss(full);
    String separator;
    oss << "[";
    std::copy( begin(), end(), OSS_iterator<T>(oss, ",") );
    oss << "]";
    return oss;
  }


public:

  /** String converter */
  inline String __repr__() const
  {
    return toString(true);
  }
  inline String __str__(const String & offset = "") const
  {
    OSS oss;
    oss << offset << toString(false);
    if (getSize() >= ResourceMap::GetAsUnsignedInteger("Collection-size-visible-in-str-from"))
      oss << "#" << getSize();
    return oss;
  }

#ifndef SWIG
  /* Friend operator */
  template <class U> friend inline
  Bool operator == (const Collection<U> & lhs,
                    const Collection<U> & rhs);

  /* Friend operator */
  template <class U> friend inline
  Bool operator != (const Collection<U> & lhs,
                    const Collection<U> & rhs);

  /* Friend operator */
  template <class U> friend inline
  Bool operator < (const Collection<U> & lhs,
                   const Collection<U> & rhs);
#endif

}; /* class Collection */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COLLECTION_HXX */
