//                                               -*- C++ -*-
/**
 *  @brief Find the roots in a given direction according to the Proban
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
#include "PersistentObjectFactory.hxx"
#include "SafeAndSlow.hxx"
#include "NumericalPoint.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SafeAndSlow
 */

CLASSNAMEINIT(SafeAndSlow);

static const Factory<SafeAndSlow> RegisteredFactory;

/* Constructor with parameters */
SafeAndSlow::SafeAndSlow():
  RootStrategyImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
SafeAndSlow::SafeAndSlow(const Solver & solver):
  RootStrategyImplementation(solver)
{
  // Nothing to do
}

/* Constructor with parameters */
SafeAndSlow::SafeAndSlow(const Solver & solver,
                         const NumericalScalar maximumDistance,
                         const NumericalScalar stepSize):
  RootStrategyImplementation(solver, maximumDistance, stepSize)
{
  // Nothing to do
}

/* Virtual constructor */
SafeAndSlow * SafeAndSlow::clone() const
{
  return new SafeAndSlow(*this);
}

/* Solve gives all the roots found applying the root strategy */
SafeAndSlow::NumericalScalarCollection SafeAndSlow::solve(const NumericalMathFunction & function,
    const NumericalScalar value)
{
  NumericalScalarCollection result(0);
  NumericalScalar infPoint = 0.0;
  NumericalScalar infValue = 0.0;
  // Get the value of the function at the origin
  try
  {
    infValue = getOriginValue();
  }
  // If it has not yet been computed, compute it and store it
  catch (NotDefinedException &)
  {
    infValue = function(NumericalPoint(1, infPoint))[0];
    setOriginValue(infValue);
  }
  // If the origin is in the failure domain add it to the roots
  if (infValue == value) result.add(infPoint);
  const NumericalScalar maximumDistance(getMaximumDistance());
  const NumericalScalar stepSize(getStepSize());
  Solver solver(getSolver());
  while(infPoint < maximumDistance)
  {
    const NumericalScalar supPoint(std::min(infPoint + stepSize, maximumDistance));
    const NumericalScalar supValue(function(NumericalPoint(1, supPoint))[0]);
    if ((infValue - value) * (supValue - value) < 0.0)
    {
      result.add(solver.solve(function, value, infPoint, supPoint, infValue, supValue));
    }
    infPoint = supPoint;
    infValue = supValue;
  }
  LOGDEBUG(OSS() << "SafeAndSlow::solve: roots=" << result);
  return result;
}

/* String converter */
String SafeAndSlow::__repr__() const
{
  OSS oss;
  oss << "class=" << SafeAndSlow::GetClassName()
      << " derived from " << RootStrategyImplementation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
