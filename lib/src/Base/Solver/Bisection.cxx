//                                               -*- C++ -*-
/**
 *  @brief Implementation class of the scalar nonlinear solver based on
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Bisection.hxx"
#include <cmath>
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Bisection
 *
 * This class is an interface for the 1D nonlinear Bisections
 */

CLASSNAMEINIT(Bisection)

static const Factory<Bisection> Factory_Bisection;

/* Parameter constructor */
Bisection::Bisection(const Scalar absoluteError,
                     const Scalar relativeError,
                     const Scalar residualError,
                     const UnsignedInteger maximumCallsNumber)
  : SolverImplementation(absoluteError, relativeError, residualError, maximumCallsNumber)
{
  // Nothing to do
}

/* Virtual constructor */
Bisection * Bisection::clone() const
{
  return new Bisection(*this);
}

/* String converter */
String Bisection::__repr__() const
{
  OSS oss;
  oss << "class=" << Bisection::GetClassName()
      << " derived from " << SolverImplementation::__repr__();
  return oss;
}

/* Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) with the bisection method */
Scalar Bisection::solve(const UniVariateFunction & function,
                        const Scalar value,
                        const Scalar infPoint,
                        const Scalar supPoint,
                        const Scalar infValue,
                        const Scalar supValue) const
{
  /* We transform the equation function(x) = value into function(x) - value = 0 */
  UnsignedInteger callsNumber = 0;
  const UnsignedInteger maximumCallsNumber = getMaximumCallsNumber();
  /* We transform function(x) = value into function(x) - value = 0 */
  Scalar a = infPoint;
  Scalar fA = infValue - value;
  if (std::abs(fA) <= getResidualError()) return a;
  Scalar b = supPoint;
  Scalar fB = supValue - value;
  if (std::abs(fB) <= getResidualError()) return b;
  if (!((fA <= 0.0) != (fB <= 0.0))) throw InternalException(HERE) << "Error: bisection method requires that the function takes different signs at the endpoints of the given starting interval, here infPoint=" << infPoint << ", supPoint=" << supPoint << ", value=" << value << ", f(infPoint) - value=" << fA << " and f(supPoint) - value=" << fB;
  Scalar c = a;
  Scalar fC = fA;
  // Main loop
  for (;;)
  {
    // Current error on the root
    const Scalar error = getRelativeError() * std::abs(c) + getAbsoluteError();
    // Mid-point step
    const Scalar delta = 0.5 * (b - a);
    // c is now the mid-point
    c = a + delta;
    // If the bracketing interval is small enough, return its center
    if (std::abs(delta) <= error) break;
    // If all the evaluation budget has been spent, return the approximation
    if (callsNumber == maximumCallsNumber) break;
    // New evaluation
    fC = function(c) - value;
    ++callsNumber;
    // If the absolute value of the function is small enough, c is a root
    if (std::abs(fC) <= getResidualError()) break;
    // If the function takes a value at middle on the same side of value that at left
    if ((fC > 0.0) == (fA > 0.0))
    {
      a = c;
      fA = fC;
    }
    else
    {
      b = c;
      fB = fC;
    }
  } // end Bisection loop
  callsNumber_ = callsNumber;
  return c;
}

END_NAMESPACE_OPENTURNS
