//                                               -*- C++ -*-
/**
 *  @brief Implementation class of an unbounded solver for systems of non-linear equations based on least square optimization
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
#ifndef OPENTURNS_LEASTSQUARESEQUATIONSSOLVER_HXX
#define OPENTURNS_LEASTSQUARESEQUATIONSSOLVER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SolverImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LeastSquaresEquationsSolver
 *
 * This class is an interface for the 1D nonlinear solverImplementations
 */
class OT_API LeastSquaresEquationsSolver :
  public SolverImplementation
{
  CLASSNAME
public:

  /** Parameter constructor */
  explicit LeastSquaresEquationsSolver(const Scalar absoluteError = ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                                       const Scalar relativeError = ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                                       const Scalar residualError = ResourceMap::GetAsScalar("Solver-DefaultResidualError"),
                                       const UnsignedInteger maximumFunctionEvaluation = ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation"));


  /** Virtual constructor */
  LeastSquaresEquationsSolver * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Solve attempt to find one root to a system of equations function(x) = 0 given a starting point x_0 */
  using SolverImplementation::solve;
  Point solve(const Function & function,
              const Point & startingPoint) const override;
  Point solve(const Function & function,
              const Point & startingPoint,
              const Interval & bounds) const override;

  /** optimization accessors */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & algorithm);

  /** save/load */
  void save(Advocate & adv) const override;
  void load(Advocate & adv) override;

private:
  /** optimization solver */
  OptimizationAlgorithm solver_;

}; /* Class LeastSquaresEquationsSolver */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LsqSolver_HXX */
