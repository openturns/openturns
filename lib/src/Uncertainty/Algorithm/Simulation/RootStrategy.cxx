//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/RootStrategy.hxx"
#include "openturns/SafeAndSlow.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class RootStrategy
 */

CLASSNAMEINIT(RootStrategy);

/* Constructor with parameters */
RootStrategy::RootStrategy(const RootStrategyImplementation & implementation)
  : TypedInterfaceObject<RootStrategyImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor with parameters */
RootStrategy::RootStrategy(Implementation & p_implementation)
  : TypedInterfaceObject<RootStrategyImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor with parameters */
RootStrategy::RootStrategy()
  : TypedInterfaceObject<RootStrategyImplementation>(new SafeAndSlow())
{
  // Nothing to do
}

/* Solve gives all the roots found applying the root strategy */
RootStrategy::NumericalScalarCollection RootStrategy::solve(const NumericalMathFunction & function,
    const NumericalScalar value)
{
  return getImplementation()->solve(function, value);
}

/* Solver accessor */
void RootStrategy::setSolver(const Solver & solver)
{
  copyOnWrite();
  getImplementation()->setSolver(solver);
}

Solver RootStrategy::getSolver() const
{
  return getImplementation()->getSolver();
}

/* Maximum distance accessor */
void RootStrategy::setMaximumDistance(const NumericalScalar maximumDistance)
{
  copyOnWrite();
  getImplementation()->setMaximumDistance(maximumDistance);
}

NumericalScalar RootStrategy::getMaximumDistance() const
{
  return getImplementation()->getMaximumDistance();
}

/* Step size accessor */
void RootStrategy::setStepSize(const NumericalScalar stepSize)
{
  copyOnWrite();
  getImplementation()->setStepSize(stepSize);
}

NumericalScalar RootStrategy::getStepSize() const
{
  return getImplementation()->getStepSize();
}

/* Value of the performance function at the origin accessor */
void RootStrategy::setOriginValue(const NumericalScalar originValue)
{
  copyOnWrite();
  getImplementation()->setOriginValue(originValue);
}

NumericalScalar RootStrategy::getOriginValue() const
{
  return getImplementation()->getOriginValue();
}

/* String converter */
String RootStrategy::__repr__() const
{
  OSS oss;
  oss << "class=" << RootStrategy::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
