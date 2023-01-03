//                                               -*- C++ -*-
/**
 *  @brief Implementation class of the scalar nonlinear solver based on
 *         the Brent mixed bisection/linear/inverse quadratic interpolation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Brent.hxx"
#include <cmath>
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class Brent
 *
 * This class is an interface for the 1D nonlinear Brents
 */

CLASSNAMEINIT(Brent)

static const Factory<Brent> Factory_Brent;

/* Parameter constructor */
Brent::Brent(const Scalar absoluteError,
             const Scalar relativeError,
             const Scalar residualError,
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
Scalar Brent::solve(const UniVariateFunction & function,
                    const Scalar value,
                    const Scalar infPoint,
                    const Scalar supPoint,
                    const Scalar infValue,
                    const Scalar supValue) const
{
  /* We transform the equation function(x) = value into function(x) - value = 0 */
  UnsignedInteger usedFunctionEvaluation = 0;
  const UnsignedInteger maximumFunctionEvaluation = getMaximumFunctionEvaluation();
  volatile Scalar a = infPoint;
  Scalar fA = infValue - value;
  if (std::abs(fA) <= getResidualError()) return a;
  volatile Scalar b = supPoint;
  Scalar fB = supValue - value;
  if (std::abs(fB) <= getResidualError()) return b;
  if ((fA <= 0.0) == (fB <= 0.0)) throw InternalException(HERE) << "Error: Brent method requires that the function takes different signs at the endpoints of the given starting interval, here infPoint=" << infPoint << ", supPoint=" << supPoint << ", value=" << value << ", f(infPoint) - value=" << fA << " and f(supPoint) - value=" << fB;
  volatile Scalar c = a;
  Scalar fC = fA;
  // Main loop
  for (;;)
  {
    // Interval length
    const Scalar oldDelta = b - a;

    // B will be the best approximation
    if (std::abs(fC) < std::abs(fB))
    {
      a = b;
      b = c;
      c = a;
      fA = fB;
      fB = fC;
      fC = fA;
    }
    // Current error on the root
    const Scalar error = getRelativeError() * std::abs(b) + getAbsoluteError();

    // Bisection step
    Scalar newDelta = 0.5 * (c - b);

    // If the current approximation of the root is good enough, return it
    if (std::abs(newDelta) <= error) break;

    // Try an interpolation if the last improvement was large enough
    if ((std::abs(oldDelta) >= error)  && (std::abs(fA) > std::abs(fB)))
    {
      // The new increment for the root will be p / q with p > 0
      Scalar p = -1.0;
      Scalar q = -1.0;
      const Scalar cb = c - b;

      // We can just perform a linear inverse interpolation here
      if (a == c)
      {
        const Scalar slopeBA = fB / fA;
        p = cb * slopeBA;
        q = 1.0 - slopeBA;
      }
      // Here we can perform an inverse quadratic interpolation
      else
      {
        const Scalar slopeAC = fA / fC;
        const Scalar slopeBC = fB / fC;
        const Scalar slopeBA = fB / fA;
        p = slopeBA * (cb * slopeAC * (slopeAC - slopeBC) - (b - a) * (slopeBC - 1.0));
        q = (slopeAC - 1.0) * (slopeBC - 1.0) * (slopeBA - 1.0);
      }
      // Ensure p > 0
      if (p > 0) q = -q;
      else p = -p;

      // Check that the increment obtained from the interpolation is not too large and will lead to an approximation well within [b, c]
      if ((p < (0.75 * cb * q - 0.5 * std::abs(error * q))) && (p < 0.5 * std::abs(oldDelta * q))) newDelta = p / q;
    } // end interpolation

    // The increment must be at least with a magnitude equals to error
    if (std::abs(newDelta) < error)
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
    fB = function(b) - value;
    ++usedFunctionEvaluation;
    // If the current approximation of the root is good enough, return it
    if (std::abs(fB) <= getResidualError()) break;
    // Enforce that the root is within [b, c]
    if ((fB < 0.0) == (fC < 0.0))
    {
      c = a;
      fC = fA;
    }
  } // end Brent loop
  usedFunctionEvaluation_ = usedFunctionEvaluation;
  return b;
}

END_NAMESPACE_OPENTURNS
