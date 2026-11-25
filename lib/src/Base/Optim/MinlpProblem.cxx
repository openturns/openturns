//                                               -*- C++ -*-
/**
 *  @brief MinlpProblem implements the solver independent part of a
 * mixed-integer nonlinear problem.
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
#include "openturns/MinlpProblem.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Constructor with parameters */
MinlpProblem::MinlpProblem(const OptimizationProblem & problem,
                           const Point & startingPoint,
                           const UnsignedInteger maximumCallsNumber,
                           const Scalar maximumTimeDuration,
                           const std::chrono::steady_clock::time_point & t0)
  : problem_(problem)
  , startingPoint_(startingPoint)
  , evaluationInputHistory_(0, problem.getDimension())
  , evaluationOutputHistory_(0, 1)
  , maximumCallsNumber_(maximumCallsNumber)
  , maximumTimeDuration_(maximumTimeDuration)
  , t0_(t0)
  , progressCallback_(std::make_pair<OptimizationAlgorithmImplementation::ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<OptimizationAlgorithmImplementation::StopCallback, void *>(0, 0))
{
  initializeConstraints();
}

/* Problem description */
UnsignedInteger MinlpProblem::getDimension() const
{
  return problem_.getDimension();
}

const OptimizationProblem & MinlpProblem::getProblem() const
{
  return problem_;
}

const Point & MinlpProblem::getStartingPoint() const
{
  return startingPoint_;
}

/* Build the normalized constraint view */
void MinlpProblem::initializeConstraints()
{
  if (!problem_.isLinear())
  {
    hasEqualityConstraint_ = problem_.hasEqualityConstraint();
    if (hasEqualityConstraint_)
    {
      equalityConstraint_ = problem_.getEqualityConstraint();
      equalityDimension_ = equalityConstraint_.getOutputDimension();
    }
    hasInequalityConstraint_ = problem_.hasInequalityConstraint();
    if (hasInequalityConstraint_)
    {
      inequalityConstraint_ = problem_.getInequalityConstraint();
      inequalityDimension_ = inequalityConstraint_.getOutputDimension();
    }
    return;
  }

  // A structured linear problem stores its constraints as L <= A.x <= U
  const Matrix coefficients(problem_.getLinearConstraintCoefficients());
  const Interval bounds(problem_.getLinearConstraintBounds());
  const UnsignedInteger dimension = problem_.getDimension();
  const UnsignedInteger nbRows = coefficients.getNbRows();
  if (nbRows != bounds.getDimension())
    throw InvalidDimensionException(HERE) << "The linear constraint matrix has "
        << nbRows << " rows but the constraint bounds have dimension "
        << bounds.getDimension();

  // Count the normalized constraints: a row with equal finite bounds is an
  // equality, otherwise each finite bound gives one inequality h(x) >= 0
  UnsignedInteger equalityDimension = 0;
  UnsignedInteger inequalityDimension = 0;
  for (UnsignedInteger i = 0; i < nbRows; ++ i)
  {
    const Bool finiteLower = bounds.getFiniteLowerBound()[i];
    const Bool finiteUpper = bounds.getFiniteUpperBound()[i];
    if (finiteLower && finiteUpper && (bounds.getLowerBound()[i] == bounds.getUpperBound()[i]))
      ++ equalityDimension;
    else
    {
      if (finiteLower)
        ++ inequalityDimension;
      if (finiteUpper)
        ++ inequalityDimension;
    }
  }

  if (equalityDimension > 0)
  {
    Point constant(equalityDimension);
    Matrix linear(equalityDimension, dimension);
    UnsignedInteger row = 0;
    for (UnsignedInteger i = 0; i < nbRows; ++ i)
    {
      const Bool finiteLower = bounds.getFiniteLowerBound()[i];
      const Bool finiteUpper = bounds.getFiniteUpperBound()[i];
      if (finiteLower && finiteUpper && (bounds.getLowerBound()[i] == bounds.getUpperBound()[i]))
      {
        constant[row] = -bounds.getLowerBound()[i];
        for (UnsignedInteger j = 0; j < dimension; ++ j)
          linear(row, j) = coefficients(i, j);
        ++ row;
      }
    }
    equalityConstraint_ = LinearFunction(Point(dimension), constant, linear);
    equalityDimension_ = equalityDimension;
    hasEqualityConstraint_ = true;
  }

  if (inequalityDimension > 0)
  {
    Point constant(inequalityDimension);
    Matrix linear(inequalityDimension, dimension);
    UnsignedInteger row = 0;
    for (UnsignedInteger i = 0; i < nbRows; ++ i)
    {
      const Bool finiteLower = bounds.getFiniteLowerBound()[i];
      const Bool finiteUpper = bounds.getFiniteUpperBound()[i];
      if (finiteLower && finiteUpper && (bounds.getLowerBound()[i] == bounds.getUpperBound()[i]))
        continue;
      if (finiteLower)
      {
        // A.x - L >= 0
        constant[row] = -bounds.getLowerBound()[i];
        for (UnsignedInteger j = 0; j < dimension; ++ j)
          linear(row, j) = coefficients(i, j);
        ++ row;
      }
      if (finiteUpper)
      {
        // U - A.x >= 0
        constant[row] = bounds.getUpperBound()[i];
        for (UnsignedInteger j = 0; j < dimension; ++ j)
          linear(row, j) = -coefficients(i, j);
        ++ row;
      }
    }
    inequalityConstraint_ = LinearFunction(Point(dimension), constant, linear);
    inequalityDimension_ = inequalityDimension;
    hasInequalityConstraint_ = true;
  }
}

Bool MinlpProblem::hasEqualityConstraint() const
{
  return hasEqualityConstraint_;
}

Bool MinlpProblem::hasInequalityConstraint() const
{
  return hasInequalityConstraint_;
}

UnsignedInteger MinlpProblem::getEqualityDimension() const
{
  return equalityDimension_;
}

UnsignedInteger MinlpProblem::getInequalityDimension() const
{
  return inequalityDimension_;
}

Function MinlpProblem::getEqualityConstraint() const
{
  return equalityConstraint_;
}

Function MinlpProblem::getInequalityConstraint() const
{
  return inequalityConstraint_;
}

OptimizationProblem MinlpProblem::getNormalizedProblem() const
{
  if (!problem_.isLinear())
    return problem_;

  OptimizationProblem normalized(problem_.getObjective());
  if (hasEqualityConstraint_)
    normalized.setEqualityConstraint(equalityConstraint_);
  if (hasInequalityConstraint_)
    normalized.setInequalityConstraint(inequalityConstraint_);
  if (problem_.hasBounds())
    normalized.setBounds(problem_.getBounds());
  for (UnsignedInteger i = 0; i < problem_.getObjective().getOutputDimension(); ++ i)
    normalized.setMinimization(problem_.isMinimization(i), i);
  normalized.setVariablesType(problem_.getVariablesType());
  return normalized;
}

/* Structured accessors */
Bool MinlpProblem::isLinear() const
{
  return problem_.isLinear();
}

Point MinlpProblem::getLinearCost() const
{
  return problem_.getLinearCost();
}

Matrix MinlpProblem::getLinearConstraintCoefficients() const
{
  return problem_.getLinearConstraintCoefficients();
}

Interval MinlpProblem::getLinearConstraintBounds() const
{
  return problem_.getLinearConstraintBounds();
}

/* Record an evaluation failure */
Bool MinlpProblem::evaluationFailed(const Point & x, const String & message)
{
  lastErrorMessage_ = message;
  LOGINFO(OSS() << "MINLP solver went to an abnormal point x=" << x.__str__() << " msg=" << message);
  return false;
}

/* Check the time limit, the callbacks and the call budget */
Bool MinlpProblem::checkTermination()
{
  // Enforce the time limit even before the solver time limit can be taken into account
  const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0_).count();
  if ((maximumTimeDuration_ > 0.0) && (timeDuration > maximumTimeDuration_))
  {
    timeOut_ = true;
    return false;
  }

  // Check callbacks
  if (progressCallback_.first && (maximumCallsNumber_ > 0))
    progressCallback_.first(100.0 * evaluationInputHistory_.getSize() / maximumCallsNumber_, progressCallback_.second);
  if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
  {
    stopped_ = true;
    return false;
  }

  if (evaluationInputHistory_.getSize() > maximumCallsNumber_)
  {
    callLimitReached_ = true;
    return false;
  }
  return true;
}

Bool MinlpProblem::evaluateObjective(const Point & x, Scalar & value)
{
  try
  {
    const Point y(problem_.getObjective()(x));
    evaluationInputHistory_.add(x);
    evaluationOutputHistory_.add(y);
    value = y[0];
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return checkTermination();
}

Bool MinlpProblem::evaluateEqualityConstraint(const Point & x, Point & values)
{
  try
  {
    values = hasEqualityConstraint_ ? equalityConstraint_(x) : Point(0);
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::evaluateInequalityConstraint(const Point & x, Point & values)
{
  try
  {
    values = hasInequalityConstraint_ ? inequalityConstraint_(x) : Point(0);
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::evaluateConstraintMarginal(const UnsignedInteger marginalIndex,
                                              const Point & x, Scalar & value)
{
  if (marginalIndex < equalityDimension_)
  {
    try
    {
      value = equalityConstraint_.getMarginal(marginalIndex)(x)[0];
    }
    catch (const std::exception & exc)
    {
      return evaluationFailed(x, exc.what());
    }
    return true;
  }
  if (marginalIndex < equalityDimension_ + inequalityDimension_)
  {
    try
    {
      value = inequalityConstraint_.getMarginal(marginalIndex - equalityDimension_)(x)[0];
    }
    catch (const std::exception & exc)
    {
      return evaluationFailed(x, exc.what());
    }
    return true;
  }
  throw InvalidArgumentException(HERE) << "Constraint marginal index (" << marginalIndex
                                       << ") is out of range, equality dimension=" << equalityDimension_
                                       << ", inequality dimension=" << inequalityDimension_;
}

Bool MinlpProblem::evaluateObjectiveGradient(const Point & x, Matrix & gradient)
{
  try
  {
    gradient = problem_.getObjective().gradient(x);
    ++ gradientCallsNumber_;
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::evaluateEqualityConstraintGradient(const Point & x, Matrix & jacobian)
{
  try
  {
    jacobian = hasEqualityConstraint_ ? equalityConstraint_.gradient(x) : Matrix(problem_.getDimension(), 0);
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::evaluateInequalityConstraintGradient(const Point & x, Matrix & jacobian)
{
  try
  {
    jacobian = hasInequalityConstraint_ ? inequalityConstraint_.gradient(x) : Matrix(problem_.getDimension(), 0);
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::evaluateConstraintMarginalGradient(const UnsignedInteger marginalIndex,
                                                      const Point & x, Matrix & gradient)
{
  if (marginalIndex < equalityDimension_)
  {
    try
    {
      gradient = equalityConstraint_.getMarginal(marginalIndex).gradient(x);
    }
    catch (const std::exception & exc)
    {
      return evaluationFailed(x, exc.what());
    }
    return true;
  }
  if (marginalIndex < equalityDimension_ + inequalityDimension_)
  {
    try
    {
      gradient = inequalityConstraint_.getMarginal(marginalIndex - equalityDimension_).gradient(x);
    }
    catch (const std::exception & exc)
    {
      return evaluationFailed(x, exc.what());
    }
    return true;
  }
  throw InvalidArgumentException(HERE) << "Constraint marginal index (" << marginalIndex
                                       << ") is out of range, equality dimension=" << equalityDimension_
                                       << ", inequality dimension=" << inequalityDimension_;
}

Bool MinlpProblem::evaluateObjectiveHessian(const Point & x, SymmetricMatrix & hessian)
{
  try
  {
    hessian = problem_.getObjective().hessian(x).getSheet(0);
    ++ hessianCallsNumber_;
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::evaluateEqualityConstraintHessian(const Point & x, SymmetricTensor & hessian)
{
  try
  {
    hessian = hasEqualityConstraint_ ? equalityConstraint_.hessian(x)
              : SymmetricTensor(problem_.getDimension(), 0);
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::evaluateInequalityConstraintHessian(const Point & x, SymmetricTensor & hessian)
{
  try
  {
    hessian = hasInequalityConstraint_ ? inequalityConstraint_.hessian(x)
              : SymmetricTensor(problem_.getDimension(), 0);
  }
  catch (const std::exception & exc)
  {
    return evaluationFailed(x, exc.what());
  }
  return true;
}

Bool MinlpProblem::checkBudget() const
{
  if (timeOut_ || callLimitReached_ || stopped_)
    return false;
  return evaluationInputHistory_.getSize() < maximumCallsNumber_;
}

/* Callbacks */
void MinlpProblem::setProgressCallback(OptimizationAlgorithmImplementation::ProgressCallback callBack,
                                       void * state)
{
  progressCallback_ = std::pair<OptimizationAlgorithmImplementation::ProgressCallback, void *>(callBack, state);
}

void MinlpProblem::setStopCallback(OptimizationAlgorithmImplementation::StopCallback callBack,
                                   void * state)
{
  stopCallback_ = std::pair<OptimizationAlgorithmImplementation::StopCallback, void *>(callBack, state);
}

/* Termination diagnostics */
Bool MinlpProblem::isTimeOut() const
{
  return timeOut_;
}

Bool MinlpProblem::isCallLimitReached() const
{
  return callLimitReached_;
}

Bool MinlpProblem::isStopped() const
{
  return stopped_;
}

const String & MinlpProblem::getLastErrorMessage() const
{
  return lastErrorMessage_;
}

/* History and counters */
Sample MinlpProblem::getInputHistory() const
{
  return evaluationInputHistory_;
}

Sample MinlpProblem::getOutputHistory() const
{
  return evaluationOutputHistory_;
}

UnsignedInteger MinlpProblem::getGradientCallsNumber() const
{
  return gradientCallsNumber_;
}

UnsignedInteger MinlpProblem::getHessianCallsNumber() const
{
  return hessianCallsNumber_;
}

END_NAMESPACE_OPENTURNS
