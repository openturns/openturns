//                                               -*- C++ -*-
/**
 *  @brief MultiStart optimization algorithm
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
#include "openturns/MultiStart.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Cobyla.hxx"

#include <chrono>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultiStart)

static const Factory<MultiStart> Factory_MultiStart;
static const Factory<PersistentCollection<OptimizationResult> > Factory_PersistentCollection_OptimizationResult;

/* Constructor with no parameters */
MultiStart::MultiStart()
  : OptimizationAlgorithmImplementation()
  , solver_(new Cobyla)
  , keepResults_(ResourceMap::GetAsBool("MultiStart-KeepResults"))
  , resultCollection_(0)
{
  // Nothing to do here
}


/* Constructor that sets starting sample */
MultiStart::MultiStart(const OptimizationAlgorithm & solver,
                       const Sample & startingSample)
  : OptimizationAlgorithmImplementation()
  , solver_(solver)
  , startingSample_(startingSample)
  , keepResults_(ResourceMap::GetAsBool("MultiStart-KeepResults"))
  , resultCollection_(0)
{
  checkSolver(solver);

  // no global limit unless the maximum eval number is set
  setMaximumCallsNumber(solver.getMaximumCallsNumber() * startingSample.getSize());

  setProblem(solver.getProblem());
}

/* Check that the solver is compatible with MultiStart */
void MultiStart::checkSolver(const OptimizationAlgorithm & solver) const
{
  if (solver.getImplementation()->getClassName() == "MultiStart")
    throw InvalidArgumentException(HERE) << "Cannot apply MultiStart to a MultiStart.";
}

void MultiStart::setProblem(const OptimizationProblem & problem)
{
  checkStartingSampleConsistentWithOptimizationProblem(startingSample_, problem);
  OptimizationAlgorithmImplementation::setProblem(problem);
  solver_.setProblem(problem);
}


/* Check whether this problem can be solved by this solver */
// Inherited method: never used by MultiStart because solver_ calls its own checkProblem method.
void MultiStart::checkProblem(const OptimizationProblem & ) const
{
  // Nothing to do
}


void MultiStart::run()
{
  if (startingSample_.getSize() == 0) throw InvalidArgumentException(HERE) << "No starting points are set.";
  const UnsignedInteger problemDimension = getProblem().getDimension();
  if (problemDimension == 0) throw InvalidArgumentException(HERE) << "No problem has been set.";
  if (problemDimension != startingSample_.getDimension())
    throw InvalidArgumentException(HERE) << "The starting points dimension (" << startingSample_.getDimension()
                                         << ") and the problem dimension (" << problemDimension << ") do not match.";

  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  Scalar timeDuration = 0.0;

  // run the solver with each starting point
  OptimizationAlgorithm solver(solver_);
  resultCollection_.clear();
  result_ = OptimizationResult(getProblem());
  const UnsignedInteger size = startingSample_.getSize();
  const UnsignedInteger initialCallsNumber = getProblem().getObjective().getCallsNumber();
  UnsignedInteger callsNumber = 0;
  UnsignedInteger successNumber = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    LOGDEBUG(OSS() << "Running local search " << (i + 1) << "/" << size << " x0=" << Point(startingSample_[i]).__str__() << " calls=" << callsNumber << "/" << getMaximumCallsNumber());
    solver.setStartingPoint(startingSample_[i]);

    // ensure we do not exceed the global evaluation budget
    const UnsignedInteger remainingCallsNumber = std::max(static_cast<SignedInteger>(getMaximumCallsNumber() - callsNumber), 0L);
    solver.setMaximumCallsNumber(std::min(remainingCallsNumber, solver_.getMaximumCallsNumber()));

    // ensure we do not exceeed the global time budget
    const Scalar remainingTimeDuration = (getMaximumTimeDuration() > 0.0) ? std::max(getMaximumTimeDuration() - timeDuration, 1e-10) : SpecFunc::MaxScalar;
    solver.setMaximumTimeDuration((solver_.getMaximumTimeDuration() > 0.0) ? std::min(remainingTimeDuration, solver_.getMaximumTimeDuration()) : remainingTimeDuration);

    try
    {
      solver.run();
      const OptimizationResult localResult(solver.getResult());
      if (!localResult.getOptimalPoint().getDimension())
        throw InvalidArgumentException(HERE) << "no feasible point";
      ++ successNumber;
      LOGDEBUG(OSS() << "Local search succeeded with " << localResult.getStatusMessage());

      if (keepResults_) resultCollection_.add(localResult);

      // concatenate result
      const Sample inputHistory(localResult.getInputSample());
      const Sample outputHistory(localResult.getOutputSample());
      const Sample absoluteErrorHistory(localResult.getAbsoluteErrorHistory());
      const Sample relativeErrorHistory(localResult.getRelativeErrorHistory());
      const Sample residualErrorHistory(localResult.getResidualErrorHistory());
      const Sample constraintErrorHistory(localResult.getConstraintErrorHistory());
      const UnsignedInteger localSize = inputHistory.getSize();
      for (UnsignedInteger k = 0; k < localSize; ++ k)
      {
        result_.store(inputHistory[k], outputHistory[k],
                      absoluteErrorHistory(k, 0), relativeErrorHistory(k, 0), residualErrorHistory(k, 0), constraintErrorHistory(k, 0));
      }
      result_.setStatusMessage(localResult.getStatusMessage());
    }
    catch (const Exception & ex)
    {
      LOGDEBUG(OSS() << "Local search failed with " << ex);
    }
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
    result_.setTimeDuration(timeDuration);

    callsNumber = getProblem().getObjective().getCallsNumber() - initialCallsNumber;
    result_.setCallsNumber(callsNumber);

    if (callsNumber > getMaximumCallsNumber())
    {
      result_.setStatus(OptimizationResult::MAXIMUMCALLS);
      result_.setStatusMessage(OSS() << "MultiStart reaches maximum calls number after " << callsNumber << " calls");
      break;
    }

    if ((getMaximumTimeDuration() > 0.0) && (timeDuration > getMaximumTimeDuration()))
    {
      result_.setStatus(OptimizationResult::TIMEOUT);
      result_.setStatusMessage(OSS() << "MultiStart optimization timeout after " << timeDuration << "s");
      break;
    }

    // callbacks
    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * callsNumber) / getMaximumCallsNumber(), progressCallback_.second);
    }
    if (stopCallback_.first)
    {
      Bool stop = stopCallback_.first(stopCallback_.second);
      if (stop)
      {
        LOGWARN(OSS() << "MultiStart was stopped by user");
        break;
      }
    }
  }

  if (!(successNumber > 0))
  {
    result_.setStatus(OptimizationResult::FAILURE);
    if (getCheckStatus())
      throw InternalException(HERE) << "MultiStart: None of the local searches succeeded.";
    else
      LOGWARN(OSS() << "MultiStart: None of the local searches succeeded.");
  }
  LOGINFO(OSS() << successNumber << " out of " << size << " local searches succeeded");
}


/* Virtual constructor */
MultiStart * MultiStart::clone() const
{
  return new MultiStart(*this);
}

/* String converter */
String MultiStart::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " " << OptimizationAlgorithmImplementation::__repr__()
      << " solver=" << solver_
      << " startingSample=" << startingSample_
      << " keepResults=" << keepResults_;
  return oss;
}

/* Accessor to the underlying solver */
void MultiStart::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  checkSolver(solver);
  setProblem(solver.getProblem());
  solver_ = solver;
}

OptimizationAlgorithm MultiStart::getOptimizationAlgorithm() const
{
  return solver_;
}


/* Useless inherited method: throw */
void MultiStart::setStartingPoint(const Point &)
{
  throw NotDefinedException(HERE) << "setStartingPoint makes no sense in a MultiStart context";
}

/* Useless inherited method: throw */
Point MultiStart::getStartingPoint() const
{
  throw NotDefinedException(HERE) << "getStartingPoint makes no sense in a MultiStart context";
}


/* Starting sample accessor */
void MultiStart::setStartingSample(const Sample & startingSample)
{
  checkStartingSampleConsistentWithOptimizationProblem(startingSample, getProblem());
  startingSample_ = startingSample;
}

/* Starting points accessor */
Sample MultiStart::getStartingSample() const
{
  return startingSample_;
}

// Check that the optimization problem is consistent with the starting sample
void MultiStart::checkStartingSampleConsistentWithOptimizationProblem(const Sample & startingSample, const OptimizationProblem & problem) const
{
  const UnsignedInteger problemDimension = problem.getDimension();
  if ( (problemDimension > 0) && (startingSample.getSize() > 0) ) // only perform check if problem is initialized and starting points are already defined
  {
    if (problemDimension != startingSample.getDimension())
    {
      throw InvalidArgumentException(HERE) << "Proposed starting sample has dimension " << startingSample.getDimension()
                                           << ", but the optimization problem has dimension " << problemDimension;
    } // starting sample has the same dimension as the optimization problem

    if (problem.hasBounds())
    {
      const Interval optimizationBounds(problem.getBounds());
      for (UnsignedInteger i = 0; i < startingSample.getSize(); ++ i)
      {
        if (!optimizationBounds.contains(startingSample[i]))
          throw InvalidArgumentException(HERE) << "Optimization bounds inconsistent with starting points of the MultiStart algorithm. \n"
                                               << "The point of index i=" << i << " is \n" << startingSample[i]
                                               << "\n and does not belong to the interval \n" << optimizationBounds;
      } //i
    } // starting sample is consistent with problem optimization bounds (if any)
  }
}

/* Flag for results management accessors */
Bool MultiStart::getKeepResults() const
{
  return keepResults_;
}

void MultiStart::setKeepResults(const Bool keepResults)
{
  keepResults_ = keepResults;
}

MultiStart::OptimizationResultCollection MultiStart::getResultCollection() const
{
  return resultCollection_;
}


/* Method save() stores the object through the StorageManager */
void MultiStart::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("startingSample_", startingSample_);
  adv.saveAttribute("keepResults_", keepResults_);
  adv.saveAttribute("resultCollection_", resultCollection_);
}

/* Method load() reloads the object from the StorageManager */
void MultiStart::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("solver_", solver_);
  if (adv.hasAttribute("startingSample_"))
    adv.loadAttribute("startingSample_", startingSample_);
  else
    adv.loadAttribute("startingPoints_", startingSample_);
  adv.loadAttribute("keepResults_", keepResults_);
  adv.loadAttribute("resultCollection_", resultCollection_);
}

END_NAMESPACE_OPENTURNS
