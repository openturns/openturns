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

#include "openturns/LeastSquaresEquationsSolver.hxx"
#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/Log.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include <cmath>
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LeastSquaresEquationsSolver
 *
 * This class is an interface for the nonlinear LeastSquaresEquationsSolver
 */

CLASSNAMEINIT(LeastSquaresEquationsSolver)

static const Factory<LeastSquaresEquationsSolver> Factory_LeastSquaresEquationsSolver;

/* Parameter constructor */
LeastSquaresEquationsSolver::LeastSquaresEquationsSolver(const Scalar absoluteError,
               const Scalar relativeError,
               const Scalar residualError,
               const UnsignedInteger maximumCallsNumber)
  : SolverImplementation(absoluteError, relativeError, residualError, maximumCallsNumber)
{
  // Nothing to do
}

/* Virtual constructor */
LeastSquaresEquationsSolver * LeastSquaresEquationsSolver::clone() const
{
  return new LeastSquaresEquationsSolver(*this);
}

/* String converter */
String LeastSquaresEquationsSolver::__repr__() const
{
  OSS oss;
  oss << "class=" << LeastSquaresEquationsSolver::GetClassName()
      << " derived from " << SolverImplementation::__repr__();
  return oss;
}

/* Solve attempt to find one root to the system of non-linear equations function(x) = 0 given a starting point x with a least square optimization method.
*/
Point LeastSquaresEquationsSolver::solve(const Function & function,
                     const Point & startingPoint) const
{ 
  UnsignedInteger callsNumber = 0;
  const UnsignedInteger maximumCallsNumber = getMaximumCallsNumber();
  const Scalar absoluteError = getAbsoluteError();
  const Scalar relativeError = getRelativeError();
  const Scalar residualError = getResidualError();
  LeastSquaresProblem lsqProblem(function);
  OptimizationAlgorithm lsqAlgorithm = OptimizationAlgorithm().Build(lsqProblem);
  lsqAlgorithm.setStartingPoint(startingPoint);
  lsqAlgorithm.setMaximumCallsNumber(maximumCallsNumber);
  lsqAlgorithm.setMaximumAbsoluteError(absoluteError);
  lsqAlgorithm.setMaximumRelativeError(relativeError);
  lsqAlgorithm.setMaximumResidualError(residualError); 

  lsqAlgorithm.run();
  callsNumber = lsqAlgorithm.getResult().getCallsNumber();
  callsNumber_ = callsNumber;
  const Point min_value_obtained = lsqAlgorithm.getResult().getOptimalValue();
  if (  residualError < min_value_obtained[0]) throw InternalException(HERE) << "Error: solver did not find a solution that satisfies the threshold, here obtained residual=" << min_value_obtained[0];
  const Point result = lsqAlgorithm.getResult().getOptimalPoint();

  return result;
}

END_NAMESPACE_OPENTURNS