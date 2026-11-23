//                                               -*- C++ -*-
/**
 *  @brief MinlpProblem implements the solver independent part of a
 * mixed-integer nonlinear problem.
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
#ifndef OPENTURNS_MINLPPROBLEM_HXX
#define OPENTURNS_MINLPPROBLEM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/OptimizationProblem.hxx"
#include "openturns/OptimizationAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/Function.hxx"
#include "openturns/Interval.hxx"

#include <chrono>
#include <utility>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MinlpProblem
 *
 * Solver independent description of a mixed-integer nonlinear problem.
 *
 * The class owns the problem, the starting point, the evaluation history,
 * the time and evaluation budgets, the progress and stop callbacks and the
 * gradient/Hessian counters.  Solver specific adapters (BonminProblem,
 * ScipProblem, ...) only translate their native callbacks to the methods
 * below.
 *
 * The constraint view is normalized: when the problem is a structured
 * LinearProgrammingProblem, its matrix form L <= A.x <= U is exposed as generic
 * equality and inequality functions, so that solvers relying on the generic
 * constraint API do not silently ignore it.
 */
class OT_API MinlpProblem
{
public:

  /** Constructor with parameters */
  MinlpProblem(const OptimizationProblem & problem,
               const Point & startingPoint,
               const UnsignedInteger maximumCallsNumber,
               const Scalar maximumTimeDuration,
               const std::chrono::steady_clock::time_point & t0);

  /** Problem description */
  UnsignedInteger getDimension() const;
  const OptimizationProblem & getProblem() const;
  const Point & getStartingPoint() const;

  /** Normalized constraint accessors */
  Bool hasEqualityConstraint() const;
  Bool hasInequalityConstraint() const;
  UnsignedInteger getEqualityDimension() const;
  UnsignedInteger getInequalityDimension() const;
  Function getEqualityConstraint() const;
  Function getInequalityConstraint() const;

  /**
   * Problem with the normalized constraints materialized as functions.
   * It is equal to getProblem() when the problem is not a structured
   * linear problem, and is used to compute the constraint errors of the
   * result.
   */
  OptimizationProblem getNormalizedProblem() const;

  /** Structured accessors, only valid when the problem is linear */
  Bool isLinear() const;
  Point getLinearCost() const;
  Matrix getLinearConstraintCoefficients() const;
  Interval getLinearConstraintBounds() const;

  /** Evaluation with history and budget, return false when the evaluation failed */
  Bool evaluateObjective(const Point & x, Scalar & value);
  Bool evaluateEqualityConstraint(const Point & x, Point & values);
  Bool evaluateInequalityConstraint(const Point & x, Point & values);
  Bool evaluateConstraintMarginal(const UnsignedInteger marginalIndex,
                                  const Point & x, Scalar & value);

  /** Gradient and Hessian evaluations, they increment their call counters */
  Bool evaluateObjectiveGradient(const Point & x, Matrix & gradient);
  Bool evaluateEqualityConstraintGradient(const Point & x, Matrix & jacobian);
  Bool evaluateInequalityConstraintGradient(const Point & x, Matrix & jacobian);
  Bool evaluateConstraintMarginalGradient(const UnsignedInteger marginalIndex,
                                          const Point & x, Matrix & gradient);
  Bool evaluateObjectiveHessian(const Point & x, SymmetricMatrix & hessian);
  Bool evaluateEqualityConstraintHessian(const Point & x, SymmetricTensor & hessian);
  Bool evaluateInequalityConstraintHessian(const Point & x, SymmetricTensor & hessian);

  /** Whether the budget is still available */
  Bool checkBudget() const;

  /** Callbacks */
  void setProgressCallback(OptimizationAlgorithmImplementation::ProgressCallback callBack,
                           void * state = nullptr);
  void setStopCallback(OptimizationAlgorithmImplementation::StopCallback callBack,
                       void * state = nullptr);

  /** Termination diagnostics */
  Bool isTimeOut() const;
  Bool isCallLimitReached() const;
  Bool isStopped() const;
  const String & getLastErrorMessage() const;

  /** Objective evaluation history and counters */
  Sample getInputHistory() const;
  Sample getOutputHistory() const;
  UnsignedInteger getGradientCallsNumber() const;
  UnsignedInteger getHessianCallsNumber() const;

private:

  /** Build the normalized constraint view */
  void initializeConstraints();

  /** Check the time limit, the callbacks and the call budget */
  Bool checkTermination();

  /** Record an evaluation failure */
  Bool evaluationFailed(const Point & x, const String & message);

  const OptimizationProblem problem_;
  const Point startingPoint_;

  // Normalized constraint view
  Bool hasEqualityConstraint_ = false;
  Bool hasInequalityConstraint_ = false;
  UnsignedInteger equalityDimension_ = 0;
  UnsignedInteger inequalityDimension_ = 0;
  Function equalityConstraint_;
  Function inequalityConstraint_;

  // Objective evaluation history
  Sample evaluationInputHistory_;
  Sample evaluationOutputHistory_;

  // Budget
  UnsignedInteger maximumCallsNumber_ = 0;
  Scalar maximumTimeDuration_ = 0.0;
  Bool timeOut_ = false;
  Bool callLimitReached_ = false;
  Bool stopped_ = false;
  std::chrono::steady_clock::time_point t0_;

  // Callbacks
  std::pair< OptimizationAlgorithmImplementation::ProgressCallback, void *> progressCallback_;
  std::pair< OptimizationAlgorithmImplementation::StopCallback, void *> stopCallback_;

  // gradient/hessian calls performed through this adapter
  UnsignedInteger gradientCallsNumber_ = 0;
  UnsignedInteger hessianCallsNumber_ = 0;

  // Last evaluation error
  String lastErrorMessage_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MINLPPROBLEM_HXX */
