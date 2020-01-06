//                                               -*- C++ -*-
/**
 *  @brief TypedInterfaceObject implements InterfaceObject for a specific class
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
#ifndef OPENTURNS_TYPEDINTERFACEOBJECT_HXX
#define OPENTURNS_TYPEDINTERFACEOBJECT_HXX

#include "openturns/InterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TypedInterfaceObject
 * @brief Implements InterfaceObject for a specific class
 * @internal
 * @tparam T The class bound to the interface object
 * @see PersistentObject
 */
template <class T>
class TypedInterfaceObject
  : public InterfaceObject
{
public:

  typedef T                                                   ImplementationType;
  typedef Pointer<ImplementationType>                         Implementation;

  /** Constructor */
  TypedInterfaceObject() : p_implementation_(0) {}
  TypedInterfaceObject(const Implementation & impl) : p_implementation_(impl) {}

  /** Returns a pointer to the underlying implementation object */
  inline Implementation & getImplementation()
  {
    return p_implementation_;
  }
  inline const Implementation & getImplementation() const
  {
    return p_implementation_;
  }

#ifndef SWIG
  /** Copy-on-write checker
   * @internal
   * This method must be called in every non const method before any access to the underlying object
   */
  inline void copyOnWrite()
  {
    if (!p_implementation_.unique()) p_implementation_.reset(p_implementation_->clone());
  }
#endif

  /** Return a pointer to the underlying implementation object viewed as a PersistentObject */
  inline virtual ImplementationAsPersistentObject getImplementationAsPersistentObject() const
  {
    return p_implementation_;
  }

  /** Set the pointer to the underlying implementation object */
  inline virtual void setImplementationAsPersistentObject(const ImplementationAsPersistentObject & obj)
  {
    p_implementation_.assign(obj);
  }

  /** This method exchanges the two pointed objects */
  inline void swap(TypedInterfaceObject<T> & other)
  {
    p_implementation_.swap(other.p_implementation_);
  }

  /** Name accessor */
  inline virtual void setName(const String & name)
  {
    this->copyOnWrite();
    p_implementation_->setName(name);
  }
  /** Name accessor */
  inline virtual String getName() const
  {
    return p_implementation_->getName();
  }

  /** Comparison Operator */
  inline virtual
  Bool operator == (const TypedInterfaceObject & other) const
  {
    return *getImplementation() == *(other.getImplementation());
  }

  /** Comparison Operator */
  inline virtual
  Bool operator != (const TypedInterfaceObject & other) const
  {
    return !operator==(other);
  }


protected:

  /** A pointer to the underlying implementation object */
  Implementation p_implementation_;

}; /* class TypedInterfaceObject */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TYPEDINTERFACEOBJECT_HXX */
