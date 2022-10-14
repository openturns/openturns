//                                               -*- C++ -*-
/**
 *  @brief TypedCollectionInterfaceObject implements TypedInterfaceObject for a class that manages a collection
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TYPEDCOLLECTIONINTERFACEOBJECT_HXX
#define OPENTURNS_TYPEDCOLLECTIONINTERFACEOBJECT_HXX

#include <stdexcept>
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TypedCollectionInterfaceObject
 * @brief Implements TypedInterfaceObject for a class that manages a collection
 * @internal
 * @see PersistentCollection
 */
template <class T>
class TypedCollectionInterfaceObject
  : public TypedInterfaceObject<T>
{
public:

  typedef typename TypedInterfaceObject<T>::ImplementationAsPersistentObject ImplementationAsPersistentObject;
  typedef typename TypedInterfaceObject<T>::ImplementationType               ImplementationType;
  typedef typename ImplementationType::ElementType                           ImplementationElementType;
  typedef typename TypedInterfaceObject<T>::Implementation                   Implementation;
  typedef typename ImplementationType::iterator                              iterator;
  typedef typename ImplementationType::const_iterator                        const_iterator;
  typedef typename ImplementationType::reverse_iterator                      reverse_iterator;
  typedef typename ImplementationType::const_reverse_iterator                const_reverse_iterator;

  /** Constructor */
  TypedCollectionInterfaceObject() {}
  TypedCollectionInterfaceObject(const Implementation & impl) : TypedInterfaceObject<T>(impl) {}


#ifndef SWIG
  /** Operator[]() gives access to the elements of the collection */
  inline ImplementationElementType & operator [] (UnsignedInteger i)
  {
#ifdef DEBUG_BOUNDCHECKING
    return this->at(i);
#else
    TypedInterfaceObject<T>::copyOnWrite();
    return TypedInterfaceObject<T>::getImplementation()->operator[](i);
#endif /* DEBUG_BOUNDCHECKING */
  }

  /** Operator[]() gives access to the elements of the const collection */
  inline const ImplementationElementType & operator [] (UnsignedInteger i) const
  {
#ifdef DEBUG_BOUNDCHECKING
    return this->at(i);
#else
    return TypedInterfaceObject<T>::getImplementation()->operator[](i);
#endif /* DEBUG_BOUNDCHECKING */
  }

  /** Element accessor */
  inline ImplementationElementType & at(UnsignedInteger i)
  {
    TypedInterfaceObject<T>::copyOnWrite();

    try
    {
      return TypedInterfaceObject<T>::getImplementation()->at(i);

    }
    catch (const std::out_of_range & ex)
    {
      throw OutOfBoundException(HERE) << "sample size=" << getSize()
                                      << " - erroneous index=" << i
                                      << " - " << ex.what();
    }
  }

  inline const ImplementationElementType & at(UnsignedInteger i) const
  {
    try
    {
      return TypedInterfaceObject<T>::getImplementation()->at(i);

    }
    catch (const std::out_of_range & ex)
    {
      throw OutOfBoundException(HERE) << "sample size=" << getSize()
                                      << " - erroneous index=" << i
                                      << " - " << ex.what();
    }
  }
#endif

  /* Method __getitem__() is for Python */
  inline const ImplementationElementType & __getitem__(UnsignedInteger i) const
  {
    return this->at(i);
  }

  /* Method __setitem__() is for Python */
  inline void __setitem__(UnsignedInteger i, const ImplementationElementType & val)
  {
    this->at(i) = val;
  }

  /** Method add() appends an element to the collection */
  inline void add(const ImplementationElementType & elt)
  {
    TypedInterfaceObject<T>::copyOnWrite();
    TypedInterfaceObject<T>::getImplementation()->add(elt);
  }

#ifndef SWIG
  /** Erase the elements between first and last */
  inline iterator erase(iterator first, iterator last)
  {
    TypedInterfaceObject<T>::copyOnWrite();
    return TypedInterfaceObject<T>::getImplementation()->erase(first, last);
  }

  /** Erase the element pointed by position */
  inline iterator erase(iterator position)
  {
    TypedInterfaceObject<T>::copyOnWrite();
    return TypedInterfaceObject<T>::getImplementation()->erase(position);
  }

  /** Erase the element pointed by position */
  inline iterator erase(UnsignedInteger position)
  {
    return erase(begin() + position);
  }
#endif

  /** Method getSize() returns the number of elements of the collection (viewed as a size) */
  inline UnsignedInteger getSize() const
  {
    return TypedInterfaceObject<T>::getImplementation()->getSize();
  }

  /** Method getDimension() returns the number of elements of the collection (viewed as a dimension) */
  inline UnsignedInteger getDimension() const
  {
    return TypedInterfaceObject<T>::getImplementation()->getDimension();
  }

  /** Method empty() returns true if there is no element in the collection */
  inline Bool isEmpty() const
  {
    return TypedInterfaceObject<T>::getImplementation()->isEmpty();
  }

#ifndef SWIG
  /** Method begin() points to the first element of the collection */
  inline iterator begin()
  {
    TypedInterfaceObject<T>::copyOnWrite();
    return TypedInterfaceObject<T>::getImplementation()->begin();
  }
  inline const_iterator begin() const
  {
    return TypedInterfaceObject<T>::getImplementation()->begin();
  }

  /** Method end() points beyond the last element of the collection */
  inline iterator end()
  {
    TypedInterfaceObject<T>::copyOnWrite();
    return TypedInterfaceObject<T>::getImplementation()->end();
  }
  inline const_iterator end() const
  {
    return TypedInterfaceObject<T>::getImplementation()->end();
  }

  /** Method rbegin() points to the last element of the collection */
  inline reverse_iterator rbegin()
  {
    TypedInterfaceObject<T>::copyOnWrite();
    return TypedInterfaceObject<T>::getImplementation()->rbegin();
  }
  inline const_reverse_iterator rbegin() const
  {
    return TypedInterfaceObject<T>::getImplementation()->rbegin();
  }

  /** Method rend() points before the first element of the collection */
  inline reverse_iterator rend()
  {
    TypedInterfaceObject<T>::copyOnWrite();
    return TypedInterfaceObject<T>::getImplementation()->rend();
  }
  inline const_reverse_iterator rend() const
  {
    return TypedInterfaceObject<T>::getImplementation()->rend();
  }
#endif

}; /* class TypedCollectionInterfaceObject */




END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TYPEDCOLLECTIONINTERFACEOBJECT_HXX */
