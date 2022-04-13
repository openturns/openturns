//                                               -*- C++ -*-
/**
 *  @brief Class PersistentObjectFactory saves and reloads the object's internal state
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Catalog.hxx"

BEGIN_NAMESPACE_OPENTURNS



/* Virtual constructor */
// PersistentObjectFactory * PersistentObjectFactory::clone() const = 0


/* This method register the factory into the Catalog */
void PersistentObjectFactory::registerMe(const String & className) const
{
  Catalog::Add(className, this);
}


/* Method build() creates a new PersistentObject from the storage manager */
// PersistentObject * PersistentObjectFactory::build(const StorageManager & mgr) const = 0;


/* Accessor to PersistentObject's shadowed id */
void PersistentObjectFactory::setShadowedId(PersistentObject & obj, Id id) const
{
  obj.setShadowedId(id);
}

Id PersistentObjectFactory::getShadowedId(PersistentObject & obj) const
{
  return obj.getShadowedId();
}


END_NAMESPACE_OPENTURNS
