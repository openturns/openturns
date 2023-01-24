//                                               -*- C++ -*-
/**
 *  @brief OptimizationAlgorithmImplementation implements an algorithm for solving an optimization problem
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SpecFunc.hxx"

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

/* Virtual constructor */  // set the result from evalaution history
  void setResultFromEvaluationHistory(const Sample & inputHistory, const Sample & outputHistory);

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
}


void OptimizationAlgorithmImplementation::setProgressCallback(ProgressCallback callBack, void * state)
{
  progressCallback_ = std::pair<ProgressCallback, void *>(callBack, state);
}


void OptimizationAlgorithmImplementation::setStopCallback(StopCallback callBack, void * state)
{
  stopCallback_ = std::pair<StopCallback, void *>(callBack, state);
}

void OptimizationAlgorithmImplementation::setResultFromEvaluationHistory(
  const Sample & inputHistory, const Sample & outputHistory,
  const Sample & inequalityHistory, const Sample & equalityHistory)
{
  // Update the result
  result_ = OptimizationResult(getProblem());
  const UnsignedInteger size = inputHistory.getSize();
  const UnsignedInteger dimension = getProblem().getDimension();

  Scalar absoluteError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;
  Scalar constraintError = -1.0;

  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point inP(inputHistory[i]);
    const Point outP(outputHistory[i]);
    constraintError = 0.0;
    if (getProblem().hasBounds())
    {
      const Interval bounds(getProblem().getBounds());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        if (bounds.getFiniteLowerBound()[j])
          constraintError = std::max(constraintError, bounds.getLowerBound()[j] - inP[j]);
        if (bounds.getFiniteUpperBound()[j])
          constraintError = std::max(constraintError, inP[j] - bounds.getUpperBound()[j]);
      }
    }
    if (getProblem().hasEqualityConstraint())
    {
      const Point g(equalityHistory[i]);
      constraintError = std::max(constraintError, g.normInf());
    }
    if (getProblem().hasInequalityConstraint())
    {
      Point h(inequalityHistory[i]);
      for (UnsignedInteger k = 0; k < getProblem().getInequalityConstraint().getOutputDimension(); ++ k)
      {
        h[k] = std::min(h[k], 0.0);// convention h(x)>=0 <=> admissibility
      }
      constraintError = std::max(constraintError, h.normInf());
    }
    if (!getProblem().isContinuous())
    {
      const Indices variablesType(getProblem().getVariablesType());
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        switch (variablesType[j])
        {
          case OptimizationProblemImplementation::CONTINUOUS:
            break;
          case OptimizationProblemImplementation::BINARY:
            constraintError = std::max(constraintError, std::abs(inP[j] - SpecFunc::Clip01(std::round(inP[j]))));
            break;
          case OptimizationProblemImplementation::INTEGER:
            constraintError = std::max(constraintError, std::abs(inP[j] - std::round(inP[j])));
            break;
        }
      }
    }

    if (i > 0)
    {
      const Point inPM(inputHistory[i - 1]);
      const Point outPM(outputHistory[i - 1]);
      absoluteError = (inP - inPM).normInf();
      relativeError = (inP.normInf() > 0.0) ? (absoluteError / inP.normInf()) : -1.0;
      residualError = (std::abs(outP[0]) > 0.0) ? (std::abs(outP[0] - outPM[0]) / std::abs(outP[0])) : -1.0;
    }
    result_.store(inP, outP, absoluteError, relativeError, residualError, constraintError, getMaximumConstraintError());
  }
  result_.setEvaluationNumber(size);
}

END_NAMESPACE_OPENTURNS
