//                                               -*- C++ -*-
/**
 *  @brief Implementation class of the scalar nonlinear solver based on
 *         the Brent mixed bisection/linear/inverse quadratic interpolation
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
#include "Brent.hxx"
#include <cmath>
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class Brent
 *
 * This class is an interface for the 1D nonlinear Brents
 */

CLASSNAMEINIT(Brent);

static const Factory<Brent> RegisteredFactory;

/* Parameter constructor */
Brent::Brent(const NumericalScalar absoluteError,
             const NumericalScalar relativeError,
             const NumericalScalar residualError,
             const UnsignedInteger maximumFunctionEvaluation)
  : SolverImplementation(absoluteError, relativeError, residualError, maximumFunctionEvaluation)
{
  // Nothing to do
}

/* Virtual constructor */
Brent * Brent::clone() const
{
  return new Brent(*this);
}

/* String converter */
String Brent::__repr__() const
{
  OSS oss;
  oss << "class=" << Brent::GetClassName()
      << " derived from " << SolverImplementation::__repr__();
  return oss;
}

/* Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) with the Brent method */
NumericalScalar Brent::solve(const NumericalMathFunction & function,
                             const NumericalScalar value,
                             const NumericalScalar infPoint,
                             const NumericalScalar supPoint,
                             const NumericalScalar infValue,
                             const NumericalScalar supValue) const
{
  if ((function.getInputDimension() != 1) || (function.getOutputDimension() != 1)) throw InvalidDimensionException(HERE) << "Error: Brent's method requires a scalar function, here input dimension=" << function.getInputDimension() << " and output dimension=" << function.getOutputDimension();
  /* We transform the equation function(x) = value into function(x) - value = 0 */
  UnsignedInteger usedFunctionEvaluation(0);
  const UnsignedInteger maximumFunctionEvaluation(getMaximumFunctionEvaluation());
  NumericalScalar a(infPoint);
  NumericalScalar fA(infValue - value);
  if (fabs(fA) <= getResidualError()) return a;
  NumericalScalar b(supPoint);
  NumericalScalar fB(supValue - value);
  if (fabs(fB) <= getResidualError()) return b;
  if (fA * fB > 0.0) throw InternalException(HERE) << "Error: Brent method requires that the function takes different signs at the endpoints of the given starting interval, here f(infPoint) - value=" << fA << " and f(supPoint) - value=" << fB;
  NumericalScalar c(a);
  NumericalScalar fC(fA);
  // Main loop
  for (;;)
  {
    // Interval length
    const NumericalScalar oldDelta(b - a);

    // B will be the best approximation
    if (fabs(fC) < fabs(fB))
    {
      a = b;
      b = c;
      c = a;
      fA = fB;
      fB = fC;
      fC = fA;
    }
    // Current error on the root
    const NumericalScalar error(2.0 * getRelativeError() * fabs(b) + 0.5 * getAbsoluteError());

    // Bisection step
    NumericalScalar newDelta(0.5 * (c - b));

    // If the current approximation of the root is good enough, return it
    if ((fabs(newDelta) <= error) || (fabs(fB) <= getResidualError())) break;

    // Try an interpolation if the last improvement was large enough
    if ((fabs(oldDelta) >= error)  && (fabs(fA) > fabs(fB)))
    {
      // The new increment for the root will be p / q with p > 0
      NumericalScalar p;
      NumericalScalar q;
      const NumericalScalar cb(c - b);

      // We can just perform a linear inverse interpolation here
      if (a == c)
      {
        const NumericalScalar slopeBA(fB / fA);
        p = cb * slopeBA;
        q = 1.0 - slopeBA;
      }
      // Here we can perform an inverse quadratic interpolation
      else
      {
        const NumericalScalar slopeAC(fA / fC);
        const NumericalScalar slopeBC(fB / fC);
        const NumericalScalar slopeBA(fB / fA);
        p = slopeBA * (cb * slopeAC * (slopeAC - slopeBC) - (b - a) * (slopeBC - 1.0));
        q = (slopeAC - 1.0) * (slopeBC - 1.0) * (slopeBA - 1.0);
      }
      // Ensure p > 0
      if (p > 0) q = -q;
      else p = -p;

      // Check that the increment obtained from the interpolation is not too large and will lead to an approximation well within [b, c]
      if ((p < (0.75 * cb * q - 0.5 * fabs(error * q))) && (p < 0.5 * fabs(oldDelta * q))) newDelta = p / q;
    } // end interpolation

    // The increment must be at least with a magnitude equals to error
    if (fabs(newDelta) < error)
    {
      if (newDelta > 0) newDelta = error;
      else newDelta = -error;
    }

    a = b;
    fA = fB;
    b += newDelta;
    // If all the evaluation budget has been spent, return the approximation
    if (usedFunctionEvaluation == maximumFunctionEvaluation) break;
    // New evaluation
    fB = function(NumericalPoint(1, b))[0] - value;
    ++usedFunctionEvaluation;
    // Enforce that the root is within [b, c]
    if (fB * fC > 0.0)
    {
      c = a;
      fC = fA;
    }
  } // end Brent loop
  usedFunctionEvaluation_ = usedFunctionEvaluation;
  return b;
}

END_NAMESPACE_OPENTURNS
