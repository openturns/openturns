//                                               -*- C++ -*-
/**
 *  @brief Find the roots in a given direction according to the Proban
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/RiskyAndFast.hxx"
#include "openturns/Point.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RiskyAndFast
 */

CLASSNAMEINIT(RiskyAndFast)

static const Factory<RiskyAndFast> Factory_RiskyAndFast;

/* Constructor with parameters */
RiskyAndFast::RiskyAndFast():
  RootStrategyImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
RiskyAndFast::RiskyAndFast(const Solver & solver):
  RootStrategyImplementation(solver)
{
  // Nothing to do
}

/* Constructor with parameters */
RiskyAndFast::RiskyAndFast(const Solver & solver,
                           const Scalar maximumDistance):
  RootStrategyImplementation(solver, maximumDistance, 0.0)
{
  // Nothing to do
}

/* Virtual constructor */
RiskyAndFast * RiskyAndFast::clone() const
{
  return new RiskyAndFast(*this);
}

/* Solve gives all the roots found applying the root strategy */
RiskyAndFast::ScalarCollection RiskyAndFast::solve(const Function & function,
    const Scalar value)
{
  ScalarCollection result(0);
  const Scalar infPoint = 0.0;
  Scalar infValue = 0.0;
  // Get the value of the function at the origin
  try
  {
    infValue = getOriginValue();
  }
  // If it has not yet been computed, compute it and store it
  catch (const NotDefinedException &)
  {
    infValue = function(Point(1, infPoint))[0];
    setOriginValue(infValue);
  }
  const Scalar supPoint = getMaximumDistance();
  const Scalar supValue = function(Point(1, supPoint))[0];
  Solver solver(getSolver());
  // If the origin is in the failure domain we don't look for additional roots
  if (infValue == value)
  {
    result.add(infPoint);
    LOGDEBUG(OSS() << "RiskyAndFast::solve: roots=" << result);
    return result;
  }
  // If there is a sign change on [0, maximumDistance]
  if ((infValue - value) * (supValue - value) < 0.0) result.add(solver.solve(function, value, infPoint, supPoint, infValue, supValue));
  LOGDEBUG(OSS() << "RiskyAndFast::solve: roots=" << result);
  return result;
}

/* String converter */
String RiskyAndFast::__repr__() const
{
  OSS oss;
  oss << "class=" << RiskyAndFast::GetClassName()
      << " derived from " << RootStrategyImplementation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
