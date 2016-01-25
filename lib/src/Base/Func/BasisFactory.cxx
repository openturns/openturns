//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all BasisFactory
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#include "BasisFactory.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BasisFactory);

static const Factory<BasisFactory> RegisteredFactory;


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
