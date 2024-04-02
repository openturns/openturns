//                                               -*- C++ -*-
/**
 *  @brief Implementation class of an unbounded solver for systems of non-linear equations based on least square optimization
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
#ifndef OPENTURNS_LSQSOLVER_HXX
#define OPENTURNS_LSQSOLVER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SolverImplementation.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LsqSolver
 *
 * This class is an interface for the 1D nonlinear solverImplementations
 */
class OT_API LsqSolver :
  public SolverImplementation
{
  CLASSNAME
public:

  /** Parameter constructor */
  explicit LsqSolver(const Scalar absoluteError = ResourceMap::GetAsScalar("Solver-DefaultAbsoluteError"),
                  const Scalar relativeError = ResourceMap::GetAsScalar("Solver-DefaultRelativeError"),
                  const Scalar residualError = ResourceMap::GetAsScalar("Solver-DefaultResidualError"),
                  const UnsignedInteger maximumFunctionEvaluation = ResourceMap::GetAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation"));


  /** Virtual constructor */
  LsqSolver * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Solve attempt to find one root to a system of equations function(x) = 0 given a starting point x_0 */
  using SolverImplementation::solve;
  Point solve(const Function & function,
               const Point & startingPoint) const override;

private:

}; /* Class LsqSolver */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LsqSolver_HXX */
