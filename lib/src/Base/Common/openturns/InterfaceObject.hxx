//                                               -*- C++ -*-
/**
 *  @brief The abstract class InterfaceObject defines the GetImplementation method
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
#ifndef OPENTURNS_INTERFACEOBJECT_HXX
#define OPENTURNS_INTERFACEOBJECT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Object.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InterfaceObject
 *
 * @brief The top-most class that allows the construction of the Interface/Implementation design pattern
 * @internal
 * @see TypedInterfaceObject
 *
 * InterfaceObject defines a pure virtual method getImplementation
 * that returns a pointer to a PersistentObject. See TypedInterfaceObject for concrete implementation.
 */

class OT_API InterfaceObject
  : public Object
{
  CLASSNAME
public:

  typedef Pointer<PersistentObject>   ImplementationAsPersistentObject;

  /** @copydoc Object::__repr__() const */
  String __repr__() const override;

  /** @copydoc Object::__str__() const */
  String __str__(const String & offset = "") const override;

  /** Returns a pointer to the underlying implementation object viewed as a PersistentObject
   * @internal
   */
  virtual ImplementationAsPersistentObject getImplementationAsPersistentObject() const = 0 ;

  /** Sets the pointer to the underlying implementation object
   * @internal
   */
  virtual void setImplementationAsPersistentObject(const ImplementationAsPersistentObject & obj) = 0;

  /** Id accessor */
  Id getId() const;

  /** Name accessor */
  virtual void setName(const String & name);
  virtual String getName() const;

  /** Method save() stores the object through the StorageManager
   * @internal
   */
  void save(StorageManager & mgr, const String & label) const;

  /** Method save() stores the object through the StorageManager
   * @internal
   */
  void save(StorageManager & mgr) const;

}; /* class InterfaceObject */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTERFACEOBJECT_HXX */
