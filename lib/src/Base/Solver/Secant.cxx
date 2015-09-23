//                                               -*- C++ -*-
/**
 *  @brief Implementation class of the scalar nonlinear solver based on
 *         a mixed bisection/secant scheme
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
#include "Secant.hxx"
#include "Log.hxx"
#include <cmath>
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Secant
 *
 * This class is an interface for the 1D nonlinear Secants
 */

CLASSNAMEINIT(Secant);

static const Factory<Secant> RegisteredFactory;

/* Parameter constructor */
Secant::Secant(const NumericalScalar absoluteError,
               const NumericalScalar relativeError,
               const NumericalScalar residualError,
               const UnsignedInteger maximumFunctionEvaluation)
  : SolverImplementation(absoluteError, relativeError, residualError, maximumFunctionEvaluation)
{
  // Nothing to do
}

/* Virtual constructor */
Secant * Secant::clone() const
{
  return new Secant(*this);
}

/* String converter */
String Secant::__repr__() const
{
  OSS oss;
  oss << "class=" << Secant::GetClassName()
      << " derived from " << SolverImplementation::__repr__();
  return oss;
}

/* Solve attempt to find one root to the equation function(x) = value in [infPoint, supPoint] given function(infPoint) and function(supPoint) with a mixed secant/bisection method.
   The code is translated from the function FUNCTION TRICPZ (TA,TB,F1,F2,ER) as found in:
   C     ALGORITHM 626 COLLECTED ALGORITHMS FROM ACM.
   C     ALGORITHM APPEARED IN ACM-TRANS. MATH. SOFTWARE, VOL.10, NO. 4, DEC., 1984,
   C     DEC., 1984, P. 473.
   see http://www.netlib.org/toms/626
*/
NumericalScalar Secant::solve(const NumericalMathFunction & function,
                              const NumericalScalar value,
                              const NumericalScalar infPoint,
                              const NumericalScalar supPoint,
                              const NumericalScalar infValue,
                              const NumericalScalar supValue) const
{
  if ((function.getInputDimension() != 1) || (function.getOutputDimension() != 1)) throw InvalidDimensionException(HERE) << "Error: the secant method requires a scalar function, here input dimension=" << function.getInputDimension() << " and output dimension=" << function.getOutputDimension();
  /* We transform the equation function(x) = value into function(x) - value = 0 */
  UnsignedInteger usedFunctionEvaluation(0);
  const UnsignedInteger maximumFunctionEvaluation(getMaximumFunctionEvaluation());
  NumericalScalar a(infPoint);
  NumericalScalar fA(infValue - value);
  if (fabs(fA) <= getResidualError()) return a;
  NumericalScalar b(supPoint);
  NumericalScalar fB(supValue - value);
  if (fabs(fB) <= getResidualError()) return b;
  if (fA * fB > 0.0) throw InternalException(HERE) << "Error: Secant method requires that the function takes different signs at the endpoints of the given starting interval, here f(infPoint) - value=" << fA << " and f(supPoint) - value=" << fB;
  // p will store the previous approximation
  NumericalScalar c(a);
  NumericalScalar fC(fA);
  // c will store the current approximation
  NumericalScalar s(b);
  NumericalScalar fS(fB);

  // Main loop
  for (;;)
  {
    const NumericalScalar h(0.5 * (b + c));
    const NumericalScalar error(0.5 * getRelativeError() * fabs(c) + 0.5 * getAbsoluteError());
    const NumericalScalar delta(fabs(h - b));
    if (delta < error)
    {
      b = h;
      break;
    }
    // Swap b and c such that fB <= fC
    NumericalScalar y;
    NumericalScalar fY;
    NumericalScalar g;
    NumericalScalar fG;
    if (fabs(fB) < fabs(fC))
    {
      y = s;
      fY = fS;
      g = c;
      fG = fC;
      s = b;
      fS = fB;
    }
    else
    {
      y = b;
      fY = fB;
      g = b;
      fG = fB;
      s = c;
      fS = fC;
    }
    // If we can do a linear interpolation (secant step)
    if (fabs(fY - fS) > getResidualError())
    {
      NumericalScalar e((s * fY - y * fS) / (fY - fS));
      // Step adjustment to avoid spurious fixed point
      if (fabs(e - s) < error) e = s + ((g - s) > 0.0 ? (error) : (-error));
      // If the secant step is not within the current bracketing interval
      if ((e - h) * (s - e) < 0.0) b = h;
      else b = e;
    }
    // Else we do a bisection
    else
    {
      b = h;
    }
    // If all the evaluation budget has been spent, return the approximation
    if (usedFunctionEvaluation == maximumFunctionEvaluation) break;
    // New evaluation
    fB = function(NumericalPoint(1, b))[0] - value;
    ++usedFunctionEvaluation;
    if (fG * fB < 0.0)
    {
      c = g;
      fC = fG;
    }
    else
    {
      c = s;
      fC = fS;
    }
  } // end Secant loop
  usedFunctionEvaluation_ = usedFunctionEvaluation;
  return b;
}

END_NAMESPACE_OPENTURNS
