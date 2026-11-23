//                                               -*- C++ -*-
/**
 *  @brief SCIPProblem implements the interface between OT and SCIP for
 * optimization problems.
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
#include "openturns/SCIPProblem.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"

#include <algorithm>
#include <cmath>
#include <limits>

BEGIN_NAMESPACE_OPENTURNS

namespace
{

// Throw an OpenTURNS exception when a SCIP function does not return SCIP_OKAY.
#define OT_SCIP_CALL(x) \
  do { \
    const SCIP_RETCODE otSCIPRetcode_ = (x); \
    if (otSCIPRetcode_ != SCIP_OKAY) \
      throw InternalException(HERE) << "SCIP error " << static_cast<int>(otSCIPRetcode_) << " in " #x; \
  } while (false)

// Conversion from the OpenTURNS variable type to the SCIP one.
SCIP_VARTYPE toSCIPType(const OptimizationProblemImplementation::VariableType type)
{
  switch (type)
  {
    case OptimizationProblemImplementation::BINARY:
      return SCIP_VARTYPE_BINARY;
    case OptimizationProblemImplementation::INTEGER:
      return SCIP_VARTYPE_INTEGER;
    default:
      return SCIP_VARTYPE_CONTINUOUS;
  }
}

} /* namespace */

/* Constructor with parameters */
SCIPProblem::SCIPProblem(const OptimizationProblem & optimProblem,
                         const Point & startingPoint,
                         const UnsignedInteger maximumCallsNumber,
                         const Scalar maximumTimeDuration,
                         const UnsignedInteger maximumIterationNumber,
                         const Scalar maximumConstraintError,
                         const std::chrono::steady_clock::time_point & t0)
  : core_(optimProblem, startingPoint, maximumCallsNumber, maximumTimeDuration, t0)
  , startingPoint_(startingPoint)
  , maximumTimeDuration_(maximumTimeDuration)
  , maximumIterationNumber_(maximumIterationNumber)
  , maximumConstraintError_(maximumConstraintError)
  , t0_(t0)
{
  // Nothing to do
}

/* Destructor */
SCIPProblem::~SCIPProblem()
{
  release();
}

/* Solve the problem */
void SCIPProblem::run()
{
  try
  {
    createSCIP();
    setParameters();
    includeExpressionHandler();
    createVariables();
    if (core_.isLinear())
    {
      createLinearConstraints();
      // The linear objective coefficients have been set with the variables,
      // only the constant part remains.
      Point zero(core_.getDimension());
      const Scalar offset = core_.getProblem().getObjective()(zero)[0];
      if (offset != 0.0)
        // SCIPaddObjoffset is reserved to the presolving stage, use the
        // original problem offset from the problem construction stage
        OT_SCIP_CALL(SCIPaddOrigObjoffset(scip_, offset));
    }
    else
    {
      if (core_.hasEqualityConstraint())
      {
        const Function equalityConstraint(core_.getEqualityConstraint());
        for (UnsignedInteger i = 0; i < core_.getEqualityDimension(); ++ i)
          createExpressionConstraint(equalityConstraint.getMarginal(i), i,
                                     FunctionKind::EQUALITY, "ot_eq_" + std::to_string(i), 0.0, 0.0);
      }
      if (core_.hasInequalityConstraint())
      {
        const Function inequalityConstraint(core_.getInequalityConstraint());
        const UnsignedInteger shift(core_.getEqualityDimension());
        const Scalar infinity(SCIPinfinity(scip_));
        for (UnsignedInteger i = 0; i < core_.getInequalityDimension(); ++ i)
          createExpressionConstraint(inequalityConstraint.getMarginal(i), shift + i,
                                     FunctionKind::INEQUALITY, "ot_ineq_" + std::to_string(i), 0.0, infinity);
      }
      createExpressionObjective();
    }
    injectStartingPoint();
    // The initial evaluation may already exhaust a cooperative limit. Do not
    // start the solve in that case: a linear solve does not call back into the
    // evaluation functions, so the stop callback and the budget would
    // otherwise be ignored.
    if (core_.isStopped() || core_.isTimeOut() || core_.isCallLimitReached())
    {
      if (core_.isStopped())
      {
        status_ = OptimizationResult::INTERRUPTION;
        statusMessage_ = "user interrupt";
      }
      else if (core_.isTimeOut())
      {
        status_ = OptimizationResult::TIMEOUT;
        statusMessage_ = "time limit";
      }
      else
      {
        status_ = OptimizationResult::MAXIMUMCALLS;
        statusMessage_ = "call limit";
      }
      return;
    }
    solve();
  }
  catch (const InvalidArgumentException &)
  {
    release();
    throw;
  }
  catch (const std::exception & exc)
  {
    solveFailed_ = true;
    if (evaluationErrorMessage_.empty())
      evaluationErrorMessage_ = exc.what();
    status_ = OptimizationResult::FAILURE;
    statusMessage_ = exc.what();
  }
  if (solved_)
    extractSolution();
  release();
}

/* SCIP object creation */
void SCIPProblem::createSCIP()
{
  OT_SCIP_CALL(SCIPcreate(&scip_));
  // Keep the plugin initialization quiet, the final verbosity is set in
  // setParameters() once the SCIP- keys have been forwarded
  OT_SCIP_CALL(SCIPsetIntParam(scip_, "display/verblevel", 0));
  OT_SCIP_CALL(SCIPincludeDefaultPlugins(scip_));
  // Poll the stop callback and the time budget at each node, so that a linear
  // solve, which never calls the evaluation callbacks, can be interrupted too
  OT_SCIP_CALL(SCIPincludeEventhdlrBasic(scip_, &stopEventhdlr_, "otstop",
                                         "OpenTURNS stop callback", stopEventExec,
                                         reinterpret_cast<SCIP_EVENTHDLRDATA *>(this)));
  OT_SCIP_CALL(SCIPsetEventhdlrInit(scip_, stopEventhdlr_, stopEventInit));
  OT_SCIP_CALL(SCIPcreateProbBasic(scip_, "OpenTURNS"));
  OT_SCIP_CALL(SCIPsetObjsense(scip_, core_.getProblem().isMinimization() ? SCIP_OBJSENSE_MINIMIZE : SCIP_OBJSENSE_MAXIMIZE));
}

/* Forward the OpenTURNS limits and the SCIP- ResourceMap keys */
void SCIPProblem::setParameters()
{
  if (maximumTimeDuration_ > 0.0)
    OT_SCIP_CALL(SCIPsetRealParam(scip_, "limits/time", maximumTimeDuration_));
  if (maximumIterationNumber_ > 0)
    // limits/nodes is a longint parameter
    OT_SCIP_CALL(SCIPsetLongintParam(scip_, "limits/nodes",
                                     static_cast<SCIP_Longint>(std::min<UnsignedInteger>(maximumIterationNumber_, std::numeric_limits<SCIP_Longint>::max()))));
  if (maximumConstraintError_ > 0.0)
    OT_SCIP_CALL(SCIPsetRealParam(scip_, "numerics/feastol", maximumConstraintError_));

  // A black-box expression is opaque to symmetry detection: SCIP cannot know
  // whether permuting its children leaves the expression unchanged, so the
  // symmetries it would infer may be invalid. Disable symmetry handling for
  // nonlinear problems unless the user configured it explicitly.
  if (!core_.isLinear() && !ResourceMap::HasKey("SCIP-misc/usesymmetry"))
    OT_SCIP_CALL(SCIPsetIntParam(scip_, "misc/usesymmetry", 0));

  const std::vector<String> keys(ResourceMap::GetKeys());
  Bool userVerbosity = false;
  for (const String & key : keys)
    if (key.substr(0, 5) == "SCIP-")
    {
      // These keys set the default OpenTURNS budgets, they are not SCIP parameters
      if ((key == "SCIP-DefaultMaximumCallsNumber") || (key == "SCIP-DefaultMaximumIterationNumber"))
        continue;
      const String name(key.substr(5));
      if (name.substr(0, 8) == "display/")
        userVerbosity = true;
      const String type(ResourceMap::GetType(key));
      // Let SCIP parse the textual value according to the actual parameter
      // type (int, longint, real, bool, char or string).
      String value;
      if (type == "str")
        value = ResourceMap::GetAsString(key);
      else if (type == "float")
        value = OSS() << ResourceMap::GetAsScalar(key);
      else if (type == "int")
        value = OSS() << ResourceMap::GetAsUnsignedInteger(key);
      else if (type == "bool")
        value = ResourceMap::GetAsBool(key) ? "TRUE" : "FALSE";
      else
        throw InvalidArgumentException(HERE) << "Unsupported type " << type << " for SCIP option " << name;
      SCIP_PARAM * parameter = SCIPgetParam(scip_, name.c_str());
      if (parameter == nullptr)
        throw InvalidArgumentException(HERE) << "Invalid SCIP parameter " << name << " (value=" << value << ")";
      // SCIP expects the double quotes around string values, but the ResourceMap
      // only has unsigned integers: a numeric string may target an integer
      // parameter (e.g. SCIP-heuristics/mpec/freq = "-1"), so only quote the
      // values that actually target a string parameter.
      if ((type == "str") && (SCIPparamGetType(parameter) == SCIP_PARAMTYPE_STRING))
        value = "\"" + value + "\"";
      if (SCIPsetParam(scip_, name.c_str(), value.c_str()) != SCIP_OKAY)
        throw InvalidArgumentException(HERE) << "Invalid SCIP parameter " << name << " (value=" << value << ")";
    }

  // Keep the solver quiet unless debug logging is enabled or the user asked
  // for a specific display option
  SCIPsetMessagehdlrQuiet(scip_, !Log::HasDebug() && !userVerbosity);
}

/* Expression handler registration */
void SCIPProblem::includeExpressionHandler()
{
  OT_SCIP_CALL(SCIPincludeExprhdlr(scip_, &exprhdlr_, "openturns", "OpenTURNS black-box function", 0, evalExpr, nullptr));
  SCIPexprhdlrSetDiff(exprhdlr_, bwdiffExpr, fwdiffExpr, bwfwdiffExpr);
  SCIPexprhdlrSetCopyFreeData(exprhdlr_, copyData, freeData);
}

/* Variables creation */
void SCIPProblem::createVariables()
{
  const OptimizationProblem & problem = core_.getProblem();
  const UnsignedInteger dimension = core_.getDimension();
  if (dimension == 0)
    throw InvalidArgumentException(HERE) << "No problem has been set.";

  const Scalar infinity = SCIPinfinity(scip_);
  const Point lowerBounds(problem.hasBounds() ? problem.getBounds().getLowerBound() : Point(dimension));
  const Point upperBounds(problem.hasBounds() ? problem.getBounds().getUpperBound() : Point(dimension));
  const Interval::BoolCollection finiteLowerBounds(problem.hasBounds() ? problem.getBounds().getFiniteLowerBound() : Interval::BoolCollection(dimension));
  const Interval::BoolCollection finiteUpperBounds(problem.hasBounds() ? problem.getBounds().getFiniteUpperBound() : Interval::BoolCollection(dimension));

  // Linear objective coefficients are set at variable creation
  const Point cost(core_.isLinear() ? core_.getLinearCost() : Point(dimension));

  const Indices variablesType(problem.getVariablesType());
  variables_.resize(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    Scalar lower = -infinity;
    Scalar upper = infinity;
    if (finiteLowerBounds[i])
      lower = lowerBounds[i];
    if (finiteUpperBounds[i])
      upper = upperBounds[i];

    switch (static_cast<OptimizationProblemImplementation::VariableType>(variablesType[i]))
    {
      case OptimizationProblemImplementation::BINARY:
        // A binary variable is always within [0, 1]
        lower = std::max(lower, 0.0);
        upper = std::min(upper, 1.0);
        break;
      case OptimizationProblemImplementation::INTEGER:
        // Keep the integer points of the interval
        if (finiteLowerBounds[i])
          lower = std::ceil(lower);
        if (finiteUpperBounds[i])
          upper = std::floor(upper);
        break;
      default:
        break;
    }
    if (lower > upper)
      throw InvalidArgumentException(HERE) << "Invalid bounds for variable " << i << ": lower bound "
                                           << lower << " is greater than upper bound " << upper;

    const Scalar objectiveCoefficient = core_.isLinear() ? cost[i] : 0.0;
    const String name("x" + std::to_string(i));
    OT_SCIP_CALL(SCIPcreateVarBasic(scip_, &variables_[i], name.c_str(), lower, upper,
                                    objectiveCoefficient, toSCIPType(static_cast<OptimizationProblemImplementation::VariableType>(variablesType[i]))));
    OT_SCIP_CALL(SCIPaddVar(scip_, variables_[i]));
  }

  // One variable expression per OpenTURNS variable, shared by all the
  // black-box constraints and by the objective.
  variableExpressions_.resize(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    OT_SCIP_CALL(SCIPcreateExprVar(scip_, &variableExpressions_[i], variables_[i], nullptr, nullptr));
}

/* Matrix fast path for a structured linear problem */
void SCIPProblem::createLinearConstraints()
{
  const Matrix coefficients(core_.getLinearConstraintCoefficients());
  const Interval bounds(core_.getLinearConstraintBounds());
  const UnsignedInteger nbRows(coefficients.getNbRows());
  if (nbRows == 0)
    return;

  const UnsignedInteger dimension(core_.getDimension());
  const Scalar infinity(SCIPinfinity(scip_));
  const Interval::BoolCollection finiteLowerBounds(bounds.getFiniteLowerBound());
  const Interval::BoolCollection finiteUpperBounds(bounds.getFiniteUpperBound());

  // SCIPcreateConsBasicLinear takes a writable SCIP_VAR ** array and
  // Collection only exposes a const data(), hence std::vector here.
  std::vector<SCIP_VAR *> rowVariables;
  std::vector<SCIP_Real> rowCoefficients;
  rowVariables.reserve(dimension);
  rowCoefficients.reserve(dimension);
  for (UnsignedInteger row = 0; row < nbRows; ++ row)
  {
    const Scalar lhs(finiteLowerBounds[row] ? bounds.getLowerBound()[row] : -infinity);
    const Scalar rhs(finiteUpperBounds[row] ? bounds.getUpperBound()[row] : infinity);
    if ((lhs == -infinity) && (rhs == infinity))
      continue;

    rowVariables.clear();
    rowCoefficients.clear();
    for (UnsignedInteger column = 0; column < dimension; ++ column)
    {
      const Scalar coefficient(coefficients(row, column));
      if (coefficient != 0.0)
      {
        rowVariables.push_back(variables_[column]);
        rowCoefficients.push_back(coefficient);
      }
    }
    const String name("ot_linear_" + std::to_string(row));
    SCIP_CONS * cons = nullptr;
    OT_SCIP_CALL(SCIPcreateConsBasicLinear(scip_, &cons, name.c_str(),
                                           static_cast<int>(rowVariables.size()),
                                           rowVariables.data(), rowCoefficients.data(), lhs, rhs));
    OT_SCIP_CALL(SCIPaddCons(scip_, cons));
    OT_SCIP_CALL(SCIPreleaseCons(scip_, &cons));
  }
}

/* One black-box expression constraint */
void SCIPProblem::createExpressionConstraint(const Function & constraint,
                                             const UnsignedInteger marginalIndex,
                                             const FunctionKind kind,
                                             const String & name,
                                             const Scalar lhs,
                                             const Scalar rhs)
{
  FunctionData * data = new FunctionData;
  data->function = constraint;
  data->owner = this;
  data->kind = kind;
  data->marginalIndex = marginalIndex;

  const int dimension = static_cast<int>(variableExpressions_.size());
  SCIP_EXPR * expr = nullptr;
  const SCIP_RETCODE createExprRetcode = SCIPcreateExpr(scip_, &expr, exprhdlr_,
                                                        reinterpret_cast<SCIP_EXPRDATA *>(data),
                                                        dimension, variableExpressions_.data(), nullptr, nullptr);
  if (createExprRetcode != SCIP_OKAY)
  {
    delete data;
    OT_SCIP_CALL(createExprRetcode);
  }

  SCIP_CONS * cons = nullptr;
  const SCIP_RETCODE createConsRetcode = SCIPcreateConsBasicNonlinear(scip_, &cons, name.c_str(), expr, lhs, rhs);
  // SCIPcreateConsBasicNonlinear captures expr when it succeeds, so our
  // reference is released in both cases.
  OT_SCIP_CALL(SCIPreleaseExpr(scip_, &expr));
  OT_SCIP_CALL(createConsRetcode);
  OT_SCIP_CALL(SCIPaddCons(scip_, cons));
  OT_SCIP_CALL(SCIPreleaseCons(scip_, &cons));
}

/* Nonlinear objective: minimize the auxiliary variable z subject to z = f(x) */
void SCIPProblem::createExpressionObjective()
{
  const Scalar infinity = SCIPinfinity(scip_);
  OT_SCIP_CALL(SCIPcreateVarBasic(scip_, &objectiveVariable_, "objective", -infinity, infinity,
                                  1.0, SCIP_VARTYPE_CONTINUOUS));
  OT_SCIP_CALL(SCIPaddVar(scip_, objectiveVariable_));

  SCIP_EXPR * objectiveExpr = nullptr;
  OT_SCIP_CALL(SCIPcreateExprVar(scip_, &objectiveExpr, objectiveVariable_, nullptr, nullptr));

  // SCIPcreateExpr takes a writable SCIP_EXPR ** array and Collection only
  // exposes a const data(), hence std::vector here.
  std::vector<SCIP_EXPR *> children;
  children.reserve(variableExpressions_.size() + 1);
  children.push_back(objectiveExpr);
  children.insert(children.end(), variableExpressions_.begin(), variableExpressions_.end());

  FunctionData * data = new FunctionData;
  data->function = core_.getProblem().getObjective();
  data->owner = this;
  data->kind = FunctionKind::OBJECTIVE;
  data->marginalIndex = 0;

  SCIP_EXPR * expr = nullptr;
  const SCIP_RETCODE createExprRetcode = SCIPcreateExpr(scip_, &expr, exprhdlr_,
                                                        reinterpret_cast<SCIP_EXPRDATA *>(data),
                                                        static_cast<int>(children.size()), children.data(), nullptr, nullptr);
  // objectiveExpr has been captured by expr when the creation succeeds
  OT_SCIP_CALL(SCIPreleaseExpr(scip_, &objectiveExpr));
  if (createExprRetcode != SCIP_OKAY)
  {
    delete data;
    OT_SCIP_CALL(createExprRetcode);
  }

  SCIP_CONS * cons = nullptr;
  const SCIP_RETCODE createConsRetcode = SCIPcreateConsBasicNonlinear(scip_, &cons, "ot_objective",
                                                                      expr, 0.0, 0.0);
  OT_SCIP_CALL(SCIPreleaseExpr(scip_, &expr));
  OT_SCIP_CALL(createConsRetcode);
  OT_SCIP_CALL(SCIPaddCons(scip_, cons));
  OT_SCIP_CALL(SCIPreleaseCons(scip_, &cons));
}

/* Inject the starting point as an initial candidate */
void SCIPProblem::injectStartingPoint()
{
  if (startingPoint_.getDimension() == 0)
    return;

  // Always evaluate the starting point through the shared core so that it
  // appears in the evaluation history, even if SCIP rejects it.
  Scalar objectiveValue = 0.0;
  const Bool evaluated = core_.evaluateObjective(startingPoint_, objectiveValue);

  // SCIPtrySolFree is only available once the problem is transformed. This is
  // idempotent and SCIPsolve() resumes from the transformed stage.
  if (SCIPgetStage(scip_) < SCIP_STAGE_TRANSFORMED)
    OT_SCIP_CALL(SCIPtransformProb(scip_));

  SCIP_SOL * sol = nullptr;
  OT_SCIP_CALL(SCIPcreateSol(scip_, &sol, nullptr));
  for (UnsignedInteger i = 0; i < core_.getDimension(); ++ i)
    OT_SCIP_CALL(SCIPsetSolVal(scip_, sol, SCIPvarGetTransVar(variables_[i]), startingPoint_[i]));
  // The auxiliary variable must always be set: a solution with unset values
  // is partial and SCIP refuses to check it. When the objective could not be
  // evaluated, the artificial value makes the point infeasible and SCIP
  // silently rejects it.
  if (objectiveVariable_ != nullptr)
    OT_SCIP_CALL(SCIPsetSolVal(scip_, sol, SCIPvarGetTransVar(objectiveVariable_), evaluated ? objectiveValue : 0.0));

  SCIP_Bool stored = FALSE;
  // An infeasible or out of bounds starting point is silently rejected.
  OT_SCIP_CALL(SCIPtrySolFree(scip_, &sol, FALSE, FALSE, TRUE, TRUE, FALSE, &stored));
  if (!stored)
    LOGINFO(OSS() << "SCIP rejected the starting point");
}

/* Run SCIP */
void SCIPProblem::solve()
{
  const SCIP_RETCODE retcode = SCIPsolve(scip_);
  solved_ = true;
  if (retcode != SCIP_OKAY)
  {
    solveFailed_ = true;
    if (evaluationErrorMessage_.empty())
      evaluationErrorMessage_ = OSS() << "SCIP solve failed (retcode=" << static_cast<int>(retcode) << ")";
  }
}

/* Extract the solution and the status */
void SCIPProblem::extractSolution()
{
  const SCIP_STATUS scipStatus = SCIPgetStatus(scip_);
  iterationNumber_ = static_cast<UnsignedInteger>(SCIPgetNNodes(scip_));

  SCIP_SOL * sol = SCIPgetBestSol(scip_);
  hasSolution_ = (sol != nullptr);

  // Compute the status before the bookkeeping evaluation below: recording the
  // incumbent in the history must not turn a successful solve into a call
  // limit when the budget was already exhausted.
  status_ = computeStatus(scipStatus);
  statusMessage_ = computeStatusMessage(scipStatus);

  if (hasSolution_)
  {
    const UnsignedInteger dimension = core_.getDimension();
    optimalPoint_ = Point(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      optimalPoint_[i] = SCIPgetSolVal(scip_, sol, variables_[i]);
    optimalValue_ = SCIPgetSolOrigObj(scip_, sol);
    // Record the optimal point in the shared history, so that the result
    // contains it even when it was not produced by our callbacks.
    Scalar value = 0.0;
    core_.evaluateObjective(optimalPoint_, value);
  }
}

/* Map the SCIP status to the OpenTURNS one */
OptimizationResult::Status SCIPProblem::computeStatus(const SCIP_STATUS scipStatus) const
{
  if (solveFailed_)
    return OptimizationResult::FAILURE;
  if (core_.isCallLimitReached() || callLimitReached_)
    return OptimizationResult::MAXIMUMCALLS;
  if (core_.isTimeOut() || timeLimitReached_ || (scipStatus == SCIP_STATUS_TIMELIMIT))
    return OptimizationResult::TIMEOUT;
  if (core_.isStopped() || (scipStatus == SCIP_STATUS_USERINTERRUPT) || (scipStatus == SCIP_STATUS_TERMINATE))
    return OptimizationResult::INTERRUPTION;

  // Without an incumbent there is nothing to report
  if (!hasSolution_)
    return OptimizationResult::FAILURE;

  switch (scipStatus)
  {
    case SCIP_STATUS_OPTIMAL:
    case SCIP_STATUS_GAPLIMIT:
      return OptimizationResult::SUCCESS;
    // The OpenTURNS status enum has no dedicated iteration/node limit: follow
    // the HiGHS precedent and report a search limit as MAXIMUMCALLS.
    case SCIP_STATUS_NODELIMIT:
    case SCIP_STATUS_TOTALNODELIMIT:
    case SCIP_STATUS_STALLNODELIMIT:
    case SCIP_STATUS_SOLLIMIT:
    case SCIP_STATUS_BESTSOLLIMIT:
    case SCIP_STATUS_PRIMALLIMIT:
      return OptimizationResult::MAXIMUMCALLS;
    default:
      return OptimizationResult::FAILURE;
  }
}

/* Human readable status */
String SCIPProblem::computeStatusMessage(const SCIP_STATUS scipStatus) const
{
  String statusName;
  // Report the cooperative limits with the OpenTURNS wording
  if (core_.isCallLimitReached() || callLimitReached_)
    statusName = "call limit";
  else if (core_.isTimeOut() || timeLimitReached_)
    statusName = "time limit";
  else
  switch (scipStatus)
  {
    case SCIP_STATUS_OPTIMAL:        statusName = "optimal"; break;
    case SCIP_STATUS_INFEASIBLE:     statusName = "infeasible"; break;
    case SCIP_STATUS_UNBOUNDED:      statusName = "unbounded"; break;
    case SCIP_STATUS_INFORUNBD:      statusName = "infeasible or unbounded"; break;
    case SCIP_STATUS_USERINTERRUPT:  statusName = "user interrupt"; break;
    case SCIP_STATUS_TERMINATE:      statusName = "terminate"; break;
    case SCIP_STATUS_NODELIMIT:      statusName = "node limit"; break;
    case SCIP_STATUS_TOTALNODELIMIT: statusName = "total node limit"; break;
    case SCIP_STATUS_STALLNODELIMIT: statusName = "stall node limit"; break;
    case SCIP_STATUS_TIMELIMIT:      statusName = "time limit"; break;
    case SCIP_STATUS_MEMLIMIT:       statusName = "memory limit"; break;
    case SCIP_STATUS_GAPLIMIT:       statusName = "gap limit"; break;
    case SCIP_STATUS_SOLLIMIT:       statusName = "solution limit"; break;
    case SCIP_STATUS_BESTSOLLIMIT:   statusName = "best solution limit"; break;
    case SCIP_STATUS_PRIMALLIMIT:    statusName = "primal limit"; break;
    case SCIP_STATUS_RESTARTLIMIT:   statusName = "restart limit"; break;
    default:                         statusName = "unknown"; break;
  }

  OSS oss;
  oss << statusName << " nodes=" << iterationNumber_;
  // For a nonlinear problem the SCIP objective is the auxiliary variable z:
  // without a relaxation of the black-box terms its bounds are not meaningful,
  // so only report them for the linear path where they are exact.
  // The bound and gap accessors are only valid from the presolving stage on
  if (core_.isLinear() && hasSolution_ && (SCIPgetStage(scip_) >= SCIP_STAGE_PRESOLVING) && (SCIPgetStage(scip_) <= SCIP_STAGE_SOLVED))
    oss << " primal=" << SCIPgetPrimalbound(scip_)
        << " dual=" << SCIPgetDualbound(scip_)
        << " gap=" << SCIPgetGap(scip_);
  if (!evaluationErrorMessage_.empty())
    oss << " error=" << evaluationErrorMessage_;
  return oss;
}

/* Stop SCIP when a cooperative limit is reached */
Bool SCIPProblem::checkLimits()
{
  Bool allowed = true;
  const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0_).count();
  if ((maximumTimeDuration_ > 0.0) && (timeDuration > maximumTimeDuration_))
  {
    timeLimitReached_ = true;
    allowed = false;
  }
  if (allowed)
    // Poll the stop callback and the cooperative budgets. checkTermination()
    // sets the time/stop flags, checkBudget() enforces the exact call budget.
    allowed = core_.checkTermination() && core_.checkBudget();
  if (!allowed)
  {
    if (core_.isTimeOut() || timeLimitReached_)
      timeLimitReached_ = true;
    else if (!core_.isStopped())
      // The call budget is the only remaining reason to stop
      callLimitReached_ = true;
  }
  if (!allowed && (scip_ != nullptr))
    SCIPinterruptSolve(scip_);
  return allowed;
}

/* Point stored in the children of an expression */
Point SCIPProblem::getEvalPoint(SCIP_EXPR * expr, const Bool withObjectiveVariable)
{
  const int numberOfChildren = SCIPexprGetNChildren(expr);
  SCIP_EXPR ** children = SCIPexprGetChildren(expr);
  const int offset = withObjectiveVariable ? 1 : 0;
  Point x(numberOfChildren - offset);
  for (int i = offset; i < numberOfChildren; ++ i)
    x[i - offset] = SCIPexprGetEvalValue(children[i]);
  return x;
}

void SCIPProblem::setEvaluationError(const String & message)
{
  evaluationErrorMessage_ = message;
}

/* Expression evaluation callback */
SCIP_RETCODE SCIPProblem::evalExpr(::SCIP * /* scip */, SCIP_EXPR * expr, SCIP_Real * value, SCIP_SOL * /* sol */)
{
  FunctionData * data = reinterpret_cast<FunctionData *>(SCIPexprGetData(expr));
  if ((data == nullptr) || (data->owner == nullptr))
    return SCIP_ERROR;
  try
  {
    const Bool objective = (data->kind == FunctionKind::OBJECTIVE);
    const Point x(getEvalPoint(expr, objective));
    Scalar result = 0.0;
    if (objective)
    {
      // evaluateObjective() also checks the budget once the value is stored in
      // the history: a negative answer due to a cooperative stop is not an
      // evaluation failure, the value is valid and checkLimits() will
      // interrupt the solve cleanly.
      if (!data->owner->core_.evaluateObjective(x, result)
          && !data->owner->core_.isCallLimitReached()
          && !data->owner->core_.isTimeOut()
          && !data->owner->core_.isStopped())
      {
        data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
        return SCIP_ERROR;
      }
      // children[0] is the auxiliary variable z
      *value = result - SCIPexprGetEvalValue(SCIPexprGetChildren(expr)[0]);
    }
    else
    {
      if (!data->owner->core_.evaluateConstraintMarginal(data->marginalIndex, x, result))
      {
        data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
        return SCIP_ERROR;
      }
      *value = result;
    }
    data->owner->checkLimits();
    return SCIP_OKAY;
  }
  catch (const std::exception & exc)
  {
    data->owner->setEvaluationError(exc.what());
    return SCIP_ERROR;
  }
}

/* Backward derivative callback */
SCIP_RETCODE SCIPProblem::bwdiffExpr(::SCIP * /* scip */, SCIP_EXPR * expr, int childIndex, SCIP_Real * value)
{
  FunctionData * data = reinterpret_cast<FunctionData *>(SCIPexprGetData(expr));
  if ((data == nullptr) || (data->owner == nullptr))
    return SCIP_ERROR;
  try
  {
    const Bool objective = (data->kind == FunctionKind::OBJECTIVE);
    if (objective && (childIndex == 0))
    {
      // d(f(x) - z)/dz
      *value = -1.0;
      return SCIP_OKAY;
    }

    const Point x(getEvalPoint(expr, objective));
    Matrix gradient;
    if (objective)
    {
      if (!data->owner->core_.evaluateObjectiveGradient(x, gradient))
      {
        data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
        return SCIP_ERROR;
      }
      *value = gradient(childIndex - 1, 0);
    }
    else
    {
      if (!data->owner->core_.evaluateConstraintMarginalGradient(data->marginalIndex, x, gradient))
      {
        data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
        return SCIP_ERROR;
      }
      *value = gradient(childIndex, 0);
    }
    return SCIP_OKAY;
  }
  catch (const std::exception & exc)
  {
    data->owner->setEvaluationError(exc.what());
    return SCIP_ERROR;
  }
}

/* Forward derivative callback */
SCIP_RETCODE SCIPProblem::fwdiffExpr(::SCIP * /* scip */, SCIP_EXPR * expr, SCIP_Real * dot, SCIP_SOL * /* direction */)
{
  FunctionData * data = reinterpret_cast<FunctionData *>(SCIPexprGetData(expr));
  if ((data == nullptr) || (data->owner == nullptr))
    return SCIP_ERROR;
  try
  {
    const Bool objective = (data->kind == FunctionKind::OBJECTIVE);
    const Point x(getEvalPoint(expr, objective));
    SCIP_EXPR ** children = SCIPexprGetChildren(expr);

    Matrix gradient;
    if (objective)
    {
      // The objective expression is f(x) - z: d/dz = -1 and d/dx_i = df/dx_i
      if (!data->owner->core_.evaluateObjectiveGradient(x, gradient))
      {
        data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
        return SCIP_ERROR;
      }
      Scalar dotValue = -SCIPexprGetDot(children[0]);
      for (int i = 0; i < static_cast<int>(x.getDimension()); ++ i)
        dotValue += gradient(i, 0) * SCIPexprGetDot(children[i + 1]);
      *dot = dotValue;
    }
    else
    {
      if (!data->owner->core_.evaluateConstraintMarginalGradient(data->marginalIndex, x, gradient))
      {
        data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
        return SCIP_ERROR;
      }
      Scalar dotValue = 0.0;
      for (int i = 0; i < static_cast<int>(x.getDimension()); ++ i)
        dotValue += gradient(i, 0) * SCIPexprGetDot(children[i]);
      *dot = dotValue;
    }
    return SCIP_OKAY;
  }
  catch (const std::exception & exc)
  {
    data->owner->setEvaluationError(exc.what());
    return SCIP_ERROR;
  }
}

/* Backward-forward derivative callback (Hessian times a direction) */
SCIP_RETCODE SCIPProblem::bwfwdiffExpr(::SCIP * /* scip */, SCIP_EXPR * expr, int childIndex,
                                       SCIP_Real * bardot, SCIP_SOL * /* direction */)
{
  FunctionData * data = reinterpret_cast<FunctionData *>(SCIPexprGetData(expr));
  if ((data == nullptr) || (data->owner == nullptr))
    return SCIP_ERROR;
  try
  {
    const Bool objective = (data->kind == FunctionKind::OBJECTIVE);
    if (objective && (childIndex == 0))
    {
      // The auxiliary variable enters the objective linearly
      *bardot = 0.0;
      return SCIP_OKAY;
    }

    const Point x(getEvalPoint(expr, objective));
    SCIP_EXPR ** children = SCIPexprGetChildren(expr);
    SymmetricMatrix hessian;
    const UnsignedInteger offset = objective ? 1 : 0;
    if (objective)
    {
      if (!data->owner->core_.evaluateObjectiveHessian(x, hessian))
      {
        data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
        return SCIP_ERROR;
      }
    }
    else
    {
      const UnsignedInteger equalityDimension = data->owner->core_.getEqualityDimension();
      SymmetricTensor tensor;
      if (data->marginalIndex < equalityDimension)
      {
        if (!data->owner->core_.evaluateEqualityConstraintHessian(x, tensor))
        {
          data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
          return SCIP_ERROR;
        }
        hessian = tensor.getSheet(data->marginalIndex);
      }
      else
      {
        if (!data->owner->core_.evaluateInequalityConstraintHessian(x, tensor))
        {
          data->owner->setEvaluationError(data->owner->core_.getLastErrorMessage());
          return SCIP_ERROR;
        }
        hessian = tensor.getSheet(data->marginalIndex - equalityDimension);
      }
    }

    Scalar bardotValue = 0.0;
    for (UnsignedInteger i = 0; i < x.getDimension(); ++ i)
      bardotValue += hessian(i, childIndex - offset) * SCIPexprGetDot(children[i + offset]);
    *bardot = bardotValue;
    return SCIP_OKAY;
  }
  catch (const std::exception & exc)
  {
    data->owner->setEvaluationError(exc.what());
    return SCIP_ERROR;
  }
}

/* Expression data copy callback */
SCIP_RETCODE SCIPProblem::copyData(::SCIP * /* targetSCIP */, SCIP_EXPRHDLR * /* targetExprhdlr */,
                                   SCIP_EXPRDATA ** targetData,
                                   ::SCIP * /* sourceSCIP */, SCIP_EXPR * sourceExpr)
{
  const FunctionData * data = reinterpret_cast<const FunctionData *>(SCIPexprGetData(sourceExpr));
  *targetData = (data != nullptr) ? reinterpret_cast<SCIP_EXPRDATA *>(new FunctionData(*data)) : nullptr;
  return SCIP_OKAY;
}

/* Expression data free callback */
SCIP_RETCODE SCIPProblem::freeData(::SCIP * /* scip */, SCIP_EXPR * expr)
{
  delete reinterpret_cast<FunctionData *>(SCIPexprGetData(expr));
  SCIPexprSetData(expr, nullptr);
  return SCIP_OKAY;
}

/* Catch the node events during the initialization of the solving process */
SCIP_RETCODE SCIPProblem::stopEventInit(::SCIP * scip, SCIP_EVENTHDLR * eventhdlr)
{
  return SCIPcatchEvent(scip, SCIP_EVENTTYPE_NODEFOCUSED, eventhdlr, nullptr, nullptr);
}

/* Poll the stop callback and the time budget at each node */
SCIP_RETCODE SCIPProblem::stopEventExec(::SCIP * /* scip */, SCIP_EVENTHDLR * eventhdlr,
                                        SCIP_EVENT * /* event */, SCIP_EVENTDATA * /* eventdata */)
{
  SCIPProblem * owner = reinterpret_cast<SCIPProblem *>(SCIPeventhdlrGetData(eventhdlr));
  if (owner != nullptr)
    owner->checkLimits();
  return SCIP_OKAY;
}

/* Release every SCIP object */
void SCIPProblem::release()
{
  if (scip_ == nullptr)
    return;

  if (objectiveVariable_ != nullptr)
  {
    SCIPreleaseVar(scip_, &objectiveVariable_);
    objectiveVariable_ = nullptr;
  }
  for (SCIP_VAR * & variable : variables_)
    if (variable != nullptr)
    {
      SCIPreleaseVar(scip_, &variable);
      variable = nullptr;
    }
  variables_.clear();

  for (SCIP_EXPR * & expr : variableExpressions_)
    if (expr != nullptr)
    {
      SCIPreleaseExpr(scip_, &expr);
      expr = nullptr;
    }
  variableExpressions_.clear();

  SCIPfree(&scip_);
  scip_ = nullptr;
  exprhdlr_ = nullptr;
  stopEventhdlr_ = nullptr;
}

/* Accessors */
Sample SCIPProblem::getInputHistory() const
{
  return core_.getInputHistory();
}

Sample SCIPProblem::getOutputHistory() const
{
  return core_.getOutputHistory();
}

UnsignedInteger SCIPProblem::getGradientCallsNumber() const
{
  return core_.getGradientCallsNumber();
}

UnsignedInteger SCIPProblem::getHessianCallsNumber() const
{
  return core_.getHessianCallsNumber();
}

Bool SCIPProblem::hasEqualityConstraint() const
{
  return core_.hasEqualityConstraint();
}

Bool SCIPProblem::hasInequalityConstraint() const
{
  return core_.hasInequalityConstraint();
}

Function SCIPProblem::getEqualityConstraint() const
{
  return core_.getEqualityConstraint();
}

Function SCIPProblem::getInequalityConstraint() const
{
  return core_.getInequalityConstraint();
}

OptimizationProblem SCIPProblem::getNormalizedProblem() const
{
  return core_.getNormalizedProblem();
}

Bool SCIPProblem::getTimeOut() const
{
  return core_.isTimeOut() || timeLimitReached_;
}

Bool SCIPProblem::getCallLimitReached() const
{
  return core_.isCallLimitReached() || callLimitReached_;
}

Bool SCIPProblem::getStopped() const
{
  return core_.isStopped();
}

Bool SCIPProblem::hasSolution() const
{
  return hasSolution_;
}

Point SCIPProblem::getOptimalPoint() const
{
  return optimalPoint_;
}

Scalar SCIPProblem::getOptimalValue() const
{
  return optimalValue_;
}

UnsignedInteger SCIPProblem::getIterationNumber() const
{
  return iterationNumber_;
}

OptimizationResult::Status SCIPProblem::getStatus() const
{
  return status_;
}

const String & SCIPProblem::getStatusMessage() const
{
  return statusMessage_;
}

void SCIPProblem::setProgressCallback(OptimizationAlgorithmImplementation::ProgressCallback callBack, void * state)
{
  core_.setProgressCallback(callBack, state);
}

void SCIPProblem::setStopCallback(OptimizationAlgorithmImplementation::StopCallback callBack, void * state)
{
  core_.setStopCallback(callBack, state);
}

END_NAMESPACE_OPENTURNS
