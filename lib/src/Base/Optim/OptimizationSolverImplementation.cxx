//                                               -*- C++ -*-
/**
 *  @brief OptimizationSolverImplementation implements an algorithm for solving an optimization problem
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/OptimizationSolverImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationSolverImplementation);

/* Default constructor */
OptimizationSolverImplementation::OptimizationSolverImplementation()
  : PersistentObject()
  , startingPoint_(NumericalPoint(0))
  , maximumIterationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationSolver-DefaultMaximumIteration"))
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationSolver-DefaultMaximumEvaluationNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumConstraintError"))
  , verbose_(false)
{
  // Nothing to do
}

/*
 * @brief Standard constructor: the optimization problem is managed by the optimization solver, and the actual solver is in charge to check if it is able to solve it.
 */
OptimizationSolverImplementation::OptimizationSolverImplementation(const OptimizationProblem & problem)
  : PersistentObject()
  , problem_(problem)
  , maximumIterationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationSolver-DefaultMaximumIteration"))
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationSolver-DefaultMaximumEvaluationNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsNumericalScalar("OptimizationSolver-DefaultMaximumConstraintError"))
  , verbose_(false)
{
  // Nothing to do
}

/* Starting point accessor */
NumericalPoint OptimizationSolverImplementation::getStartingPoint() const
{
  return startingPoint_;
}

/* Starting point accessor */
void OptimizationSolverImplementation::setStartingPoint(const NumericalPoint & startingPoint)
{
  startingPoint_ = startingPoint;
}

/* Result accessor */
OptimizationResult OptimizationSolverImplementation::getResult() const
{
  return result_;
}

/* Result accessor */
void OptimizationSolverImplementation::setResult(const OptimizationResult & result)
{
  result_ = result;
}

/* Maximum iterations number accessor */
UnsignedInteger OptimizationSolverImplementation::getMaximumIterationNumber() const
{
  return maximumIterationNumber_;
}

/* Maximum iterations number accessor */
void OptimizationSolverImplementation::setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber)
{
  maximumIterationNumber_ = maximumIterationNumber;
}

void OptimizationSolverImplementation::setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber)
{
  maximumEvaluationNumber_ = maximumEvaluationNumber;
}

UnsignedInteger OptimizationSolverImplementation::getMaximumEvaluationNumber() const
{
  return maximumEvaluationNumber_;
}

/* Maximum absolute error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}

/* Maximum absolute error accessor */
void OptimizationSolverImplementation::setMaximumAbsoluteError(const NumericalScalar maximumAbsoluteError)
{
  maximumAbsoluteError_ = maximumAbsoluteError;
}

/* Maximum relative error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}

/* Maximum relative error accessor */
void OptimizationSolverImplementation::setMaximumRelativeError(const NumericalScalar maximumRelativeError)
{
  maximumRelativeError_ = maximumRelativeError;
}

/* Maximum residual error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumResidualError() const
{
  return maximumResidualError_;
}

/* Maximum residual error accessor */
void OptimizationSolverImplementation::setMaximumResidualError(const NumericalScalar maximumResidualError)
{
  maximumResidualError_ = maximumResidualError;
}

/* Maximum constraint error accessor */
NumericalScalar OptimizationSolverImplementation::getMaximumConstraintError() const
{
  return maximumConstraintError_;
}

/* Maximum constraint error accessor */
void OptimizationSolverImplementation::setMaximumConstraintError(const NumericalScalar maximumConstraintError)
{
  maximumConstraintError_ = maximumConstraintError;
}

/* String converter */
String OptimizationSolverImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationSolverImplementation::GetClassName()
      << " problem=" << problem_
      << " startingPoint=" << startingPoint_
      << " maximumIterationNumber=" << maximumIterationNumber_
      << " maximumEvaluationNumber=" << maximumEvaluationNumber_
      << " maximumAbsoluteError=" << maximumAbsoluteError_
      << " maximumRelativeError=" << maximumRelativeError_
      << " maximumResidualError=" << maximumResidualError_
      << " maximumConstraintError=" << maximumConstraintError_
      << " verbose=" << (verbose_ ? "true" : "false");
  return oss;
}

/* Problem accessor */
OptimizationProblem OptimizationSolverImplementation::getProblem() const
{
  return problem_;
}

void OptimizationSolverImplementation::setProblem(const OptimizationProblem & problem)
{
  checkProblem(problem);
  problem_ = problem;
}

/* Performs the actual checks. Must be overloaded by the actual optimisation algorithm */
void OptimizationSolverImplementation::checkProblem(const OptimizationProblem & problem) const
{
  throw NotYetImplementedException(HERE) << "In OptimizationSolverImplementation::checkProblem()";
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void OptimizationSolverImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In OptimizationSolverImplementation::run()";
}

/* Computes the Lagrange multipliers associated with the constraints as a post-processing of the optimal point. Actual algorithms should overload this method. */
/* L(x, l_eq, l_lower_bound, l_upper_bound, l_ineq) = J(x) + l_eq * C_eq(x) + l_lower_bound * (x-lb)^+ + l_upper_bound * (ub-x)^+ + l_ineq * C_ineq^+(x)
   d/dx(L = d/dx(J) + l_eq * d/dx(C_eq) + l_lower_bound * d/dx(x-lb)^+ + l_upper_bound * d/dx(ub - x)^+ + l_ineq * d/dx(C_ineq^+)

   The Lagrange multipliers are stored as [l_eq, l_lower_bounds, l_upper_bounds, l_ineq], where:
   * l_eq is of dimension 0 if no equality constraint, else of dimension the number of scalar equality constraints
   * l_lower_bounds and l_upper_bounds are of dimension 0 if no bound constraint, else of dimension dim(x) for both of them
   * l_ineq is of dimension 0 if no inequality constraint, else of dimension the number of scalar inequality constraints

   so if there is no constraint of any kind, the Lagrange multipliers are of dimension 0.
 */
NumericalPoint OptimizationSolverImplementation::computeLagrangeMultipliers(const NumericalPoint & x) const
{
  const UnsignedInteger equalityDimension = problem_.getEqualityConstraint().getOutputDimension();
  const UnsignedInteger inequalityDimension = problem_.getInequalityConstraint().getOutputDimension();
  const UnsignedInteger boundDimension = problem_.getBounds().getDimension();
  // If no constraint
  if (equalityDimension + inequalityDimension + boundDimension == 0) return NumericalPoint(0);
  // Here we have to compute the Lagrange multipliers as the solution of a linear problem with rhs=[d/dx(C_eq) | d/dx(x-lb)^+ | d/dx(ub - x)^+ | d/dx(C_ineq^+)] and lhs=-d/dx(J)
  const UnsignedInteger inputDimension = x.getDimension();
  // Get the lhs as a NumericalPoint
  const NumericalPoint lhs(NumericalPoint(*problem_.getObjective().gradient(x).getImplementation()) * (-1.0));
  // In order to ease the construction of the rhs matrix, we use its internal storage representation as a NumericalPoint in column-major storage.
  NumericalPoint rhs(0);
  // First, the equality constraints. Each scalar equality constraint gives a column in the rhs
  if (equalityDimension > 0)
    rhs.add(*problem_.getEqualityConstraint().gradient(x).getImplementation());
  // Second, the bounds
  if (boundDimension > 0)
  {
    // First the lower bounds
    const NumericalPoint lowerBounds(problem_.getBounds().getLowerBound());
    for (UnsignedInteger i = 0; i < boundDimension; ++i)
    {
      NumericalPoint boundGradient(inputDimension);
      // Check if the current lower bound is active up to the tolerance
      if (std::abs(x[i] - lowerBounds[i]) <= getMaximumConstraintError())
        boundGradient[i] = 1.0;
      rhs.add(boundGradient);
    } // Lower bounds
    // Second the upper bounds
    const NumericalPoint upperBounds(problem_.getBounds().getUpperBound());
    for (UnsignedInteger i = 0; i < boundDimension; ++i)
    {
      NumericalPoint boundGradient(inputDimension);
      // Check if the current lower bound is active up to the tolerance
      if (std::abs(upperBounds[i] - x[i]) <= getMaximumConstraintError())
        boundGradient[i] = -1.0;
      rhs.add(boundGradient);
    } // Upper bounds
  } // boundDimension > 0
  // Third, the inequality constraints
  if (inequalityDimension > 0)
  {
    NumericalPoint inequality(problem_.getInequalityConstraint()(x));
    Matrix gradientInequality(problem_.getInequalityConstraint().gradient(x));
    for (UnsignedInteger i = 0; i < inequalityDimension; ++i)
    {
      // Check if the current inequality constraint is active up to the tolerance
      if (std::abs(inequality[i]) <= getMaximumConstraintError())
        rhs.add(*gradientInequality.getColumn(i).getImplementation());
      else
        rhs.add(NumericalPoint(inputDimension));
    }
  } // Inequality constraints
  return Matrix(inputDimension, rhs.getDimension() / inputDimension, rhs).solveLinearSystem(lhs, false);
}

/* Virtual constructor */
OptimizationSolverImplementation * OptimizationSolverImplementation::clone() const
{
  return new OptimizationSolverImplementation(*this);
}

/* Verbose accessor */
Bool OptimizationSolverImplementation::getVerbose() const
{
  return verbose_;
}

/* Verbose accessor */
void OptimizationSolverImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

/* Method save() stores the object through the StorageManager */
void OptimizationSolverImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "startingPoint_", startingPoint_);
  adv.saveAttribute( "problem_", problem_);
  adv.saveAttribute( "maximumIterationNumber_", maximumIterationNumber_);
  adv.saveAttribute( "maximumEvaluationNumber_", maximumEvaluationNumber_);
  adv.saveAttribute( "maximumAbsoluteError_", maximumAbsoluteError_);
  adv.saveAttribute( "maximumRelativeError_", maximumRelativeError_);
  adv.saveAttribute( "maximumResidualError_", maximumResidualError_);
  adv.saveAttribute( "maximumConstraintError_", maximumConstraintError_);
  adv.saveAttribute( "verbose_", verbose_);
}


/* Method load() reloads the object from the StorageManager */
void OptimizationSolverImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "startingPoint_", startingPoint_);
  adv.loadAttribute( "problem_", problem_);
  adv.loadAttribute( "maximumIterationNumber_", maximumIterationNumber_);
  adv.loadAttribute( "maximumEvaluationNumber_", maximumEvaluationNumber_);
  adv.loadAttribute( "maximumAbsoluteError_", maximumAbsoluteError_);
  adv.loadAttribute( "maximumRelativeError_", maximumRelativeError_);
  adv.loadAttribute( "maximumResidualError_", maximumResidualError_);
  adv.loadAttribute( "maximumConstraintError_", maximumConstraintError_);
  adv.loadAttribute( "verbose_", verbose_);
}

END_NAMESPACE_OPENTURNS
