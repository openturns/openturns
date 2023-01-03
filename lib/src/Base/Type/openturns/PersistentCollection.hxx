//                                               -*- C++ -*-
/**
 *  @brief PersistentCollection defines top-most collection strategies
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
#ifndef OPENTURNS_PERSISTENTCOLLECTION_HXX
#define OPENTURNS_PERSISTENTCOLLECTION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Collection.hxx"

#define TEMPLATE_CLASSNAMEINIT(T) template<> OT::String PersistentCollectionGetClassName<T>() { return OT::String(#T); }


BEGIN_NAMESPACE_OPENTURNS


/**
 * @class PersistentCollection
 *
 * PersistentCollection defines top-most collection strategies
 */

/* Forward declaration */
template <class T> class PersistentCollection;


/** Stream operator */
template <class T> inline
OStream & operator << (OStream & OS,
                       const PersistentCollection<T> & collection)
{
  return OS << collection.__repr__();
}

template <class T> inline
std::ostream & operator << (std::ostream & os,
                            const PersistentCollection<T> & collection)
{
  return os << collection.__str__();
}


/* Generic function for PersistentCollection of PersistentObject instances */
template<typename T>
String PersistentCollectionGetClassName()
{
  return String("PersistentCollection<") + T::GetClassName() + String(">");
}

/* Specializations for non-OT types */
template<> inline String PersistentCollectionGetClassName<Bool>()
{
  return String("PersistentCollection<Bool>");
}
template<> inline String PersistentCollectionGetClassName<String>()
{
  return String("PersistentCollection<String>");
}
template<> inline String PersistentCollectionGetClassName<Scalar>()
{
  return String("PersistentCollection<Scalar>");
}
template<> inline String PersistentCollectionGetClassName<Complex>()
{
  return String("PersistentCollection<Complex>");
}
template<> inline String PersistentCollectionGetClassName<SignedInteger>()
{
  return String("PersistentCollection<SignedInteger>");
}
template<> inline String PersistentCollectionGetClassName<UnsignedInteger>()
{
  return String("PersistentCollection<UnsignedInteger>");
}
#ifndef OPENTURNS_UNSIGNEDLONG_SAME_AS_UINT64
template<> inline String PersistentCollectionGetClassName<Unsigned64BitsInteger>()
{
  return String("PersistentCollection<Unsigned64BitsInteger>");
}
template<> inline String PersistentCollectionGetClassName<Signed64BitsInteger>()
{
  return String("PersistentCollection<Signed64BitsInteger>");
}
#endif


template <class T>
class PersistentCollection
  : public PersistentObject,
    public Collection<T>
{
public:
  /** Class name accessor */
  static  String GetClassName()
  {
    return PersistentCollectionGetClassName<T>();
  }
  virtual String getClassName() const
  {
    return PersistentCollection<T>::GetClassName();
  }

public:

  typedef Collection<T>                                 InternalType;
  typedef typename InternalType::ElementType            ElementType;
  typedef typename InternalType::ValueType              ValueType;
  typedef typename InternalType::iterator               iterator;
  typedef typename InternalType::const_iterator         const_iterator;
  typedef typename InternalType::reverse_iterator       reverse_iterator;
  typedef typename InternalType::const_reverse_iterator const_reverse_iterator;



  /** Default constructor */
  PersistentCollection()
    : PersistentObject(),
      Collection<T>()
  {
    // Nothing to do
  }

  /** Constructor from a collection */
  PersistentCollection(const Collection<T> & collection)
    : PersistentObject(),
      Collection<T>(collection)
  {
    // Nothing to do
  }

  /** Constructor that pre-allocate size elements */
  PersistentCollection(const UnsignedInteger size)
    : PersistentObject(),
      Collection<T>(size)
  {
    // Nothing to do
  }

  /** Constructor that pre-allocate size elements with value */
  PersistentCollection(const UnsignedInteger size,
                       const T & value)
    : PersistentObject(),
      Collection<T>(size, value)
  {
    // Nothing to do
  }

  /** Constructor from a range of elements */
  template <typename InputIterator>
  PersistentCollection(InputIterator first,
                       InputIterator last)
    : PersistentObject(),
      Collection<T>(first, last)
  {
    // Nothing to do
  }


  /** Virtual constructor */
  virtual PersistentCollection * clone() const
  {
    return new PersistentCollection(*this);
  }


  /** String converter */
  inline
  String __repr__() const
  {
    return Collection<T>::__repr__();
  }

  inline
  String __str__(const String & offset = "") const
  {
    return Collection<T>::__str__(offset);
  }

  /** Method save() stores the object through the StorageManager */
  inline
  void save(Advocate & adv) const
  {
    PersistentObject::save(adv);
    adv.saveAttribute( "size", this->getSize() );
    // We cannot write
    //    std::copy( Collection<T>::coll__.begin(),
    //               Collection<T>::coll__.end(),
    //               AdvocateIterator< T >( adv ) );
    // because AdvocateIterator is not copy-assignable.
    AdvocateIterator<T> adv_it(adv);
    for (const_iterator cit = Collection<T>::coll__.begin();
         cit != Collection<T>::coll__.end();
         ++cit, ++adv_it)
    {
      *adv_it = *cit;
    }
  }

  /** Method load() reloads the object from the StorageManager */
  inline
  void load(Advocate & adv)
  {
    PersistentObject::load(adv);
    UnsignedInteger size = 0;
    adv.loadAttribute( "size", size );
    Collection<T>::coll__.resize(size);
    std::generate( Collection<T>::coll__.begin(),
                   Collection<T>::coll__.end(),
                   AdvocateIterator< T >( adv ) );
  }


}; /* class PersistentCollection */


/** Comparison operator */
template <class T> inline
Bool operator == (const PersistentCollection<T> & lhs,
                  const PersistentCollection<T> & rhs)
{
  return static_cast<const Collection<T>& >(lhs) == static_cast<const Collection<T>& >(rhs);
}


/** Comparison operator */
template <class T> inline
Bool operator != (const PersistentCollection<T> & lhs,
                  const PersistentCollection<T> & rhs)
{
  return !(lhs == rhs);
}


/** Ordering operator */
template <class T> inline
Bool operator < (const PersistentCollection<T> & lhs,
                 const PersistentCollection<T> & rhs)
{
  return static_cast<const Collection<T>& >(lhs) < static_cast<const Collection<T>& >(rhs);
}


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PERSISTENTCOLLECTION_HXX */
