//                                               -*- C++ -*-
/**
 *  @brief FailurePenaltyProblem replaces the objective by a failure-penalty function
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
#ifndef OPENTURNS_FAILUREPENALTYPROBLEM_HXX
#define OPENTURNS_FAILUREPENALTYPROBLEM_HXX

#include <cmath>

#include "openturns/OptimizationProblemImplementation.hxx"
#include "openturns/OptimizationProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FailurePenaltyProblem
 *
 * This class wraps an OptimizationProblem and replaces its objective
 * by a failure-penalty function using FailurePenaltyEvaluation, so that optimization
 * algorithms can proceed when the objective throws. The penalized values
 * account for the minimization/maximization flags, including per-output
 * flags for multi-objective problems. Equality and inequality constraints
 * are penalized as well: a failed constraint evaluation is reported as
 * infeasible (+penalizedValue_ for equality constraints, -penalizedValue_
 * for inequality constraints).
 *
 * The penalized objective and constraints use finite-difference
 * gradient/hessian evaluated on the penalized evaluation, which never
 * throws. Analytic derivatives of the wrapped functions are not propagated.
 *
 * Note that the penalized functions are flat (constant) where the wrapped
 * evaluation fails, so finite-difference gradients vanish there.
 * Gradient-based solvers (TNC, SLSQP, LBFGS, ...) may mistake this null
 * gradient for stationarity and converge silently to the penalized value.
 * Use derivative-free solvers (Cobyla, Pagmo, Dlib, ...) with this class,
 * and check that the optimal value differs from the penalized value.
 */
class OT_API FailurePenaltyProblem
  : public OptimizationProblemImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  FailurePenaltyProblem();

#ifdef SWIG
  /** Dummy copy ctor to fix overload precedence */
  FailurePenaltyProblem(const FailurePenaltyProblem & other);
#endif

  /** Constructor with parameters */
  explicit FailurePenaltyProblem(const OptimizationProblem & problem,
                            const Scalar penalizedValue = std::sqrt(SpecFunc::MaxScalar));

  /** Virtual constructor */
  FailurePenaltyProblem * clone() const override;

  /** Problem accessor */
  OptimizationProblem getProblem() const;
  void setProblem(const OptimizationProblem & problem);

  /** Penalized value (magnitude) accessor */
  Scalar getPenalizedValue() const;
  void setPenalizedValue(const Scalar penalizedValue);

  /** Objective accessor, replaces the objective in the wrapped problem */
  void setObjective(const Function & objective);

  /** Equality constraint accessor */
  void setEqualityConstraint(const Function & equalityConstraint);

  /** Inequality constraint accessor */
  void setInequalityConstraint(const Function & inequalityConstraint);

  /** Bounds accessor */
  void setBounds(const Interval & bounds);

  /** Minimization accessor, updates penalized values sign */
  void setMinimization(Bool minimization, UnsignedInteger marginalIndex = 0);

  /** Variables type accessor */
  void setVariablesType(const Indices & variableType);

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Rebuild the penalized objective from the wrapped problem */
  void updateObjective();

  /** Rebuild the penalized constraints from the wrapped problem */
  void updateConstraints();

  /** Wrap a function with a penalized evaluation */
  static Function penalizeFunction(const Function & function,
                                   const Point & penalizedValues);

  /** Copy bounds, minimization flags and variables type */
  void syncFromProblem();

  // The wrapped (unpenalized) problem
  OptimizationProblem problem_;

  // Penalized magnitude (positive value, same for all outputs)
  Scalar penalizedValue_ = 0.0;

} ; /* class FailurePenaltyProblem */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FAILUREPENALTYPROBLEM_HXX */
