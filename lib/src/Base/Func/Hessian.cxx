//                                               -*- C++ -*-
/**
 *  @brief The interface class that implements numerical math functions
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Hessian.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Hessian)

/* Default constructor */
Hessian::Hessian()
  : TypedInterfaceObject<HessianImplementation>(new HessianImplementation())
{
  // Nothing to do
}

/* Parameter constructor */
Hessian::Hessian(const HessianImplementation & implementation)
  : TypedInterfaceObject<HessianImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Parameter constructor */
Hessian::Hessian(const Implementation & p_implementation)
  : TypedInterfaceObject<HessianImplementation>(p_implementation)
{
  // Nothing to do
}

/* Parameter constructor */
Hessian::Hessian(HessianImplementation * p_implementation)
  : TypedInterfaceObject<HessianImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool Hessian::operator ==(const Hessian & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String Hessian::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String Hessian::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Get the i-th marginal function */
Hessian Hessian::getMarginal(const UnsignedInteger i) const
{
  return getImplementation()->getMarginal(i);
}

/* Get the function corresponding to indices components */
Hessian Hessian::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* Hessian method */
SymmetricTensor Hessian::hessian(const Point & inP) const
{
  return getImplementation()->hessian(inP);
}

/* Accessor for input point dimension */
UnsignedInteger Hessian::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger Hessian::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Parameters value accessor */
Point Hessian::getParameter() const
{
  return getImplementation()->getParameter();
}

void Hessian::setParameter(const Point & parameter)
{
  copyOnWrite();
  getImplementation()->setParameter(parameter);
}

/* Get the number of calls to operator() */
UnsignedInteger Hessian::getCallsNumber() const
{
  return getImplementation()->getCallsNumber();
}

END_NAMESPACE_OPENTURNS
