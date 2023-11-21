//                                               -*- C++ -*-
/**
 *  @brief Ipopt optimization solver.
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

/* Check whether this problem can be solved by this solver. */
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
        throw InvalidArgumentException(HERE) << "Invalid Ipopt option " << optionName;
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
  app->Options()->SetStringValue("honor_original_bounds", "yes");// disabled in ipopt 3.14
  GetOptionsFromResourceMap(app->Options());
  String optlist;
  app->Options()->PrintList(optlist);
  LOGDEBUG(optlist);

  // Initialize the IpoptApplication and process the options
  ApplicationReturnStatus status = app->Initialize();
  if (status != Solve_Succeeded)
  {
    throw InternalException(HERE) << "ipopt failed with code " << status;
  }

  // Ask Ipopt to solve the problem
  status = app->OptimizeTNLP(ipoptProblem);

  const Sample inputHistory(ipoptProblem->getInputHistory());
  setResultFromEvaluationHistory(inputHistory, ipoptProblem->getOutputHistory(),
                                 getProblem().hasInequalityConstraint() ? getProblem().getInequalityConstraint()(inputHistory) : Sample(),
                                 getProblem().hasEqualityConstraint() ? getProblem().getEqualityConstraint()(inputHistory) : Sample());

  String statusMessage;
  switch (status)
  {
    // info/warning (>0)
    case Solved_To_Acceptable_Level:
      statusMessage = "Solved to acceptable level";
      break;
    case Infeasible_Problem_Detected:
      statusMessage = "Infeasible problem detected";
      break;
    case Search_Direction_Becomes_Too_Small:
      statusMessage = "Search direction becomes too small";
      break;
    case Diverging_Iterates:
      statusMessage = "Diverging iterates";
      break;
    case User_Requested_Stop:
      statusMessage = "User requested stop";
      break;
    case Feasible_Point_Found:
      statusMessage = "Feasible point found";
      break;
    // errors/exception (<0)
    case Maximum_Iterations_Exceeded:
      statusMessage = "Maximum iterations exceeded";
      break;
    case Restoration_Failed:
      statusMessage = "Restoration failed";
      break;
    case Error_In_Step_Computation:
      statusMessage = "Error in step computation";
      break;
    case Maximum_CpuTime_Exceeded:
      statusMessage = "Maximum CPU time exceeded";
      break;
    case Not_Enough_Degrees_Of_Freedom:
      statusMessage = "Not enough degrees of freedom";
      break;
    case Invalid_Problem_Definition:
      statusMessage = "Invalid problem definition";
      break;
    case Invalid_Option:
      statusMessage = "Invalid option";
      break;
    case Invalid_Number_Detected:
      statusMessage = "Invalid number detected";
      break;
    case Unrecoverable_Exception:
      statusMessage = "Unrecoverable exception";
      break;
    case NonIpopt_Exception_Thrown:
      statusMessage = "NonIpopt exception thrown";
      break;
    case Insufficient_Memory:
      statusMessage = "Insufficient memory";
      break;
    case Internal_Error:
      statusMessage = "Internal Error";
      break;
    default:
      statusMessage = (OSS() << status);
      break;
  }
  result_.setStatusMessage(statusMessage);

  if (status > 0)
  {
    LOGINFO(OSS() << "Ipopt exited with status: " << statusMessage);
  }
  else if (status < 0)
  {
    result_.setStatus(OptimizationResult::FAILURE);
    throw InternalException(HERE) << "Ipopt error: " << statusMessage;
  }

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
      if (type == "str")
        oss << optionName << "=" << ResourceMap::GetAsString(keys[i]) << "\n";
      else if (type == "float")
        oss << optionName << "=" << ResourceMap::GetAsScalar(keys[i]) << "\n";
      else if (type == "int")
        oss << optionName << "=" << ResourceMap::GetAsUnsignedInteger(keys[i]) << "\n";
      else if (type == "bool")
        oss << optionName << "=" << ResourceMap::GetAsBool(keys[i]) << "\n";
      else
        throw InvalidArgumentException(HERE) << "Unsupported type " << type << " for Ipopt option " << optionName;
    }

  return oss;
}

END_NAMESPACE_OPENTURNS
