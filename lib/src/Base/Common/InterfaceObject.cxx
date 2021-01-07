//                                               -*- C++ -*-
/**
 *  @brief The abstract class InterfaceObject defines the GetImplementation method
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iostream>              // for std::ostream
#include "openturns/InterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(InterfaceObject)


/* String converter */
String InterfaceObject::__repr__() const
{
  return getImplementationAsPersistentObject()->__repr__();
}

/* String converter */
String InterfaceObject::__str__(const String & offset) const
{
  return getImplementationAsPersistentObject()->__str__(offset);
}

/* Id accessor */
Id InterfaceObject::getId() const
{
  return getImplementationAsPersistentObject()->getId();
}

/* Name accessor */
void InterfaceObject::setName(const String & name)
{
  getImplementationAsPersistentObject()->setName(name);
}


/* Name accessor */
String InterfaceObject::getName() const
{
  return getImplementationAsPersistentObject()->getName();
}


/* Method save() stores the object through the StorageManager */
void InterfaceObject::save(StorageManager & mgr, const String & label) const
{
  getImplementationAsPersistentObject()->save( mgr, label );
}

/* Method save() stores the object through the StorageManager */
void InterfaceObject::save(StorageManager & mgr) const
{
  getImplementationAsPersistentObject()->save( mgr );
}


END_NAMESPACE_OPENTURNS
