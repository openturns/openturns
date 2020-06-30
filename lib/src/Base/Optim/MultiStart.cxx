//                                               -*- C++ -*-
/**
 *  @brief MultiStart optimization algorithm
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
#include "openturns/MultiStart.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Box.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultiStart)

static const Factory<MultiStart> Factory_MultiStart;

/* Constructor with parameters */
MultiStart::MultiStart()
  : OptimizationAlgorithmImplementation()
  , solver_(new Cobyla)
  , startingPoints_()
  , keepResults_(ResourceMap::GetAsBool("MultiStart-KeepResults"))
  , resultCollection_(0)
{
  // Nothing to do here
}


/* Constructor with parameters */
MultiStart::MultiStart(const OptimizationAlgorithm & solver,
                       const Sample & startingPoints)
  : OptimizationAlgorithmImplementation(solver.getProblem())
  , solver_(solver)
  , startingPoints_(startingPoints)
  , keepResults_(ResourceMap::GetAsBool("MultiStart-KeepResults"))
  , resultCollection_(0)
{
  // Nothing to do here
}


void MultiStart::setProblem(const OptimizationProblem & problem)
{
  OptimizationAlgorithmImplementation::setProblem(problem);
  solver_.setProblem(problem);
}


/* Check whether this problem can be solved by this solver.  Must be overloaded by the actual optimisation algorithm */
void MultiStart::checkProblem(const OptimizationProblem & ) const
{
  // Nothing to do
}


void MultiStart::run()
{
  if (startingPoints_.getDimension() != getProblem().getDimension())
    throw InvalidArgumentException(HERE) << "The starting points dimension must match the problem dimension";

  // run the solver with each starting point
  OptimizationAlgorithm solver(solver_);
  resultCollection_.clear();
  Scalar bestValue = getProblem().isMinimization() ? SpecFunc::MaxScalar : -SpecFunc::MaxScalar;
  const UnsignedInteger size = startingPoints_.getSize();
  const UnsignedInteger initialEvaluationNumber = getProblem().getObjective().getEvaluationCallsNumber();
  UnsignedInteger evaluationNumber = 0;
  UnsignedInteger successNumber = 0;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    solver.setStartingPoint(startingPoints_[i]);
    solver.enableLagrangeMultipliers(isLagrangeMultipliersEnabled());
    solver.setMaximumEvaluationNumber(getMaximumEvaluationNumber() - evaluationNumber);
    try
    {
      solver.run();
    }
    catch (Exception &)
    {
      continue;
    }

    const OptimizationResult result(solver.getResult());
    if (keepResults_) resultCollection_.add(result);
    Scalar currentValue = result.getOptimalValue()[0];
    if ((getProblem().isMinimization() && (currentValue < bestValue))
        || (!getProblem().isMinimization() && (currentValue > bestValue)))
    {
      bestValue = currentValue;
      setResult(result);
      LOGINFO(OSS() << "Best initial point so far=" << result.getOptimalPoint() << " value=" << result.getOptimalValue());
      ++successNumber;
    }

    evaluationNumber += getProblem().getObjective().getEvaluationCallsNumber() - initialEvaluationNumber;
    if (evaluationNumber > getMaximumEvaluationNumber())
    {
      break;
    }

    // callbacks
    if (progressCallback_.first)
    {
      progressCallback_.first((100.0 * evaluationNumber) / getMaximumEvaluationNumber(), progressCallback_.second);
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
  LOGINFO(OSS() << successNumber << " out of " << size << " local searches succeeded");

  if (successNumber == 0)
  {
    throw InternalException(HERE) << "None of the local searches succeeded.";
  }
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
      << " startingPoints=" << startingPoints_
      << " keepResults=" << keepResults_;
  return oss;
}

void MultiStart::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

OptimizationAlgorithm MultiStart::getOptimizationAlgorithm() const
{
  return solver_;
}


void MultiStart::setStartingPoints(const Sample & startingPoints)
{
  startingPoints_ = startingPoints;
}


Sample MultiStart::getStartingPoints() const
{
  return startingPoints_;
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
  adv.saveAttribute("startingPoints_", startingPoints_);
  adv.saveAttribute("keepResults_", keepResults_);
  adv.saveAttribute("resultCollection_", resultCollection_);
}

/* Method load() reloads the object from the StorageManager */
void MultiStart::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("solver_", solver_);
  adv.loadAttribute("startingPoints_", startingPoints_);
  adv.loadAttribute("keepResults_", keepResults_);
  adv.loadAttribute("resultCollection_", resultCollection_);
}

END_NAMESPACE_OPENTURNS
