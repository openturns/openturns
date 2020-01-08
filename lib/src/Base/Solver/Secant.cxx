//                                               -*- C++ -*-
/**
 *  @brief Implementation class of the scalar nonlinear solver based on
 *         a mixed bisection/secant scheme
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Secant.hxx"
#include "openturns/Log.hxx"
#include <cmath>
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Secant
 *
 * This class is an interface for the 1D nonlinear Secants
 */

CLASSNAMEINIT(Secant)

static const Factory<Secant> Factory_Secant;

/* Parameter constructor */
Secant::Secant(const Scalar absoluteError,
               const Scalar relativeError,
               const Scalar residualError,
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
Scalar Secant::solve(const UniVariateFunction & function,
                     const Scalar value,
                     const Scalar infPoint,
                     const Scalar supPoint,
                     const Scalar infValue,
                     const Scalar supValue) const
{
  /* We transform the equation function(x) = value into function(x) - value = 0 */
  UnsignedInteger usedFunctionEvaluation = 0;
  const UnsignedInteger maximumFunctionEvaluation = getMaximumFunctionEvaluation();
  Scalar a = infPoint;
  Scalar fA = infValue - value;
  if (std::abs(fA) <= getResidualError()) return a;
  Scalar b = supPoint;
  Scalar fB = supValue - value;
  if (std::abs(fB) <= getResidualError()) return b;
  if (!(fA * fB <= 0.0)) throw InternalException(HERE) << "Error: Secant  method requires that the function takes different signs at the endpoints of the given starting interval, here infPoint=" << infPoint << ", supPoint=" << supPoint << ", value=" << value << ", f(infPoint) - value=" << fA << " and f(supPoint) - value=" << fB;
  // p will store the previous approximation
  Scalar c = a;
  Scalar fC = fA;
  // c will store the current approximation
  Scalar s = b;
  Scalar fS = fB;

  // Main loop
  for (;;)
  {
    const Scalar h = 0.5 * (b + c);
    const Scalar error = 0.5 * getRelativeError() * std::abs(c) + 0.5 * getAbsoluteError();
    const Scalar delta = std::abs(h - b);
    if (delta < error)
    {
      b = h;
      break;
    }
    // Swap b and c such that fB <= fC
    Scalar y = -1.0;
    Scalar fY = -1.0;
    Scalar g = -1.0;
    Scalar fG = -1.0;
    if (std::abs(fB) < std::abs(fC))
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
    if (std::abs(fY - fS) > getResidualError())
    {
      Scalar e = (s * fY - y * fS) / (fY - fS);
      // Step adjustment to avoid spurious fixed point
      if (std::abs(e - s) < error) e = s + ((g - s) > 0.0 ? (error) : (-error));
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
    fB = function(b) - value;
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
