//                                               -*- C++ -*-
/**
 *  @brief Find the roots in a given direction according to the Proban
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
#include "PersistentObjectFactory.hxx"
#include "MediumSafe.hxx"
#include "NumericalPoint.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MediumSafe
 */

CLASSNAMEINIT(MediumSafe);

static const Factory<MediumSafe> RegisteredFactory;

/* Constructor with parameters */
MediumSafe::MediumSafe():
  RootStrategyImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
MediumSafe::MediumSafe(const Solver & solver):
  RootStrategyImplementation(solver)
{
  // Nothing to do
}

/* Constructor with parameters */
MediumSafe::MediumSafe(const Solver & solver,
                       const NumericalScalar maximumDistance,
                       const NumericalScalar stepSize):
  RootStrategyImplementation(solver, maximumDistance, stepSize)
{
  // Nothing to do
}

/* Virtual constructor */
MediumSafe * MediumSafe::clone() const
{
  return new MediumSafe(*this);
}

/* Solve gives all the roots found applying the root strategy */
MediumSafe::NumericalScalarCollection MediumSafe::solve(const NumericalMathFunction & function,
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
  // If the origin is in the failure domain, this strategy does not try to find another root
  if (infValue == value)
  {
    result.add(infPoint);
    LOGDEBUG(OSS() << "MediumSafe::solve: roots=" << result);
    return result;
  }
  const NumericalScalar maximumDistance(getMaximumDistance());
  NumericalScalar stepSize(getStepSize());
  Solver solver(getSolver());
  while(infPoint < maximumDistance)
  {
    const NumericalScalar supPoint(std::min(infPoint + stepSize, maximumDistance));
    const NumericalScalar supValue(function(NumericalPoint(1, supPoint))[0]);
    // With this strategy, we stop after the first zero found
    if ((infValue - value) * (supValue - value) < 0.0)
    {
      result.add(solver.solve(function, value, infPoint, supPoint, infValue, supValue));
      LOGDEBUG(OSS() << "MediumSafe::solve: roots=" << result);
      return result;
    }
    infPoint = supPoint;
    infValue = supValue;
  }
  LOGDEBUG(OSS() << "MediumSafe::solve: roots=" << result);
  return result;
}

/* String converter */
String MediumSafe::__repr__() const
{
  OSS oss;
  oss << "class=" << MediumSafe::GetClassName()
      << " derived from " << RootStrategyImplementation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
