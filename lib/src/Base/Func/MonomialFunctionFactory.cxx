//                                               -*- C++ -*-
/**
 *  @brief This a class for 1D monomial function factories
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MonomialFunctionFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(MonomialFunctionFactory)

static const Factory<MonomialFunctionFactory> Factory_MonomialFunctionFactory;


/* Default constructor */
MonomialFunctionFactory::MonomialFunctionFactory()
  : UniVariateFunctionFactory()
{
  // Nothing to do.
}

/* Virtual constructor */
MonomialFunctionFactory * MonomialFunctionFactory::clone() const
{
  return new MonomialFunctionFactory(*this);
}


/* String converter */
String MonomialFunctionFactory::__repr__() const
{
  return OSS() << "class=" << getClassName();
}


/* The method to get the function of any order as MonomialFunction*/
MonomialFunction MonomialFunctionFactory::buildAsMonomialFunction(const UnsignedInteger order) const
{
  return MonomialFunction(order);
}

/* The method to get the function of any order */
UniVariateFunction MonomialFunctionFactory::build(const UnsignedInteger order) const
{
  return new MonomialFunction(order);
}

/* Method save() stores the object through the StorageManager */
void MonomialFunctionFactory::save(Advocate & adv) const
{
  UniVariateFunctionFactory::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void MonomialFunctionFactory::load(Advocate & adv)
{
  UniVariateFunctionFactory::load(adv);
}

END_NAMESPACE_OPENTURNS
