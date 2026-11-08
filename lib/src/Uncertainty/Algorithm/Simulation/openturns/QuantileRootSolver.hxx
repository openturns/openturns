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
#ifndef OPENTURNS_QUANTILEROOTSOLVER_HXX
#define OPENTURNS_QUANTILEROOTSOLVER_HXX

#include <functional>

#include "openturns/OTprivate.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileRootSolver
 *
 * Internal bisection solver shared by the root-finding quantile
 * algorithms.  The objective is a decreasing scalar function h(t); the
 * solver first brackets h(t)=0 by geometric expansion around a pilot
 * value and then bisects the bracket.  The common random numbers are
 * owned by the caller: every evaluation of h must reuse the same
 * underlying direction or line sample, which turns the stochastic
 * probability estimate into a smooth random function of t.
 *
 * The class is an implementation detail and is not exposed to Python.
 */
class QuantileRootSolver
{
public:
  typedef std::function<Scalar(Scalar)> FunctionType;

  /** Default constructor */
  QuantileRootSolver();

  /** Constructor with parameters */
  explicit QuantileRootSolver(const Scalar tolerance,
                              const UnsignedInteger maximumIteration = 100);

  /** Bracket h(t)=0 by geometric expansion around a start value */
  void bracket(const FunctionType & function,
               const Scalar start,
               Scalar & lowerBound,
               Scalar & upperBound,
               Scalar & lowerValue,
               Scalar & upperValue,
               const Scalar initialStep,
               const UnsignedInteger maximumExpansion = 40) const;

  /** Bisect a bracket where the function is non-negative at the lower
      bound and non-positive at the upper bound */
  Scalar solveBisection(const FunctionType & function,
                        Scalar lowerBound,
                        Scalar upperBound,
                        Scalar lowerValue,
                        Scalar upperValue,
                        Scalar & residual,
                        Interval & bracket,
                        UnsignedInteger & iterationCount,
                        Bool & converged) const;

  /** Tolerance accessor */
  void setTolerance(const Scalar tolerance);
  Scalar getTolerance() const;

  /** Maximum iteration accessor */
  void setMaximumIteration(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumIteration() const;

  /** String converter */
  String __repr__() const;

private:
  Scalar tolerance_ = 1.0e-4;
  UnsignedInteger maximumIteration_ = 100;

}; /* class QuantileRootSolver */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILEROOTSOLVER_HXX */
