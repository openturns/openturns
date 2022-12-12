//                                               -*- C++ -*-
/**
 *  @brief Bonmin allows one to describe a MINLP optimization algorithm
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
using namespace Bonmin;
using namespace Ipopt;
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
  const Description algoNames = {"B-BB", "B-OA", "B-QG", "B-Hyb"};
  // iFP/Ecp are disabled, see https://github.com/coin-or/Bonmin/issues/31
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
static void GetOptionsFromResourceMap(SmartPtr<OptionsList> options)
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
  Ipopt::SmartPtr<BonminProblem> tminlp = new BonminProblem(getProblem(), getStartingPoint(), getMaximumEvaluationNumber());
  tminlp->setProgressCallback(progressCallback_.first, progressCallback_.second);
  tminlp->setStopCallback(stopCallback_.first, stopCallback_.second);

  // Create setup, initialize options
  BonminSetup app;
  app.initializeOptionsAndJournalist();
  app.options()->SetStringValue("bonmin.algorithm", algoName_);
  app.options()->SetIntegerValue("max_iter", getMaximumIterationNumber());
  app.options()->SetStringValue("sb", "yes"); // skip ipopt banner
  app.options()->SetIntegerValue("print_level", 0);
  app.options()->SetStringValue("honor_original_bounds", "yes");// disabled in ipopt 3.14
  app.options()->SetIntegerValue("bonmin.bb_log_level", 0);
  app.options()->SetIntegerValue("bonmin.nlp_log_level", 0);
  app.options()->SetIntegerValue("bonmin.lp_log_level", 0);
  app.options()->SetIntegerValue("bonmin.oa_log_level", 0);
  app.options()->SetIntegerValue("bonmin.fp_log_level", 0);
  app.options()->SetIntegerValue("bonmin.milp_log_level", 0);
  GetOptionsFromResourceMap(app.options());
  String optlist;
  app.options()->PrintList(optlist);
  LOGDEBUG(optlist);

  // Update setup with BonminProblem
  app.initialize(GetRawPtr(tminlp));

  // Solve problem
  Bab solver;
  solver(app);

  // Retrieve input/output history
  Sample inputHistory(tminlp->getInputHistory());
  Sample outputHistory(tminlp->getOutputHistory());

  // Create OptimizationResult, initialize error values
  OptimizationResult optimResult(getProblem());
  Scalar absoluteError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;
  Scalar constraintError = -1.0;

  /* Populate OptimizationResult from history */

  for (UnsignedInteger i = 0; i < inputHistory.getSize(); ++ i)
  {
    const Point inP(inputHistory[i]);
    const Point outP(outputHistory[i]);
    constraintError = 0.0;

    // Constraint error on bounds
    if (getProblem().hasBounds())
    {
      Interval bounds(getProblem().getBounds());
      for (UnsignedInteger j = 0; j < getProblem().getDimension(); ++ j)
      {
        if (bounds.getFiniteLowerBound()[j])
          constraintError = std::max(constraintError, bounds.getLowerBound()[j] - inP[j]);
        if (bounds.getFiniteUpperBound()[j])
          constraintError = std::max(constraintError, inP[j] - bounds.getUpperBound()[j]);
      }
    }

    // Constraint error on equality constraints
    if (getProblem().hasEqualityConstraint())
    {
      const Point g(getProblem().getEqualityConstraint()(inP));
      constraintError = std::max(constraintError, g.normInf());
    }

    // Constraint error on inequality constraints
    if (getProblem().hasInequalityConstraint())
    {
      Point h(getProblem().getInequalityConstraint()(inP));
      for (UnsignedInteger k = 0; k < getProblem().getInequalityConstraint().getOutputDimension(); ++ k)
      {
        h[k] = std::min(h[k], 0.0); // convention h(x)>=0 <=> admissibility
      }
      constraintError = std::max(constraintError, h.normInf());
    }

    // Computing errors, storing into OptimizationResult
    if (i > 0)
    {
      const Point inPM(inputHistory[i - 1]);
      const Point outPM(outputHistory[i - 1]);
      absoluteError = (inP - inPM).normInf();
      relativeError = (inP.normInf() > 0.0) ? (absoluteError / inP.normInf()) : -1.0;
      residualError = (std::abs(outP[0]) > 0.0) ? (std::abs(outP[0] - outPM[0]) / std::abs(outP[0])) : -1.0;
    }

    optimResult.store(inP,
                      outP,
                      absoluteError,
                      relativeError,
                      residualError,
                      constraintError);
  }

  // Optimum is not the last call to objective function
  optimResult.setOptimalPoint(tminlp->getOptimalPoint());
  optimResult.setOptimalValue(tminlp->getOptimalValue());

  setResult(optimResult);

  String allOptions;
  app.options()->PrintList(allOptions);
  LOGINFO(allOptions);

#else

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
