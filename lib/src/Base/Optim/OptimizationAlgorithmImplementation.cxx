//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithmImplementation implements an algorithm for solving an optimization problem
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

#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationAlgorithmImplementation)

static const Factory<OptimizationAlgorithmImplementation> Factory_OptimizationAlgorithmImplementation;

/* Default constructor */
OptimizationAlgorithmImplementation::OptimizationAlgorithmImplementation()
  : PersistentObject()
  , progressCallback_(std::make_pair<ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<StopCallback, void *>(0, 0))
  , startingPoint_(Point(0))
  , maximumIterationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumIterationNumber"))
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumEvaluationNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError"))
  , verbose_(false)
  , isLagrangeMultipliersEnabled_(true)
{
  // Nothing to do
}

/*
 * @brief Standard constructor: the optimization problem is managed by the optimization solver, and the actual solver is in charge to check if it is able to solve it.
 */
OptimizationAlgorithmImplementation::OptimizationAlgorithmImplementation(const OptimizationProblem & problem)
  : PersistentObject()
  , progressCallback_(std::make_pair<ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<StopCallback, void *>(0, 0))
  , problem_(problem)
  , maximumIterationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumIterationNumber"))
  , maximumEvaluationNumber_(ResourceMap::GetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumEvaluationNumber"))
  , maximumAbsoluteError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError"))
  , maximumRelativeError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError"))
  , maximumResidualError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError"))
  , maximumConstraintError_(ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError"))
  , verbose_(false)
  , isLagrangeMultipliersEnabled_(true)
{
  // Nothing to do
}

/* Starting point accessor */
Point OptimizationAlgorithmImplementation::getStartingPoint() const
{
  return startingPoint_;
}

/* Starting point accessor */
void OptimizationAlgorithmImplementation::setStartingPoint(const Point & startingPoint)
{
  startingPoint_ = startingPoint;
}

/* Result accessor */
OptimizationResult OptimizationAlgorithmImplementation::getResult() const
{
  return result_;
}

/* Result accessor */
void OptimizationAlgorithmImplementation::setResult(const OptimizationResult & result)
{
  result_ = result;
}

/* Maximum iterations number accessor */
UnsignedInteger OptimizationAlgorithmImplementation::getMaximumIterationNumber() const
{
  return maximumIterationNumber_;
}

/* Maximum iterations number accessor */
void OptimizationAlgorithmImplementation::setMaximumIterationNumber(const UnsignedInteger maximumIterationNumber)
{
  maximumIterationNumber_ = maximumIterationNumber;
}

void OptimizationAlgorithmImplementation::setMaximumEvaluationNumber(const UnsignedInteger maximumEvaluationNumber)
{
  maximumEvaluationNumber_ = maximumEvaluationNumber;
}

UnsignedInteger OptimizationAlgorithmImplementation::getMaximumEvaluationNumber() const
{
  return maximumEvaluationNumber_;
}

/* Maximum absolute error accessor */
Scalar OptimizationAlgorithmImplementation::getMaximumAbsoluteError() const
{
  return maximumAbsoluteError_;
}

/* Maximum absolute error accessor */
void OptimizationAlgorithmImplementation::setMaximumAbsoluteError(const Scalar maximumAbsoluteError)
{
  maximumAbsoluteError_ = maximumAbsoluteError;
}

/* Maximum relative error accessor */
Scalar OptimizationAlgorithmImplementation::getMaximumRelativeError() const
{
  return maximumRelativeError_;
}

/* Maximum relative error accessor */
void OptimizationAlgorithmImplementation::setMaximumRelativeError(const Scalar maximumRelativeError)
{
  maximumRelativeError_ = maximumRelativeError;
}

/* Maximum residual error accessor */
Scalar OptimizationAlgorithmImplementation::getMaximumResidualError() const
{
  return maximumResidualError_;
}

/* Maximum residual error accessor */
void OptimizationAlgorithmImplementation::setMaximumResidualError(const Scalar maximumResidualError)
{
  maximumResidualError_ = maximumResidualError;
}

/* Maximum constraint error accessor */
Scalar OptimizationAlgorithmImplementation::getMaximumConstraintError() const
{
  return maximumConstraintError_;
}

/* Maximum constraint error accessor */
void OptimizationAlgorithmImplementation::setMaximumConstraintError(const Scalar maximumConstraintError)
{
  maximumConstraintError_ = maximumConstraintError;
}

/* String converter */
String OptimizationAlgorithmImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationAlgorithmImplementation::GetClassName()
      << " problem=" << problem_
      << " startingPoint=" << startingPoint_
      << " maximumIterationNumber=" << maximumIterationNumber_
      << " maximumEvaluationNumber=" << maximumEvaluationNumber_
      << " maximumAbsoluteError=" << maximumAbsoluteError_
      << " maximumRelativeError=" << maximumRelativeError_
      << " maximumResidualError=" << maximumResidualError_
      << " maximumConstraintError=" << maximumConstraintError_
      << " verbose=" << verbose_;
  return oss;
}

/* Problem accessor */
OptimizationProblem OptimizationAlgorithmImplementation::getProblem() const
{
  return problem_;
}

void OptimizationAlgorithmImplementation::setProblem(const OptimizationProblem & problem)
{
  checkProblem(problem);
  problem_ = problem;
}

/* Performs the actual checks. Must be overloaded by the actual optimisation algorithm */
void OptimizationAlgorithmImplementation::checkProblem(const OptimizationProblem & ) const
{
  throw NotYetImplementedException(HERE) << "In OptimizationAlgorithmImplementation::checkProblem()";
}

/* Performs the actual computation. Must be overloaded by the actual optimisation algorithm */
void OptimizationAlgorithmImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In OptimizationAlgorithmImplementation::run()";
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
Point OptimizationAlgorithmImplementation::computeLagrangeMultipliers(const Point & x) const
{
  const UnsignedInteger equalityDimension = problem_.getEqualityConstraint().getOutputDimension();
  const UnsignedInteger inequalityDimension = problem_.getInequalityConstraint().getOutputDimension();
  const UnsignedInteger boundDimension = problem_.getBounds().getDimension();
  // If no constraint
  if (equalityDimension + inequalityDimension + boundDimension == 0) return Point(0);
  // Here we have to compute the Lagrange multipliers as the solution of a linear problem with rhs=[d/dx(C_eq) | d/dx(x-lb)^+ | d/dx(ub - x)^+ | d/dx(C_ineq^+)] and lhs=-d/dx(J)
  const UnsignedInteger inputDimension = x.getDimension();
  // Get the lhs as a Point
  const Point lhs(Point(*problem_.getObjective().gradient(x).getImplementation()) * (-1.0));
  // In order to ease the construction of the rhs matrix, we use its internal storage representation as a Point in column-major storage.
  Point rhs(0);
  // First, the equality constraints. Each scalar equality constraint gives a column in the rhs
  if (equalityDimension > 0)
    rhs.add(*problem_.getEqualityConstraint().gradient(x).getImplementation());
  // Second, the bounds
  if (boundDimension > 0)
  {
    // First the lower bounds
    const Point lowerBounds(problem_.getBounds().getLowerBound());
    for (UnsignedInteger i = 0; i < boundDimension; ++i)
    {
      Point boundGradient(inputDimension);
      // Check if the current lower bound is active up to the tolerance
      if (std::abs(x[i] - lowerBounds[i]) <= getMaximumConstraintError())
        boundGradient[i] = 1.0;
      rhs.add(boundGradient);
    } // Lower bounds
    // Second the upper bounds
    const Point upperBounds(problem_.getBounds().getUpperBound());
    for (UnsignedInteger i = 0; i < boundDimension; ++i)
    {
      Point boundGradient(inputDimension);
      // Check if the current lower bound is active up to the tolerance
      if (std::abs(upperBounds[i] - x[i]) <= getMaximumConstraintError())
        boundGradient[i] = -1.0;
      rhs.add(boundGradient);
    } // Upper bounds
  } // boundDimension > 0
  // Third, the inequality constraints
  if (inequalityDimension > 0)
  {
    Point inequality(problem_.getInequalityConstraint()(x));
    Matrix gradientInequality(problem_.getInequalityConstraint().gradient(x));
    for (UnsignedInteger i = 0; i < inequalityDimension; ++i)
    {
      // Check if the current inequality constraint is active up to the tolerance
      if (std::abs(inequality[i]) <= getMaximumConstraintError())
        rhs.add(*gradientInequality.getColumn(i).getImplementation());
      else
        rhs.add(Point(inputDimension));
    }
  } // Inequality constraints
  return Matrix(inputDimension, rhs.getDimension() / inputDimension, rhs).solveLinearSystem(lhs, false);
}

/* Enable/disable lagrange multipliers */
void OptimizationAlgorithmImplementation::enableLagrangeMultipliers(const Bool isLagrangeMultipliersEnabled)
{
  isLagrangeMultipliersEnabled_ = isLagrangeMultipliersEnabled;
}

Bool OptimizationAlgorithmImplementation::isLagrangeMultipliersEnabled() const
{
  return isLagrangeMultipliersEnabled_;
}

/* Virtual constructor */
OptimizationAlgorithmImplementation * OptimizationAlgorithmImplementation::clone() const
{
  return new OptimizationAlgorithmImplementation(*this);
}

/* Verbose accessor */
Bool OptimizationAlgorithmImplementation::getVerbose() const
{
  return verbose_;
}

/* Verbose accessor */
void OptimizationAlgorithmImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

/* Method save() stores the object through the StorageManager */
void OptimizationAlgorithmImplementation::save(Advocate & adv) const
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
  adv.saveAttribute( "isLagrangeMultipliersEnabled_", isLagrangeMultipliersEnabled_);
}


/* Method load() reloads the object from the StorageManager */
void OptimizationAlgorithmImplementation::load(Advocate & adv)
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
  adv.loadAttribute( "isLagrangeMultipliersEnabled_", isLagrangeMultipliersEnabled_);
}


void OptimizationAlgorithmImplementation::setProgressCallback(ProgressCallback callBack, void * state)
{
  progressCallback_ = std::pair<ProgressCallback, void *>(callBack, state);
}


void OptimizationAlgorithmImplementation::setStopCallback(StopCallback callBack, void * state)
{
  stopCallback_ = std::pair<StopCallback, void *>(callBack, state);
}


END_NAMESPACE_OPENTURNS
