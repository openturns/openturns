//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all BasisFactory
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
#include "openturns/BasisFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BasisFactory)

static const Factory<BasisFactory> Factory_BasisFactory;


/* Default constructor */
BasisFactory::BasisFactory()
  : PersistentObject()
{
  // Nothing to do
}

/* String converter */
String BasisFactory::__repr__() const
{
  return OSS() << "class=" << BasisFactory::GetClassName()
         << " name=" << getName();
}


/* Here is the interface that all derived class must implement */

BasisFactory* BasisFactory::clone() const
{
  return new BasisFactory(*this);
}


Basis BasisFactory::build() const
{
  throw NotYetImplementedException(HERE) << "In BasisFactory::build() const";
}


END_NAMESPACE_OPENTURNS
