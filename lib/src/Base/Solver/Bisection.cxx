//                                               -*- C++ -*-
/**
 *  @brief Implementation class of the scalar nonlinear solver based on
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
#include "openturns/Bisection.hxx"
#include <cmath>
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Bisection
 *
 * This class is an interface for the 1D nonlinear Bisections
 */

CLASSNAMEINIT(Bisection);

static const Factory<Bisection> Factory_Bisection;

/* Parameter constructor */
Bisection::Bisection(const NumericalScalar absoluteError,
                     const NumericalScalar relativeError,
                     const NumericalScalar residualError,
                     const UnsignedInteger maximumFunctionEvaluation)
  : SolverImplementation(absoluteError, relativeError, residualError, maximumFunctionEvaluation)
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
NumericalScalar Bisection::solve(const NumericalMathFunction & function,
                                 const NumericalScalar value,
                                 const NumericalScalar infPoint,
                                 const NumericalScalar supPoint,
                                 const NumericalScalar infValue,
                                 const NumericalScalar supValue) const
{
  if ((function.getInputDimension() != 1) || (function.getOutputDimension() != 1)) throw InvalidDimensionException(HERE) << "Error: the bisection method requires a scalar function, here input dimension=" << function.getInputDimension() << " and output dimension=" << function.getOutputDimension();
  /* We transform the equation function(x) = value into function(x) - value = 0 */
  UnsignedInteger usedFunctionEvaluation = 0;
  const UnsignedInteger maximumFunctionEvaluation = getMaximumFunctionEvaluation();
  /* We transform function(x) = value into function(x) - value = 0 */
  NumericalScalar a = infPoint;
  NumericalScalar fA = infValue - value;
  if (std::abs(fA) <= getResidualError()) return a;
  NumericalScalar b = supPoint;
  NumericalScalar fB = supValue - value;
  if (std::abs(fB) <= getResidualError()) return b;
  if (fA * fB > 0.0) throw InternalException(HERE) << "Error: bisection method requires that the function takes different signs at the endpoints of the given starting interval, here f(infPoint) - value=" << fA << ", f(supPoint) - value=" << fB;
  NumericalScalar c = a;
  NumericalScalar fC = fA;
  // Main loop
  for (;;)
  {
    // Current error on the root
    const NumericalScalar error = 2.0 * getRelativeError() * std::abs(c) + 0.5 * getAbsoluteError();
    // Mid-point step
    const NumericalScalar delta = 0.5 * (b - a);
    // If the current approximation of the root is good enough, return it
    if ((std::abs(delta) <= error) || (std::abs(fC) <= getResidualError())) break;
    // c is now the mid-point
    c = a + delta;
    // If all the evaluation budget has been spent, return the approximation
    if (usedFunctionEvaluation == maximumFunctionEvaluation) break;
    // New evaluation
    fC = function(NumericalPoint(1, c))[0] - value;
    ++usedFunctionEvaluation;
    // If the function takes a value at middle on the same side of value that at left
    if (fC * fA > 0.0)
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
  usedFunctionEvaluation_ = usedFunctionEvaluation;
  return c;
}

END_NAMESPACE_OPENTURNS
