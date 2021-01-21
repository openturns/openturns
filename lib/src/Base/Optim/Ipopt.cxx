//                                               -*- C++ -*-
/**
 *  @brief Ipopt optimization solver.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Ipopt.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/OSS.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#ifdef OPENTURNS_HAVE_IPOPT
#include "openturns/IpoptProblem.hxx"
#include <IpIpoptApplication.hpp>
using namespace Ipopt;
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Ipopt)

static const Factory<Ipopt> Factory_Ipopt;

/** Constructors */
Ipopt::Ipopt()
  : OptimizationAlgorithmImplementation()
{
  // Nothing to do
}

Ipopt::Ipopt( OptimizationProblem & problem)
  : OptimizationAlgorithmImplementation(problem)
{
  // Nothing to do
}

/* Virtual constructor */
Ipopt * Ipopt::clone() const
{
  return new Ipopt(*this);
}



/** Ipopt static methods */
Bool Ipopt::IsAvailable()
{
#ifdef OPENTURNS_HAVE_IPOPT
  return true;
#else
  return false;
#endif
}

/** Check whether this problem can be solved by this solver. */
void Ipopt::checkProblem(const OptimizationProblem & problem) const
{
  // Cannot solve multi-objective problems
  if (problem.hasMultipleObjective()) throw InvalidArgumentException(HERE) << "Ipopt does not support multi-objective optimization";

  // No LeastSquaresProblem / NearestPointProblem
  if (problem.hasResidualFunction() || problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Ipopt does not support least squares or nearest point problems";

  // Cannot solve non continuous problems
  if (!problem.isContinuous())
    throw InvalidArgumentException(HERE) << "Ipopt does not support non continuous problems";
}


#ifdef OPENTURNS_HAVE_IPOPT

/** Accessors to Ipopt options */
static void GetOptionsFromResourceMap(SmartPtr<OptionsList> options)
{
//   Get options for Ipopt setup from ResourceMap
//   See Ipopt/Ipopt user manuals for more details.

  std::vector<String> keys(ResourceMap::GetKeys());
  const UnsignedInteger nbKeys = keys.size();

  for (UnsignedInteger i = 0; i < nbKeys; ++ i)
    if (keys[i].substr(0, 6) == "Ipopt-")
    {
      String optionName(keys[i].substr(6));
      String type(ResourceMap::GetType(keys[i]));
      if (type == "string")
        options->SetStringValue(optionName, ResourceMap::GetAsString(keys[i]));
      else if (type == "float")
        options->SetNumericValue(optionName, ResourceMap::GetAsScalar(keys[i]));
      else if (type == "unsigned int")
        options->SetIntegerValue(optionName, ResourceMap::GetAsUnsignedInteger(keys[i]));
      else
        throw InvalidArgumentException(HERE) << "Unsupported type " << type << " for Ipopt option " << optionName;
    }
}

#endif

/** Performs the actual computation. */
void Ipopt::run()
{
#ifdef OPENTURNS_HAVE_IPOPT
  // Check problem
  checkProblem(getProblem());

  // Check starting point
  if (getStartingPoint().getDimension() != getProblem().getDimension())
    throw InvalidArgumentException(HERE) << "Invalid starting point dimension (" << getStartingPoint().getDimension() << "), expected " << getProblem().getDimension();

  // Create BonminProblem
  ::Ipopt::SmartPtr<IpoptProblem> ipoptProblem = new IpoptProblem(getProblem(), getStartingPoint(), getMaximumEvaluationNumber());
  ipoptProblem->setProgressCallback(progressCallback_.first, progressCallback_.second);
  ipoptProblem->setStopCallback(stopCallback_.first, stopCallback_.second);

  SmartPtr<IpoptApplication> app = IpoptApplicationFactory();
  app->Options()->SetIntegerValue("print_level", 0);
  app->Options()->SetIntegerValue("max_iter", getMaximumIterationNumber());
  app->Options()->SetStringValue("sb", "yes"); // skip banner
  GetOptionsFromResourceMap(app->Options());

  // Intialize the IpoptApplication and process the options
  ApplicationReturnStatus status = app->Initialize();
  if (status != Solve_Succeeded)
  {
    throw InternalException(HERE) << "ipopt failed with code " << status;
  }

  // Ask Ipopt to solve the problem
  status = app->OptimizeTNLP(ipoptProblem);
  String statusString;
  switch (status)
  {
    // info/warning (>0)
    case Solved_To_Acceptable_Level:
      statusString = "Solved to acceptable level";
      break;
    case Infeasible_Problem_Detected:
      statusString = "Infeasible problem detected";
      break;
    case Search_Direction_Becomes_Too_Small:
      statusString = "Search direction becomes too small";
      break;
    case Diverging_Iterates:
      statusString = "Diverging iterates";
      break;
    case User_Requested_Stop:
      statusString = "User requested stop";
      break;
    case Feasible_Point_Found:
      statusString = "Feasible point found";
      break;
    // errors/exception (<0)
    case Maximum_Iterations_Exceeded:
      statusString = "Maximum iterations exceeded";
      break;
    case Restoration_Failed:
      statusString = "Restoration failed";
      break;
    case Error_In_Step_Computation:
      statusString = "Error in step computation";
      break;
    case Maximum_CpuTime_Exceeded:
      statusString = "Maximum CPU time exceeded";
      break;
    case Not_Enough_Degrees_Of_Freedom:
      statusString = "Not enough degrees of freedom";
      break;
    case Invalid_Problem_Definition:
      statusString = "Invalid problem definition";
      break;
    case Invalid_Option:
      statusString = "Invalid option";
      break;
    case Invalid_Number_Detected:
      statusString = "Invalid number detected";
      break;
    case Unrecoverable_Exception:
      statusString = "Unrecoverable exception";
      break;
    case NonIpopt_Exception_Thrown:
      statusString = "NonIpopt exception thrown";
      break;
    case Insufficient_Memory:
      statusString = "Insufficient memory";
      break;
    case Internal_Error:
      statusString = "Internal Error";
      break;
    default:
      statusString = (OSS() << status);
      break;
  }
  if (status > 0)
  {
    LOGINFO(OSS() << "Ipopt exited with status: " << statusString);
  }
  else if (status < 0)
  {
    throw InternalException(HERE) << "Ipopt error: " << statusString;
  }

  // Retrieve MemoizeFunction input/output history
  Sample inputHistory(ipoptProblem->getInputHistory());
  Sample outputHistory(ipoptProblem->getOutputHistory());

  // Create OptimizationResult, initialize error values
  OptimizationResult optimResult(getProblem());
  Scalar absoluteError = -1.0;
  Scalar relativeError = -1.0;
  Scalar residualError = -1.0;
  Scalar constraintError = -1.0;

  /* Populate OptimizationResult from memoize history */

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
  optimResult.setOptimalPoint(ipoptProblem->getOptimalPoint());
  optimResult.setOptimalValue(ipoptProblem->getOptimalValue());

  setResult(optimResult);

#else
  throw NotYetImplementedException(HERE) << "No Ipopt support";
#endif
}


/** Description of object */
String Ipopt::__str__(const String &) const
{
  OSS oss(false);
  oss << "class=" << getClassName();
  return oss;
}

String Ipopt::__repr__() const
{
  OSS oss(false);
  oss << __str__();
  oss << "\noptions=\n";

  // List user-defined options
  std::vector<String> keys(ResourceMap::GetKeys());
  UnsignedInteger nbKeys = keys.size();

  for (UnsignedInteger i = 0; i < nbKeys; ++i)
    if (keys[i].substr(0, 6) == "Ipopt-")
    {
      String optionName(keys[i].substr(6));
      String type(ResourceMap::GetType(keys[i]));
      if (type == "string")
        oss << optionName << "=" << ResourceMap::GetAsString(keys[i]) << "\n";
      else if (type == "float")
        oss << optionName << "=" << ResourceMap::GetAsScalar(keys[i]) << "\n";
      else if (type == "unsigned int")
        oss << optionName << "=" << ResourceMap::GetAsUnsignedInteger(keys[i]) << "\n";
      else
        throw InvalidArgumentException(HERE) << "Unsupported type " << type << " for Ipopt option " << optionName;
    }

  return oss;
}

END_NAMESPACE_OPENTURNS
