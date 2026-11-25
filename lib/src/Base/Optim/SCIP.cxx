//                                               -*- C++ -*-
/**
 *  @brief SCIP allows one to describe a MINLP optimization algorithm
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
#include "openturns/SCIP.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

#ifdef OPENTURNS_HAVE_SCIP
#include "openturns/SCIPProblem.hxx"
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <scip/scip_nlpi.h>
#include <chrono>
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SCIP)

static const Factory<SCIP> Factory_SCIP;

/* Default constructor */
SCIP::SCIP()
  : OptimizationAlgorithmImplementation()
{
  // One branch-and-bound node per iteration: MIP needs a much larger budget
  // than the default number of iterations of the smooth solvers.
  setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("SCIP-DefaultMaximumCallsNumber"));
  setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("SCIP-DefaultMaximumIterationNumber"));
}

/* Constructor with parameters */
SCIP::SCIP(const OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)
{
  setMaximumCallsNumber(ResourceMap::GetAsUnsignedInteger("SCIP-DefaultMaximumCallsNumber"));
  setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("SCIP-DefaultMaximumIterationNumber"));
  checkProblem(getProblem());
}

/* Name and version of the LP solver embedded in the SCIP library */
String SCIP::GetLPsolver()
{
#ifdef OPENTURNS_HAVE_SCIP
  return SCIPlpiGetSolverName();
#else
  return "";
#endif
}

/* Names of the NLP solvers embedded in the SCIP library */
Description SCIP::GetNLPsolvers()
{
  Description nlpSolvers;
#ifdef OPENTURNS_HAVE_SCIP
  // The NLP solvers are plugins, they show up once the default ones are included
  ::SCIP * scip = nullptr;
  if (SCIPcreate(&scip) == SCIP_OKAY && scip != nullptr)
  {
    if (SCIPincludeDefaultPlugins(scip) == SCIP_OKAY)
    {
      const int numberOfNlpis = SCIPgetNNlpis(scip);
      ::SCIP_NLPI ** nlpis = SCIPgetNlpis(scip);
      for (int i = 0; i < numberOfNlpis; ++ i)
        nlpSolvers.add(SCIPnlpiGetName(nlpis[i]));
    }
    SCIPfree(&scip);
  }
#endif
  return nlpSolvers;
}

/* Virtual constructor */
SCIP * SCIP::clone() const
{
  return new SCIP(*this);
}

/* Check whether this problem can be solved by this solver */
void SCIP::checkProblem(const OptimizationProblem & problem) const
{
  // Cannot solve multi-objective problems
  if (problem.hasMultipleObjective())
    throw InvalidArgumentException(HERE) << "SCIP does not support multi-objective optimization";

  // No LeastSquaresProblem / NearestPointProblem
  if (problem.hasResidualFunction() || problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "SCIP does not support least squares or nearest point problems";
}

/* Performs the actual computation */
void SCIP::run()
{
  result_ = OptimizationResult(getProblem());
#ifdef OPENTURNS_HAVE_SCIP
  const UnsignedInteger dimension = getProblem().getDimension();
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "No problem has been set.";
  checkProblem(getProblem());

  // Check the starting point, an empty one means no warm start
  const Point startingPoint(getStartingPoint());
  if ((startingPoint.getDimension() != 0) && (startingPoint.getDimension() != dimension))
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << startingPoint.getDimension()
                                         << "), expected " << dimension;

  const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  SCIPProblem scipProblem(getProblem(), startingPoint,
                          getMaximumCallsNumber(), getMaximumTimeDuration(),
                          getMaximumIterationNumber(), getMaximumConstraintError(), t0);
  scipProblem.setProgressCallback(progressCallback_.first, progressCallback_.second);
  scipProblem.setStopCallback(stopCallback_.first, stopCallback_.second);
  scipProblem.run();

  const Sample inputHistory(scipProblem.getInputHistory());
  try
  {
    if (inputHistory.getSize() > 0)
      setResultFromEvaluationHistory(inputHistory, scipProblem.getOutputHistory(),
                                     scipProblem.hasInequalityConstraint() ? scipProblem.getInequalityConstraint()(inputHistory) : Sample(),
                                     scipProblem.hasEqualityConstraint() ? scipProblem.getEqualityConstraint()(inputHistory) : Sample(),
                                     scipProblem.getNormalizedProblem());
  }
  catch (const InvalidArgumentException &)
  {
    // No feasible point has been evaluated, the SCIP incumbent is reported below
  }

  // Report the SCIP incumbent when the history does not contain it
  if (scipProblem.hasSolution() && (result_.getOptimalPoint().getDimension() == 0))
  {
    result_.setOptimalPoint(scipProblem.getOptimalPoint());
    result_.setOptimalValue(Point(1, scipProblem.getOptimalValue()));
  }

  OptimizationResult::Status status = scipProblem.getStatus();
  if ((status == OptimizationResult::SUCCESS) && (result_.getOptimalPoint().getDimension() == 0))
    status = OptimizationResult::FAILURE;
  result_.setStatus(status);
  result_.setStatusMessage(scipProblem.getStatusMessage());
  result_.setIterationNumber(scipProblem.getIterationNumber());
  result_.setGradientCallsNumber(scipProblem.getGradientCallsNumber());
  result_.setHessianCallsNumber(scipProblem.getHessianCallsNumber());
  const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  result_.setTimeDuration(std::chrono::duration<Scalar>(t1 - t0).count());

  if (result_.getStatus() != OptimizationResult::SUCCESS)
  {
    if (getCheckStatus())
      throw InternalException(HERE) << "Solving problem by SCIP method failed (" << result_.getStatusMessage() << ")";
    else
      LOGWARN(OSS() << "SCIP algorithm failed. The error message is " << result_.getStatusMessage());
  }
#else
  result_.setStatus(OptimizationResult::FAILURE);
  throw NotYetImplementedException(HERE) << "No SCIP support";
#endif
}

/* Description of object */
String SCIP::__str__(const String &) const
{
  OSS oss(false);
  oss << "class=" << getClassName();
  return oss;
}

String SCIP::__repr__() const
{
  OSS oss(false);
  oss << __str__();
  oss << "\noptions=\n";

  // List user-defined options
  const std::vector<String> keys(ResourceMap::GetKeys());
  for (const String & key : keys)
    if ((key.substr(0, 5) == "SCIP-") && (key != "SCIP-DefaultMaximumCallsNumber")
        && (key != "SCIP-DefaultMaximumIterationNumber"))
    {
      const String optionName(key.substr(5));
      const String type(ResourceMap::GetType(key));
      if (type == "str")
        oss << optionName << "=" << ResourceMap::GetAsString(key) << "\n";
      else if (type == "float")
        oss << optionName << "=" << ResourceMap::GetAsScalar(key) << "\n";
      else if (type == "int")
        oss << optionName << "=" << ResourceMap::GetAsUnsignedInteger(key) << "\n";
      else if (type == "bool")
        oss << optionName << "=" << ResourceMap::GetAsBool(key) << "\n";
      else
        throw InvalidArgumentException(HERE) << "Unsupported type " << type << " for SCIP option " << optionName;
    }

  return oss;
}

/* Method save() stores the object through the StorageManager */
void SCIP::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void SCIP::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
