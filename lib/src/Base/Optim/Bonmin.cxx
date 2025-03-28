//                                               -*- C++ -*-
/**
 *  @brief Bonmin allows one to describe a MINLP optimization algorithm
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Bonmin.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"

#ifdef OPENTURNS_HAVE_BONMIN
#include "openturns/BonminProblem.hxx"
#include <BonBonminSetup.hpp>
#include <BonCbc.hpp>
#include <BonminConfig.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Bonmin)

static const Factory<Bonmin> Factory_Bonmin;

/** Constructors */
Bonmin::Bonmin(const String & algoName)
  : OptimizationAlgorithmImplementation()
  , algoName_()
{
  setAlgorithmName(algoName);
}

Bonmin::Bonmin( OptimizationProblem & problem,
                const String & algoName)
  : OptimizationAlgorithmImplementation(problem)
  , algoName_()
{
  setAlgorithmName(algoName);
}

/* Virtual constructor */
Bonmin * Bonmin::clone() const
{
  return new Bonmin(*this);
}

Description Bonmin::GetAlgorithmNames()
{
  const Description algoNames = {"B-BB", "B-OA", "B-QG", "B-Hyb"
#if (BONMIN_VERSION_MAJOR * 100000 + BONMIN_VERSION_MINOR * 100 + BONMIN_VERSION_RELEASE) >= 100809
                                 , "B-Ecp", "B-iFP"
#endif
                                };
  return algoNames;
}

/** Accessors to Bonmin attributes */
void Bonmin::setAlgorithmName(const String & algoName)
{
  // Check algoName
  if (!GetAlgorithmNames().contains(algoName))
    throw InvalidArgumentException(HERE) << "Unknown solver " << algoName;
  algoName_ = algoName;
}

String Bonmin::getAlgorithmName() const
{
  return algoName_;
}


/** Check whether this problem can be solved by this solver. */
void Bonmin::checkProblem(const OptimizationProblem & problem) const
{
  // Cannot solve multi-objective problems
  if (problem.hasMultipleObjective()) throw InvalidArgumentException(HERE) << "Bonmin does not support multi-objective optimization";

  // No LeastSquaresProblem / NearestPointProblem
  if (problem.hasResidualFunction() || problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Bonmin does not support least squares or nearest point problems";
}


#ifdef OPENTURNS_HAVE_BONMIN

/** Accessors to Bonmin options */
static void GetOptionsFromResourceMap(::Ipopt::SmartPtr<::Ipopt::OptionsList> options)
{
//   Get options for Bonmin setup from ResourceMap
//   See Bonmin/Ipopt user manuals for more details.

  std::vector<String> keys(ResourceMap::GetKeys());
  const UnsignedInteger nbKeys = keys.size();

  for (UnsignedInteger i = 0; i < nbKeys; ++i)
    if (keys[i].substr(0, 7) == "Bonmin-")
    {
      String optionName(keys[i].substr(7));
      String type(ResourceMap::GetType(keys[i]));
      Bool ok = false;
      if (type == "str")
        ok = options->SetStringValue(optionName, ResourceMap::GetAsString(keys[i]));
      else if (type == "float")
        ok = options->SetNumericValue(optionName, ResourceMap::GetAsScalar(keys[i]));
      else if (type == "int")
        ok = options->SetIntegerValue(optionName, ResourceMap::GetAsUnsignedInteger(keys[i]));
      else if (type == "bool")
        ok = options->SetStringValue(optionName, ResourceMap::GetAsBool(keys[i]) ? "yes" : "no");
      if (!ok)
        throw InvalidArgumentException(HERE) << "Invalid Bonmin option " << optionName;
    }
}

#endif

/** Performs the actual computation. */
void Bonmin::run()
{
#ifdef OPENTURNS_HAVE_BONMIN
  // Check problem
  checkProblem(getProblem());

  // Check starting point
  if (getStartingPoint().getDimension() != getProblem().getDimension())
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << getStartingPoint().getDimension() << "), expected " << getProblem().getDimension();

  // Create BonminProblem
  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  Ipopt::SmartPtr<BonminProblem> tminlp = new BonminProblem(getProblem(), getStartingPoint(), getMaximumCallsNumber(), getMaximumTimeDuration(), t0);
  tminlp->setProgressCallback(progressCallback_.first, progressCallback_.second);
  tminlp->setStopCallback(stopCallback_.first, stopCallback_.second);

  // Create setup, initialize options
  ::Bonmin::BonminSetup app;
  app.initializeOptionsAndJournalist();
  if (!app.options()->SetStringValue("bonmin.algorithm", algoName_))
    throw InvalidArgumentException(HERE) << "Bonmin: Invalid parameter for bonmin.algorithm";
  if (!app.options()->SetIntegerValue("bonmin.iteration_limit", getMaximumIterationNumber()))
    throw InvalidArgumentException(HERE) << "Bonmin: Invalid parameter for bonmin.iteration_limit";
  if (getMaximumTimeDuration() > 0.0)
    app.options()->SetNumericValue("bonmin.time_limit", getMaximumTimeDuration());
  if (!app.options()->SetStringValue("sb", "yes"))
    throw InvalidArgumentException(HERE) << "Bonmin: Invalid parameter for sb";
  if (!app.options()->SetIntegerValue("print_level", 0))
    throw InvalidArgumentException(HERE) << "Bonmin: Invalid parameter for print_level";
  if (!app.options()->SetStringValue("honor_original_bounds", "yes"))
    throw InvalidArgumentException(HERE) << "Bonmin: Invalid parameter for honor_original_bounds";
  const Description algos = {"bb", "nlp", "lp", "oa", "fp", "milp"};
  for (UnsignedInteger i = 0; i < algos.getSize(); ++ i)
    if (!app.options()->SetIntegerValue("bonmin." + algos[i] + "_log_level", 0))
      throw InvalidArgumentException(HERE) << "Bonmin: Invalid parameter for bonmin." << algos[i] << "_log_level";
  app.options()->SetNumericValue("constr_viol_tol", std::max(getMaximumConstraintError(), SpecFunc::MinScalar));
  app.options()->SetNumericValue("bound_relax_factor", 0.0);
  GetOptionsFromResourceMap(app.options());

  // Update setup with BonminProblem
  try
  {
    app.initialize(GetRawPtr(tminlp));

    // Solve problem
    ::Bonmin::Bab solver;
    solver(app);
  }
  catch (::Bonmin::TNLPSolver::UnsolvedError *exc)
  {
    const Sample inputHistory(tminlp->getInputHistory());
    setResultFromEvaluationHistory(inputHistory, tminlp->getOutputHistory(),
                                   getProblem().hasInequalityConstraint() ? getProblem().getInequalityConstraint()(inputHistory) : Sample(),
                                   getProblem().hasEqualityConstraint() ? getProblem().getEqualityConstraint()(inputHistory) : Sample());

    result_.setStatus(OptimizationResult::FAILURE);
    std::ostringstream oss;
    exc->printError(oss);
    result_.setStatusMessage(oss.str());
    return;
  }
  catch(const ::Bonmin::OsiTMINLPInterface::SimpleError & exc)
  {
    result_.setStatus(OptimizationResult::FAILURE);
    result_.setStatusMessage(exc.message());
    return;
  }
  catch(const CoinError & exc)
  {
    result_.setStatus(OptimizationResult::FAILURE);
    result_.setStatusMessage(exc.message());
    return;
  }

  // print used options
  String optionsLog;
  app.options()->PrintList(optionsLog);
  LOGINFO(optionsLog);

  const Sample inputHistory(tminlp->getInputHistory());
  setResultFromEvaluationHistory(inputHistory, tminlp->getOutputHistory(),
                                 getProblem().hasInequalityConstraint() ? getProblem().getInequalityConstraint()(inputHistory) : Sample(),
                                 getProblem().hasEqualityConstraint() ? getProblem().getEqualityConstraint()(inputHistory) : Sample());

  const ::Bonmin::TMINLP::SolverReturn status = tminlp->getStatus();
  const Description bonminExitStatus = {"SUCCESS", "INFEASIBLE", "CONTINUOUS_UNBOUNDED",
                                        "LIMIT_EXCEEDED", "USER_INTERRUPT", "MINLP_ERROR"
                                       };
  result_.setStatusMessage(bonminExitStatus[status]);
  switch (status)
  {
    case ::Bonmin::TMINLP::SUCCESS:
      break;
    case ::Bonmin::TMINLP::USER_INTERRUPT:
      result_.setStatus(OptimizationResult::INTERRUPTION);
      break;
    default:
      result_.setStatus(OptimizationResult::FAILURE);
      break;
  }
  if (tminlp->getTimeOut())
    result_.setStatus(OptimizationResult::TIMEOUT);

  if (result_.getStatus() != OptimizationResult::SUCCESS)
  {
    if (getCheckStatus())
      throw InternalException(HERE) << "Solving problem by Bonmin method failed (" << result_.getStatusMessage() << ")";
    else
      LOGWARN(OSS() << "Bonmin algorithm failed. The error message is " << result_.getStatusMessage());
  }
#else
  result_.setStatus(OptimizationResult::FAILURE);
  throw NotYetImplementedException(HERE) << "No Bonmin support";

#endif
}


/** Description of object */
String Bonmin::__str__(const String &) const
{
  OSS oss(false);
  oss << "class=" << getClassName()
      << "\nalgorithm=" << algoName_;
  return oss;
}

String Bonmin::__repr__() const
{
  OSS oss(false);
  oss << __str__();
  oss << "\noptions=\n";

  // List user-defined options
  std::vector<String> keys(ResourceMap::GetKeys());
  UnsignedInteger nbKeys = keys.size();

  for (UnsignedInteger i = 0; i < nbKeys; ++i)
    if (keys[i].substr(0, 7) == "Bonmin-")
    {
      String optionName(keys[i].substr(7));
      String type(ResourceMap::GetType(keys[i]));
      if (type == "str")
        oss << optionName << "=" << ResourceMap::GetAsString(keys[i]) << "\n";
      else if (type == "float")
        oss << optionName << "=" << ResourceMap::GetAsScalar(keys[i]) << "\n";
      else if (type == "int")
        oss << optionName << "=" << ResourceMap::GetAsUnsignedInteger(keys[i]) << "\n";
      else if (type == "bool")
        oss << optionName << "=" << ResourceMap::GetAsBool(keys[i]) << "\n";
      else
        throw InvalidArgumentException(HERE) << "Unsupported type " << type << " for Bonmin option " << optionName;
    }

  return oss;
}

END_NAMESPACE_OPENTURNS
