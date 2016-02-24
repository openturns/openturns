//                                               -*- C++ -*-
/**
 *  @brief Univariate function implementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(UniVariateFunctionImplementation);

static const Factory<UniVariateFunctionImplementation> RegisteredFactory;


/* Default constructor */
UniVariateFunctionImplementation::UniVariateFunctionImplementation()
  : PersistentObject()
{
  // Nothing to do
}


/* Virtual constructor */
UniVariateFunctionImplementation * UniVariateFunctionImplementation::clone() const
{
  return new UniVariateFunctionImplementation(*this);
}


/* String converter */
String UniVariateFunctionImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName();
}


String UniVariateFunctionImplementation::__str__(const String & offset) const
{
  return OSS(true) << offset << __repr__();
}


/* UniVariateFunctionImplementation are evaluated as functors */
NumericalScalar UniVariateFunctionImplementation::operator() (const NumericalScalar x) const
{
  throw NotYetImplementedException(HERE) << "UniVariateFunctionImplementation::operator() (const NumericalScalar x)";
}

/* UniVariateFunctionImplementation gradient */
NumericalScalar UniVariateFunctionImplementation::gradient(const NumericalScalar x) const
{
  throw NotYetImplementedException(HERE) << "UniVariateFunctionImplementation::gradient(const NumericalScalar x)";
}

/* UniVariateFunctionImplementation hessian */
NumericalScalar UniVariateFunctionImplementation::hessian(const NumericalScalar x) const
{
  throw NotYetImplementedException(HERE) << "UniVariateFunctionImplementation::hessian(const NumericalScalar x)";
}

/* Method save() stores the object through the StorageManager */
void UniVariateFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void UniVariateFunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}


END_NAMESPACE_OPENTURNS
