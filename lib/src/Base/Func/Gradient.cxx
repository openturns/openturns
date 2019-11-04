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
#include "openturns/Gradient.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Gradient)

/* Default constructor */
Gradient::Gradient()
  : TypedInterfaceObject<GradientImplementation>(new GradientImplementation())
{
  // Nothing to do
}

/* Parameter constructor */
Gradient::Gradient(const GradientImplementation & implementation)
  : TypedInterfaceObject<GradientImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Parameter constructor */
Gradient::Gradient(const Implementation & p_implementation)
  : TypedInterfaceObject<GradientImplementation>(p_implementation)
{
  // Nothing to do
}

/* Parameter constructor */
Gradient::Gradient(GradientImplementation * p_implementation)
  : TypedInterfaceObject<GradientImplementation>(p_implementation)
{
  // Nothing to do
}

/* Comparison operator */
Bool Gradient::operator ==(const Gradient & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String Gradient::__repr__() const
{
  return getImplementation()->__repr__();
}

/* String converter */
String Gradient::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Get the i-th marginal function */
Gradient Gradient::getMarginal(const UnsignedInteger i) const
{
  return getImplementation()->getMarginal(i);
}

/* Get the function corresponding to indices components */
Gradient Gradient::getMarginal(const Indices & indices) const
{
  return getImplementation()->getMarginal(indices);
}

/* Accessor for input point dimension */
UnsignedInteger Gradient::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger Gradient::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

/* Parameters value accessor */
Point Gradient::getParameter() const
{
  return getImplementation()->getParameter();
}

void Gradient::setParameter(const Point & parameter)
{
  copyOnWrite();
  getImplementation()->setParameter(parameter);
}

/* Get the number of calls to operator() */
UnsignedInteger Gradient::getCallsNumber() const
{
  return getImplementation()->getCallsNumber();
}

/* Gradient method */
Matrix Gradient::gradient(const Point & inP) const
{
  return getImplementation()->gradient(inP);
}


END_NAMESPACE_OPENTURNS
