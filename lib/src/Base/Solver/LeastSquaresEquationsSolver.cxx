//                                               -*- C++ -*-
/**
 *  @brief Implementation class of an unbounded solver for systems of non-linear equations based on least square optimization
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

#include "openturns/LeastSquaresEquationsSolver.hxx"
#include "openturns/LeastSquaresProblem.hxx"
#include "openturns/Log.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymbolicFunction.hxx"

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
  // LeastSquares problem
  LeastSquaresProblem problem(SymbolicFunction("x", "x"));
  solver_ = OptimizationAlgorithm::Build(problem);
  solver_.setMaximumCallsNumber(maximumCallsNumber);
  solver_.setMaximumAbsoluteError(absoluteError);
  solver_.setMaximumRelativeError(relativeError);
  solver_.setMaximumResidualError(residualError);
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

void LeastSquaresEquationsSolver::setOptimizationAlgorithm(const OptimizationAlgorithm & algorithm)
{
  solver_ = algorithm;
}

OptimizationAlgorithm LeastSquaresEquationsSolver::getOptimizationAlgorithm() const
{
  return solver_;
}

/* Solve attempt to find one root to the system of non-linear equations function(x) = 0
   given a starting point x with a least square optimization method.
*/
Point LeastSquaresEquationsSolver::solve(const Function & function,
    const Point & startingPoint) const
{
  const Interval bounds;
  return solve(function, startingPoint, bounds);
}

/* Solve attempt to find one root to the system of non-linear equations function(x) = 0
   given a starting point x with a least square optimization method.
*/
Point LeastSquaresEquationsSolver::solve(const Function & function,
    const Point & startingPoint,
    const Interval & bounds) const
{
  LeastSquaresProblem lsqProblem(function);
  const UnsignedInteger boundsDimension = bounds.getDimension();
  if (boundsDimension == function.getInputDimension())
    lsqProblem.setBounds(bounds);
  if ((boundsDimension > 0) && (boundsDimension != function.getInputDimension()))
    throw InvalidArgumentException(HERE) << "Bounds should be of dimension 0 or dimension = " << function.getInputDimension()
                                         << ". Here bounds's dimension = " << boundsDimension;
  OptimizationAlgorithm solver(solver_);
  solver.setStartingPoint(startingPoint);
  try
  {
    solver.setProblem(lsqProblem);
  }
  catch (const InvalidArgumentException &)
  {
    LOGWARN("Default optimization algorithm could not solve the least squares problem. Trying to set up a new one...");
    solver = OptimizationAlgorithm::Build(lsqProblem);
  }
  solver.setProblem(lsqProblem);
  solver.run();
  callsNumber_ = solver.getResult().getCallsNumber();
  const Point min_value_obtained = solver.getResult().getOptimalValue();
  if (  getResidualError() < min_value_obtained[0])
    throw InternalException(HERE) << "Error: solver did not find a solution that satisfies the threshold, here obtained residual=" << min_value_obtained[0];
  const Point result = solver.getResult().getOptimalPoint();
  return result;
}

/* Method save() stores the object through the StorageManager */
void LeastSquaresEquationsSolver::save(Advocate & adv) const
{
  SolverImplementation::save(adv);
  adv.saveAttribute( "solver_", solver_ );
}

/* Method load() reloads the object from the StorageManager */
void LeastSquaresEquationsSolver::load(Advocate & adv)
{
  SolverImplementation::load(adv);
  adv.loadAttribute( "solver_", solver_ );
}

END_NAMESPACE_OPENTURNS
