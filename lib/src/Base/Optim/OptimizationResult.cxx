//                                               -*- C++ -*-
/**
 *  @brief OptimizationResult stores the result of a OptimizationAlgorithmImplementation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OptimizationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Curve.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationResult)

static const Factory<OptimizationResult> Factory_OptimizationResult;

/* Default constructor */
OptimizationResult::OptimizationResult()
  : PersistentObject()
{
  absoluteErrorHistory_.setDimension(1);
  relativeErrorHistory_.setDimension(1);
  residualErrorHistory_.setDimension(1);
  constraintErrorHistory_.setDimension(1);
}

/* Default constructor */
OptimizationResult::OptimizationResult(const OptimizationProblem & problem)
  : PersistentObject()
  , problem_(problem)
{
  absoluteErrorHistory_.setDimension(1);
  relativeErrorHistory_.setDimension(1);
  residualErrorHistory_.setDimension(1);
  constraintErrorHistory_.setDimension(1);
  inputHistory_.setDimension(problem.getObjective().getInputDimension());
  outputHistory_.setDimension(problem.getObjective().getOutputDimension());
}

/* Virtual constructor */
OptimizationResult * OptimizationResult::clone() const
{
  return new OptimizationResult(*this);
}

/* Optimal point accessor */
Point OptimizationResult::getOptimalPoint() const
{
  if (getProblem().getObjective().getOutputDimension() > 1)
    throw InvalidArgumentException(HERE) << "No optimal point available for multi-objective";
  return optimalPoint_;
}

void OptimizationResult::setOptimalPoint(const Point & optimalPoint)
{
  optimalPoint_ = optimalPoint;
}

/* Optimal value accessor */
Point OptimizationResult::getOptimalValue() const
{
  if (getProblem().getObjective().getOutputDimension() > 1)
    throw InvalidArgumentException(HERE) << "No optimal value available for multi-objective";
  return optimalValue_;
}

void OptimizationResult::setOptimalValue(const Point & optimalValue)
{
  optimalValue_ = optimalValue;
}

/* Final points accessor */
Sample OptimizationResult::getFinalPoints() const
{
  if (!finalPoints_.getSize())
    return Sample(1, optimalPoint_);
  return finalPoints_;
}

void OptimizationResult::setFinalPoints(const Sample & finalPoints)
{
  finalPoints_ = finalPoints;
}

/* Final values accessor */
Sample OptimizationResult::getFinalValues() const
{
  if (!finalValues_.getSize())
    return Sample(1, optimalValue_);
  return finalValues_;
}

void OptimizationResult::setFinalValues(const Sample & finalValues)
{
  finalValues_ = finalValues;
}

/* Evaluation number accessor */
UnsignedInteger OptimizationResult::getEvaluationNumber() const
{
  return evaluationNumber_;
}

void OptimizationResult::setEvaluationNumber(const UnsignedInteger evaluationNumber)
{
  evaluationNumber_ = evaluationNumber;
}

/* Iteration number accessor */
UnsignedInteger OptimizationResult::getIterationNumber() const
{
  return iterationNumber_;
}

void OptimizationResult::setIterationNumber(const UnsignedInteger iterationNumber)
{
  iterationNumber_ = iterationNumber;
}

/* Absolute error accessor */
Scalar OptimizationResult::getAbsoluteError() const
{
  return absoluteError_;
}

Sample OptimizationResult::getAbsoluteErrorHistory() const
{
  return absoluteErrorHistory_.getSample();
}

/* Absolute error accessor */
void OptimizationResult::setAbsoluteError(const Scalar absoluteError)
{
  absoluteError_ = absoluteError;
}

/* Relative error accessor */
Scalar OptimizationResult::getRelativeError() const
{
  return relativeError_;
}

Sample OptimizationResult::getRelativeErrorHistory() const
{
  return relativeErrorHistory_.getSample();
}

/* Relative error accessor */
void OptimizationResult::setRelativeError(const Scalar relativeError)
{
  relativeError_ = relativeError;
}

/* Residual error accessor */
Scalar OptimizationResult::getResidualError() const
{
  return residualError_;
}

Sample OptimizationResult::getResidualErrorHistory() const
{
  return residualErrorHistory_.getSample();
}

/* Residual error accessor */
void OptimizationResult::setResidualError(const Scalar residualError)
{
  residualError_ = residualError;
}

/* Constraint error accessor */
Scalar OptimizationResult::getConstraintError() const
{
  return constraintError_;
}

Sample OptimizationResult::getConstraintErrorHistory() const
{
  return constraintErrorHistory_.getSample();
}

/* Constraint error accessor */
void OptimizationResult::setConstraintError(const Scalar constraintError)
{
  constraintError_ = constraintError;
}

Sample OptimizationResult::getInputSample() const
{
  return inputHistory_.getSample();
}

Sample OptimizationResult::getOutputSample() const
{
  return outputHistory_.getSample();
}

void OptimizationResult::setProblem(const OptimizationProblem & problem)
{
  problem_ = problem;
}

OptimizationProblem OptimizationResult::getProblem() const
{
  return problem_;
}

/* String converter */
String OptimizationResult::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationResult::GetClassName()
      << " optimal point=" << optimalPoint_
      << " optimal value=" << optimalValue_
      << " evaluationNumber=" << evaluationNumber_
      << " iterationNumber=" << iterationNumber_
      << " absoluteError=" << getAbsoluteError()
      << " relativeError=" << getRelativeError()
      << " residualError=" << getResidualError()
      << " constraintError=" << getConstraintError()
      << " problem=" << problem_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OptimizationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "optimalPoint_", optimalPoint_ );
  adv.saveAttribute( "optimalValue_", optimalValue_ );
  adv.saveAttribute( "evaluationNumber_", evaluationNumber_ );
  adv.saveAttribute( "iterationNumber_", iterationNumber_ );
  adv.saveAttribute( "absoluteError_", absoluteError_ );
  adv.saveAttribute( "relativeError_", relativeError_ );
  adv.saveAttribute( "residualError_", residualError_ );
  adv.saveAttribute( "constraintError_", constraintError_ );

  adv.saveAttribute( "absoluteErrorHistory_", absoluteErrorHistory_ );
  adv.saveAttribute( "relativeErrorHistory_", relativeErrorHistory_ );
  adv.saveAttribute( "residualErrorHistory_", residualErrorHistory_ );
  adv.saveAttribute( "constraintErrorHistory_", constraintErrorHistory_ );

  adv.saveAttribute( "inputHistory_", inputHistory_ );
  adv.saveAttribute( "outputHistory_", outputHistory_ );

  adv.saveAttribute( "problem_", problem_ );
  adv.saveAttribute( "finalPoints_", finalPoints_ );
  adv.saveAttribute( "finalValues_", finalValues_ );
  adv.saveAttribute( "paretoFrontsIndices_", paretoFrontsIndices_ );
}

/* Method load() reloads the object from the StorageManager */
void OptimizationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "optimalPoint_", optimalPoint_ );
  adv.loadAttribute( "optimalValue_", optimalValue_ );
  adv.loadAttribute( "evaluationNumber_", evaluationNumber_ );
  adv.loadAttribute( "iterationNumber_", iterationNumber_ );
  adv.loadAttribute( "absoluteError_", absoluteError_ );
  adv.loadAttribute( "relativeError_", relativeError_ );
  adv.loadAttribute( "residualError_", residualError_ );
  adv.loadAttribute( "constraintError_", constraintError_ );

  adv.loadAttribute( "absoluteErrorHistory_", absoluteErrorHistory_ );
  adv.loadAttribute( "relativeErrorHistory_", relativeErrorHistory_ );
  adv.loadAttribute( "residualErrorHistory_", residualErrorHistory_ );
  adv.loadAttribute( "constraintErrorHistory_", constraintErrorHistory_ );

  adv.loadAttribute( "inputHistory_", inputHistory_ );
  adv.loadAttribute( "outputHistory_", outputHistory_ );

  adv.loadAttribute( "problem_", problem_ );
  if (adv.hasAttribute("finalPoints_"))
  {
    adv.loadAttribute( "finalPoints_", finalPoints_ );
    adv.loadAttribute( "finalValues_", finalValues_ );
    adv.loadAttribute( "paretoFrontsIndices_", paretoFrontsIndices_ );
  }
}

/* Incremental history storage */
void OptimizationResult::store(const Point & x,
                               const Point & y,
                               const Scalar absoluteError,
                               const Scalar relativeError,
                               const Scalar residualError,
                               const Scalar constraintError)
{
  if (getProblem().getObjective().getOutputDimension() <= 1)
  {
    if (!getOptimalValue().getDimension()
        || getProblem().hasLevelFunction() // consider the last value as optimal for nearest-point algos
        || ((getProblem().isMinimization() && y[0] < getOptimalValue()[0])
            || (!getProblem().isMinimization() && y[0] > getOptimalValue()[0])))
    {
      setOptimalPoint(x);
      setOptimalValue(y);
    }
  }

  // update values
  absoluteError_ = absoluteError;
  relativeError_ = relativeError;
  residualError_ = residualError;
  constraintError_ = constraintError;

  // append values
  absoluteErrorHistory_.store(Point(1, absoluteError));
  relativeErrorHistory_.store(Point(1, relativeError));
  residualErrorHistory_.store(Point(1, residualError));
  constraintErrorHistory_.store(Point(1, constraintError));

  inputHistory_.store(x);
  outputHistory_.store(y);
}

Graph OptimizationResult::drawErrorHistory() const
{
  if (getProblem().getObjective().getOutputDimension() > 1)
    throw NotYetImplementedException(HERE) << "drawErrorHistory is not available for multi-objective";
  Graph result("Error history", iterationNumber_ > 0 ? "Iteration number" : "Evaluation number", "Error value", true, "topright", 1.0, GraphImplementation::LOGY);
  result.setGrid(true);
  result.setGridColor("black");
// create a sample with the iteration number to be plotted as x data
  const UnsignedInteger size = getAbsoluteErrorHistory().getSize();
  {
    Sample data(getAbsoluteErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve absoluteErrorCurve( data, "absolute error" );
    absoluteErrorCurve.setLegend("absolute error");
    absoluteErrorCurve.setColor("red");
    result.add( absoluteErrorCurve );
  }
// Relative error
  {
    Sample data(getRelativeErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve relativeErrorCurve( data, "relative error" );
    relativeErrorCurve.setLegend("relative error");
    relativeErrorCurve.setColor("blue");
    result.add( relativeErrorCurve );
  }
// Residual error
  {
    Sample data(getResidualErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve residualErrorCurve( data, "residual error" );
    residualErrorCurve.setLegend("residual error");
    residualErrorCurve.setColor("green");
    result.add( residualErrorCurve );
  }
// Constraint error
  {
    Sample data(getConstraintErrorHistory());
    for (UnsignedInteger i = 0; i < size; ++i) if (data(i, 0) <= 0.0) data(i, 0) = SpecFunc::ScalarEpsilon;
    Curve constraintErrorCurve( data, "constraint error" );
    constraintErrorCurve.setLegend("constraint error");
    constraintErrorCurve.setColor("magenta");
    result.add( constraintErrorCurve );
  }
  result.setYMargin(0.0);// tighten the Y axis
  return result;
}

/* Draw optimal value graph */
Graph OptimizationResult::drawOptimalValueHistory() const
{
  if (getProblem().getObjective().getOutputDimension() > 1)
    throw NotYetImplementedException(HERE) << "drawOptimalValueHistory is not available for multi-objective";
  Graph result("Optimal value history", iterationNumber_ > 0 ? "Iteration number" : "Evaluation number", "Optimal value", true, "topright", 1.0);
  result.setGrid(true);
  result.setGridColor("black");
  Sample data(getOutputSample().getMarginal(0));
  const UnsignedInteger size = data.getSize();
  const Bool minimization = problem_.isMinimization();
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    const UnsignedInteger j = 0;
    if (!((minimization && (data(i, j) < data(i - 1, j)))
          || (!minimization && (data(i, j) > data(i - 1, j)))))
    {
      data(i, j) = data(i - 1, j);
    }
  }
  Curve optimalValueCurve(data, "optimal value");
  optimalValueCurve.setLegend("optimal value");
  optimalValueCurve.setColor("red");
  result.add(optimalValueCurve);
  return result;
}

/* Computes the Lagrange multipliers associated with the constraints as a post-processing of the optimal point. */
/* L(x, l_eq, l_lower_bound, l_upper_bound, l_ineq) = J(x) + l_eq * C_eq(x) + l_lower_bound * (x-lb)^+ + l_upper_bound * (ub-x)^+ + l_ineq * C_ineq^+(x)
   d/dx(L = d/dx(J) + l_eq * d/dx(C_eq) + l_lower_bound * d/dx(x-lb)^+ + l_upper_bound * d/dx(ub - x)^+ + l_ineq * d/dx(C_ineq^+)

   The Lagrange multipliers are stored as [l_eq, l_lower_bounds, l_upper_bounds, l_ineq], where:
   * l_eq is of dimension 0 if no equality constraint, else of dimension the number of scalar equality constraints
   * l_lower_bounds and l_upper_bounds are of dimension 0 if no bound constraint, else of dimension dim(x) for both of them
   * l_ineq is of dimension 0 if no inequality constraint, else of dimension the number of scalar inequality constraints

   so if there is no constraint of any kind, the Lagrange multipliers are of dimension 0.
 */
Point OptimizationResult::computeLagrangeMultipliers(const Point & x) const
{
  if (getProblem().getObjective().getOutputDimension() > 1)
    throw NotYetImplementedException(HERE) << "computeLagrangeMultipliers is not available for multi-objective";
  const Scalar maximumConstraintError = ResourceMap::GetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError");
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
      if (std::abs(x[i] - lowerBounds[i]) <= maximumConstraintError)
        boundGradient[i] = 1.0;
      rhs.add(boundGradient);
    } // Lower bounds
    // Second the upper bounds
    const Point upperBounds(problem_.getBounds().getUpperBound());
    for (UnsignedInteger i = 0; i < boundDimension; ++i)
    {
      Point boundGradient(inputDimension);
      // Check if the current lower bound is active up to the tolerance
      if (std::abs(upperBounds[i] - x[i]) <= maximumConstraintError)
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
      if (std::abs(inequality[i]) <= maximumConstraintError)
        rhs.add(*gradientInequality.getColumn(i).getImplementation());
      else
        rhs.add(Point(inputDimension));
    }
  } // Inequality constraints
  return Matrix(inputDimension, rhs.getDimension() / inputDimension, rhs).solveLinearSystem(lhs, false);
}


Point OptimizationResult::computeLagrangeMultipliers() const
{
  return computeLagrangeMultipliers(getOptimalPoint());
}

/* Pareto fronts accessor */
void OptimizationResult::setParetoFrontsIndices(const IndicesCollection & indices)
{
  paretoFrontsIndices_ = indices;
}

IndicesCollection OptimizationResult::getParetoFrontsIndices() const
{
  if (getProblem().getObjective().getOutputDimension() <= 1)
    throw InvalidArgumentException(HERE) << "No pareto fronts available for mono-objective";
  return paretoFrontsIndices_;
}

END_NAMESPACE_OPENTURNS

