//                                               -*- C++ -*-
/**
 *  @brief Univariate function
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
#include "openturns/UniVariateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(UniVariateFunction)

/* Default constructor */
UniVariateFunction::UniVariateFunction()
  : TypedInterfaceObject<UniVariateFunctionImplementation>(new UniVariateFunctionImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
UniVariateFunction::UniVariateFunction(const UniVariateFunctionImplementation & implementation)
  : TypedInterfaceObject<UniVariateFunctionImplementation>(implementation.clone())
{
  // Nothing to do
}


/* Constructor from implementation pointer */
UniVariateFunction::UniVariateFunction(const Implementation & p_implementation)
  : TypedInterfaceObject<UniVariateFunctionImplementation>(p_implementation)
{
  // Nothing to do
}


/* Constructor from implementation pointer */
UniVariateFunction::UniVariateFunction(UniVariateFunctionImplementation * p_implementation)
  : TypedInterfaceObject<UniVariateFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String UniVariateFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String UniVariateFunction::__str__(const String & offset) const
{
  return OSS(false) << getImplementation()->__str__(offset);
}

/* UniVariateFunction are evaluated as functors */
Scalar UniVariateFunction::operator() (const Scalar x) const
{
  return getImplementation()->operator()(x);
}

/* UniVariateFunctionImplementation gradient */
Scalar UniVariateFunction::gradient(const Scalar x) const
{
  return getImplementation()->gradient(x);
}

/* UniVariateFunctionImplementation hessian */
Scalar UniVariateFunction::hessian(const Scalar x) const
{
  return getImplementation()->hessian(x);
}

/* Method to draw the graph of the polynomial between given bounds */
Graph UniVariateFunction::draw(const Scalar xMin,
                               const Scalar xMax,
                               const UnsignedInteger pointNumber) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber);
}

END_NAMESPACE_OPENTURNS
