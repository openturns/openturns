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
#ifndef OPENTURNS_SCIPPROBLEM_HXX
#define OPENTURNS_SCIPPROBLEM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/OptimizationResult.hxx"
#include "openturns/MinlpProblem.hxx"

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <chrono>
#include <vector>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SCIPProblem
 *
 * Translation layer between an OpenTURNS optimization problem and the
 * SCIP C API.  It owns the SCIP instance, creates the variables, the
 * constraints and the objective, runs the solver and reports the raw
 * status.  The solver independent bookkeeping (evaluation history, call
 * and time budgets, progress and stop callbacks) is delegated to
 * MinlpProblem, shared with the Bonmin adapter.
 */
class SCIPProblem
{
public:

  /** Constructor with parameters */
  SCIPProblem(const OptimizationProblem & optimProblem,
              const Point & startingPoint,
              const UnsignedInteger maximumCallsNumber,
              const Scalar maximumTimeDuration,
              const UnsignedInteger maximumIterationNumber,
              const Scalar maximumConstraintError,
              const std::chrono::steady_clock::time_point & t0);

  /** Destructor */
  ~SCIPProblem();

  /** Solve the problem, the result is available through the accessors below */
  void run();

  /** Retrieving objective function input/output history */
  Sample getInputHistory() const;
  Sample getOutputHistory() const;

  /** Number of gradient/Hessian calls performed through this wrapper */
  UnsignedInteger getGradientCallsNumber() const;
  UnsignedInteger getHessianCallsNumber() const;

  /** Normalized constraint view, shared with the other MINLP adapters */
  Bool hasEqualityConstraint() const;
  Bool hasInequalityConstraint() const;
  Function getEqualityConstraint() const;
  Function getInequalityConstraint() const;
  OptimizationProblem getNormalizedProblem() const;

  /** Termination diagnostics */
  Bool getTimeOut() const;
  Bool getCallLimitReached() const;
  Bool getStopped() const;

  /** Solution and statistics */
  Bool hasSolution() const;
  Point getOptimalPoint() const;
  Scalar getOptimalValue() const;
  UnsignedInteger getIterationNumber() const;
  OptimizationResult::Status getStatus() const;
  const String & getStatusMessage() const;

  /** Callbacks */
  void setProgressCallback(OptimizationAlgorithmImplementation::ProgressCallback callBack,
                           void * state = nullptr);
  void setStopCallback(OptimizationAlgorithmImplementation::StopCallback callBack,
                       void * state = nullptr);

private:

  /** Kind of function evaluated by the expression handler */
  enum class FunctionKind {OBJECTIVE, EQUALITY, INEQUALITY};

  /** Data attached to a black-box expression */
  struct FunctionData
  {
    Function function;
    SCIPProblem * owner = nullptr;
    FunctionKind kind = FunctionKind::OBJECTIVE;

    // For a constraint, index in the normalized constraint list (equality
    // constraints first, then inequality constraints) used to report
    // evaluation failures through MinlpProblem.
    UnsignedInteger marginalIndex = 0;
  };

  /** Expression handler callbacks */
  static SCIP_RETCODE evalExpr(::SCIP * scip, SCIP_EXPR * expr, SCIP_Real * value, SCIP_SOL * sol);
  static SCIP_RETCODE bwdiffExpr(::SCIP * scip, SCIP_EXPR * expr, int childIndex, SCIP_Real * value);
  static SCIP_RETCODE fwdiffExpr(::SCIP * scip, SCIP_EXPR * expr, SCIP_Real * dot, SCIP_SOL * direction);
  static SCIP_RETCODE bwfwdiffExpr(::SCIP * scip, SCIP_EXPR * expr, int childIndex, SCIP_Real * bardot,
                                   SCIP_SOL * direction);
  static SCIP_RETCODE copyData(::SCIP * targetSCIP, SCIP_EXPRHDLR * targetExprhdlr,
                               SCIP_EXPRDATA ** targetData,
                               ::SCIP * sourceSCIP, SCIP_EXPR * sourceExpr);
  static SCIP_RETCODE freeData(::SCIP * scip, SCIP_EXPR * expr);

  /** Node event handler polling the stop callback and the time budget */
  static SCIP_RETCODE stopEventInit(::SCIP * scip, SCIP_EVENTHDLR * eventhdlr);
  static SCIP_RETCODE stopEventExec(::SCIP * scip, SCIP_EVENTHDLR * eventhdlr,
                                    SCIP_EVENT * event, SCIP_EVENTDATA * eventdata);

  /** Lifecycle helpers */
  void createSCIP();
  void setParameters();
  void includeExpressionHandler();
  void createVariables();
  void createLinearConstraints();
  void createExpressionConstraint(const Function & constraint,
                                  const UnsignedInteger marginalIndex,
                                  const FunctionKind kind,
                                  const String & name,
                                  const Scalar lhs,
                                  const Scalar rhs);
  void createExpressionObjective();
  void injectStartingPoint();
  void solve();
  void extractSolution();
  void release();

  OptimizationResult::Status computeStatus(const SCIP_STATUS scipStatus) const;
  String computeStatusMessage(const SCIP_STATUS scipStatus) const;

  /** Ask SCIP to stop when a limit is reached */
  Bool checkLimits();

  /** Values of the children of an expression */
  static Point getEvalPoint(SCIP_EXPR * expr, const Bool withObjectiveVariable);

  /** Record an evaluation error coming from a callback */
  void setEvaluationError(const String & message);

  // Solver independent part of the problem
  MinlpProblem core_;

  // Solver settings, kept for the callbacks
  Point startingPoint_;
  Scalar maximumTimeDuration_ = 0.0;
  UnsignedInteger maximumIterationNumber_ = 0;
  Scalar maximumConstraintError_ = 0.0;
  std::chrono::steady_clock::time_point t0_;

  // SCIP objects owned by this adapter
  ::SCIP * scip_ = nullptr;
  SCIP_EXPRHDLR * exprhdlr_ = nullptr;
  SCIP_EVENTHDLR * stopEventhdlr_ = nullptr;
  SCIP_VAR * objectiveVariable_ = nullptr;
  Collection<SCIP_VAR *> variables_;

  // SCIPcreateExpr takes a writable SCIP_EXPR ** array and Collection only
  // exposes a const data(), hence std::vector here.
  std::vector<SCIP_EXPR *> variableExpressions_;

  // Outcome of the last run
  Bool solved_ = false;
  Bool solveFailed_ = false;
  Bool hasSolution_ = false;
  Bool timeLimitReached_ = false;
  Bool callLimitReached_ = false;
  Point optimalPoint_;
  Scalar optimalValue_ = 0.0;
  UnsignedInteger iterationNumber_ = 0;
  OptimizationResult::Status status_ = OptimizationResult::FAILURE;
  String statusMessage_;
  String evaluationErrorMessage_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SCIPPROBLEM_HXX */
