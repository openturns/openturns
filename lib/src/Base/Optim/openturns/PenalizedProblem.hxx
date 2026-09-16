//                                               -*- C++ -*-
/**
 *  @brief PenalizedProblem replaces the objective by a penalized function
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
#ifndef OPENTURNS_PENALIZEDPROBLEM_HXX
#define OPENTURNS_PENALIZEDPROBLEM_HXX

#include <cmath>

#include "openturns/OptimizationProblemImplementation.hxx"
#include "openturns/OptimizationProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PenalizedProblem
 *
 * This class wraps an OptimizationProblem and replaces its objective
 * by a penalized function using PenalizedEvaluation, so that optimization
 * algorithms can proceed when the objective throws. The penalized values
 * account for the minimization/maximization flags, including per-output
 * flags for multi-objective problems. Analytic gradient/hessian are wrapped
 * with PenalizedGradient/PenalizedHessian and return zeros on failure;
 * finite-difference gradient/hessian need no wrapping as they are evaluated
 * on the penalized evaluation.
 */
class OT_API PenalizedProblem
  : public OptimizationProblemImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PenalizedProblem();

  /** Constructor with parameters */
  explicit PenalizedProblem(const OptimizationProblem & problem,
                            const Scalar penalizedValue = std::sqrt(SpecFunc::MaxScalar));

  /** Virtual constructor */
  PenalizedProblem * clone() const override;

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

  /** Copy constraints, bounds, minimization flags and variables type */
  void syncFromProblem();

  // The wrapped (unpenalized) problem
  OptimizationProblem problem_;

  // Penalized magnitude (positive value, same for all outputs)
  Scalar penalizedValue_ = 0.0;

} ; /* class PenalizedProblem */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PENALIZEDPROBLEM_HXX */
