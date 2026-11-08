//                                               -*- C++ -*-
/**
 *  @brief Shared bisection solver for quantile root-finding algorithms
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>
#include <algorithm>
#include <limits>

#include "openturns/QuantileRootSolver.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Default constructor */
QuantileRootSolver::QuantileRootSolver()
{
  // Nothing to do
}

/* Constructor with parameters */
QuantileRootSolver::QuantileRootSolver(const Scalar tolerance,
                                       const UnsignedInteger maximumIteration)
{
  setTolerance(tolerance);
  setMaximumIteration(maximumIteration);
}

/* Tolerance accessor */
void QuantileRootSolver::setTolerance(const Scalar tolerance)
{
  if (!(tolerance > 0.0) || !std::isfinite(tolerance))
    throw InvalidArgumentException(HERE)
        << "The root-finding tolerance must be finite and positive, but tolerance = "
        << tolerance;
  tolerance_ = tolerance;
}

Scalar QuantileRootSolver::getTolerance() const
{
  return tolerance_;
}

/* Maximum iteration accessor */
void QuantileRootSolver::setMaximumIteration(const UnsignedInteger maximumIteration)
{
  if (maximumIteration == 0)
    throw InvalidArgumentException(HERE)
        << "The maximum iteration count must be positive";
  maximumIteration_ = maximumIteration;
}

UnsignedInteger QuantileRootSolver::getMaximumIteration() const
{
  return maximumIteration_;
}

/* Bracket h(t)=0 by geometric expansion around a start value */
void QuantileRootSolver::bracket(const FunctionType & function,
                                 const Scalar start,
                                 Scalar & lowerBound,
                                 Scalar & upperBound,
                                 Scalar & lowerValue,
                                 Scalar & upperValue,
                                 const Scalar initialStep,
                                 const UnsignedInteger maximumExpansion) const
{
  if (!function)
    throw InvalidArgumentException(HERE) << "The root-finding objective is not set";
  if (!std::isfinite(start))
    throw NotDefinedException(HERE)
        << "The root-finding start value must be finite, but start = " << start;

  Scalar step = (std::isfinite(initialStep) && (initialStep > 0.0)) ? initialStep : 1.0;
  const Scalar startValue = function(start);
  if (!std::isfinite(startValue))
    throw NotDefinedException(HERE)
        << "The root-finding objective is not finite at the start value " << start;

  // h is decreasing.  If h(start) >= 0 we need an upper bound with a
  // non-positive value, otherwise a lower bound with a non-negative value.
  if (startValue >= 0.0)
  {
    lowerBound = start;
    lowerValue = startValue;
    upperBound = start + step;
    Scalar value = function(upperBound);
    UnsignedInteger expansion = 0;
    while (std::isfinite(value) && (value > 0.0) && (expansion < maximumExpansion))
    {
      step *= 2.0;
      upperBound += step;
      value = function(upperBound);
      ++expansion;
    }
    if (!std::isfinite(upperBound) || !std::isfinite(value) || (value > 0.0))
      throw NotDefinedException(HERE)
          << "Unable to bracket the quantile root above the start value " << start
          << " after " << maximumExpansion << " bracket expansions";
    upperValue = value;
  }
  else
  {
    upperBound = start;
    upperValue = startValue;
    lowerBound = start - step;
    Scalar value = function(lowerBound);
    UnsignedInteger expansion = 0;
    while (std::isfinite(value) && (value < 0.0) && (expansion < maximumExpansion))
    {
      step *= 2.0;
      lowerBound -= step;
      value = function(lowerBound);
      ++expansion;
    }
    if (!std::isfinite(lowerBound) || !std::isfinite(value) || (value < 0.0))
      throw NotDefinedException(HERE)
          << "Unable to bracket the quantile root below the start value " << start
          << " after " << maximumExpansion << " bracket expansions";
    lowerValue = value;
  }
}

/* Bisect a bracket where the function is non-negative at the lower
   bound and non-positive at the upper bound */
Scalar QuantileRootSolver::solveBisection(const FunctionType & function,
                                          Scalar lowerBound,
                                          Scalar upperBound,
                                          Scalar lowerValue,
                                          Scalar upperValue,
                                          Scalar & residual,
                                          Interval & bracket,
                                          UnsignedInteger & iterationCount,
                                          Bool & converged) const
{
  if (!function)
    throw InvalidArgumentException(HERE) << "The root-finding objective is not set";
  if (!(lowerBound <= upperBound))
    throw InvalidArgumentException(HERE)
        << "The root-finding bracket is not ordered: lower = " << lowerBound
        << ", upper = " << upperBound;
  if (!(lowerValue >= 0.0))
    throw NotDefinedException(HERE)
        << "The root-finding objective must be non-negative at the lower bound, but h("
        << lowerBound << ") = " << lowerValue;
  if (!(upperValue <= 0.0))
    throw NotDefinedException(HERE)
        << "The root-finding objective must be non-positive at the upper bound, but h("
        << upperBound << ") = " << upperValue;

  iterationCount = 0;
  converged = false;
  Scalar midpoint = 0.5 * (lowerBound + upperBound);
  Scalar midpointValue = 0.0;
  Bool hasMidpointValue = false;
  while (iterationCount < maximumIteration_)
  {
    const Scalar scale = std::max(1.0, std::max(std::abs(lowerBound), std::abs(upperBound)));
    if ((upperBound - lowerBound) <= tolerance_ * scale)
    {
      converged = true;
      break;
    }
    midpoint = 0.5 * (lowerBound + upperBound);
    // The bracket cannot be narrowed below the floating-point resolution.
    if ((midpoint == lowerBound) || (midpoint == upperBound))
    {
      converged = true;
      break;
    }
    midpointValue = function(midpoint);
    hasMidpointValue = true;
    ++iterationCount;
    if (!std::isfinite(midpointValue))
      throw NotDefinedException(HERE)
          << "The root-finding objective is not finite at " << midpoint;
    if (midpointValue >= 0.0)
    {
      lowerBound = midpoint;
      lowerValue = midpointValue;
    }
    else
    {
      upperBound = midpoint;
      upperValue = midpointValue;
    }
  }
  const Scalar estimate = 0.5 * (lowerBound + upperBound);
  // Evaluate the residual at the returned estimate.  It is an extra
  // evaluation but it detects discontinuous probability surfaces, for
  // which the bracket converges to a jump instead of a root.
  if (!hasMidpointValue || (midpoint != estimate))
  {
    midpointValue = function(estimate);
    ++iterationCount;
  }
  residual = midpointValue;
  bracket = Interval(lowerBound, upperBound);
  return estimate;
}

/* String converter */
String QuantileRootSolver::__repr__() const
{
  OSS oss;
  oss << "class=QuantileRootSolver"
      << " tolerance=" << tolerance_
      << " maximumIteration=" << maximumIteration_;
  return oss;
}

END_NAMESPACE_OPENTURNS
