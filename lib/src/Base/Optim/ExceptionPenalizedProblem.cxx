//                                               -*- C++ -*-
/**
 *  @brief ExceptionPenalizedProblem replaces the objective by a failure-penalty function
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

#include "openturns/ExceptionPenalizedProblem.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ExceptionPenalizedEvaluation.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExceptionPenalizedProblem)

static const Factory<ExceptionPenalizedProblem> Factory_ExceptionPenalizedProblem;

/* Default constructor */
ExceptionPenalizedProblem::ExceptionPenalizedProblem()
  : OptimizationProblemImplementation()
  , problem_()
  , penalizedValue_(std::sqrt(SpecFunc::MaxScalar))
{
  // Nothing to do
}

/* Constructor with parameters */
ExceptionPenalizedProblem::ExceptionPenalizedProblem(const OptimizationProblem & problem,
                                   const Scalar penalizedValue)
  : OptimizationProblemImplementation()
  , problem_(problem)
  , penalizedValue_(penalizedValue)
{
  if (!(penalizedValue > 0.0) || !std::isfinite(penalizedValue))
    throw InvalidArgumentException(HERE) << "ExceptionPenalizedProblem: penalized value must be finite and strictly positive";
  syncFromProblem();
  updateObjective();
  updateConstraints();
}

/* Virtual constructor */
ExceptionPenalizedProblem * ExceptionPenalizedProblem::clone() const
{
  return new ExceptionPenalizedProblem(*this);
}

/* Problem accessor */
OptimizationProblem ExceptionPenalizedProblem::getProblem() const
{
  return problem_;
}

void ExceptionPenalizedProblem::setProblem(const OptimizationProblem & problem)
{
  problem_ = problem;
  syncFromProblem();
  updateObjective();
  updateConstraints();
}

/* Penalized value (magnitude) accessor */
Scalar ExceptionPenalizedProblem::getPenalizedValue() const
{
  return penalizedValue_;
}

void ExceptionPenalizedProblem::setPenalizedValue(const Scalar penalizedValue)
{
  if (!(penalizedValue > 0.0) || !std::isfinite(penalizedValue))
    throw InvalidArgumentException(HERE) << "ExceptionPenalizedProblem: penalized value must be finite and strictly positive";
  penalizedValue_ = penalizedValue;
  updateObjective();
  updateConstraints();
}

/* Objective accessor, replaces the objective in the wrapped problem */
void ExceptionPenalizedProblem::setObjective(const Function & objective)
{
  problem_.setObjective(objective);
  syncFromProblem();
  updateObjective();
  updateConstraints();
}

/* Equality constraint accessor */
void ExceptionPenalizedProblem::setEqualityConstraint(const Function & equalityConstraint)
{
  problem_.setEqualityConstraint(equalityConstraint);
  syncFromProblem();
  updateConstraints();
}

/* Inequality constraint accessor */
void ExceptionPenalizedProblem::setInequalityConstraint(const Function & inequalityConstraint)
{
  problem_.setInequalityConstraint(inequalityConstraint);
  syncFromProblem();
  updateConstraints();
}

/* Bounds accessor */
void ExceptionPenalizedProblem::setBounds(const Interval & bounds)
{
  problem_.setBounds(bounds);
  OptimizationProblemImplementation::setBounds(bounds);
}

/* Minimization accessor, updates penalized values sign */
void ExceptionPenalizedProblem::setMinimization(Bool minimization, UnsignedInteger marginalIndex)
{
  problem_.setMinimization(minimization, marginalIndex);
  OptimizationProblemImplementation::setMinimization(minimization, marginalIndex);
  updateObjective();
}

/* Variables type accessor */
void ExceptionPenalizedProblem::setVariablesType(const Indices & variableType)
{
  problem_.setVariablesType(variableType);
  OptimizationProblemImplementation::setVariablesType(variableType);
}

/* String converter */
String ExceptionPenalizedProblem::__repr__() const
{
  OSS oss;
  oss << "class=" << ExceptionPenalizedProblem::GetClassName()
      << " problem=" << problem_.__repr__()
      << " penalizedValue=" << penalizedValue_;
  return oss;
}

/* Copy bounds, minimization flags and variables type */
void ExceptionPenalizedProblem::syncFromProblem()
{
  const Function objective(problem_.getObjective());
  const UnsignedInteger outputDimension = objective.getOutputDimension();
  dimension_ = problem_.getDimension();
  bounds_ = problem_.hasBounds() ? problem_.getBounds() : Interval();
  variablesType_ = problem_.getVariablesType();
  minimizationCollection_ = BoolPersistentCollection(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++ i) minimizationCollection_[i] = problem_.isMinimization(i);
}

/* Wrap a function with a penalized evaluation, using the given per-output penalized values.
 * The penalized function keeps finite-difference gradient/hessian evaluated on the
 * penalized evaluation, which never throws. Analytic derivatives are intentionally
 * not propagated: see the class documentation. */
Function ExceptionPenalizedProblem::penalizeFunction(const Function & function,
    const Point & penalizedValues)
{
  const ExceptionPenalizedEvaluation penalizedEvaluation(function.getEvaluation(), penalizedValues);
  Function penalizedFunction(penalizedEvaluation);
  penalizedFunction.setName(function.getName());
  penalizedFunction.setDescription(function.getDescription());
  return penalizedFunction;
}

/* Rebuild the penalized objective from the wrapped problem */
void ExceptionPenalizedProblem::updateObjective()
{
  const Function originalObjective(problem_.getObjective());
  if (!originalObjective.getEvaluation().getImplementation()->isActualImplementation())
  {
    objective_ = originalObjective;
    return;
  }
  const UnsignedInteger outputDimension = originalObjective.getOutputDimension();
  Point signedValues(outputDimension, penalizedValue_);
  for (UnsignedInteger i = 0; i < outputDimension; ++ i)
    if (!problem_.isMinimization(i)) signedValues[i] = -signedValues[i];
  objective_ = penalizeFunction(originalObjective, signedValues);
}

/* Rebuild the penalized constraints from the wrapped problem.
 * A failed constraint evaluation is reported as infeasible:
 * equality constraints (g = 0) return +penalizedValue_,
 * inequality constraints (h >= 0) return -penalizedValue_. */
void ExceptionPenalizedProblem::updateConstraints()
{
  if (problem_.hasEqualityConstraint())
  {
    const Function originalConstraint(problem_.getEqualityConstraint());
    const UnsignedInteger outputDimension = originalConstraint.getOutputDimension();
    equalityConstraint_ = penalizeFunction(originalConstraint, Point(outputDimension, penalizedValue_));
  }
  else
    equalityConstraint_ = Function();
  if (problem_.hasInequalityConstraint())
  {
    const Function originalConstraint(problem_.getInequalityConstraint());
    const UnsignedInteger outputDimension = originalConstraint.getOutputDimension();
    inequalityConstraint_ = penalizeFunction(originalConstraint, Point(outputDimension, -penalizedValue_));
  }
  else
    inequalityConstraint_ = Function();
}

/* Method save() stores the object through the StorageManager */
void ExceptionPenalizedProblem::save(Advocate & adv) const
{
  OptimizationProblemImplementation::save(adv);
  adv.saveAttribute("problem_", problem_);
  adv.saveAttribute("penalizedValue_", penalizedValue_);
}

/* Method load() reloads the object from the StorageManager */
void ExceptionPenalizedProblem::load(Advocate & adv)
{
  OptimizationProblemImplementation::load(adv);
  adv.loadAttribute("problem_", problem_);
  adv.loadAttribute("penalizedValue_", penalizedValue_);
}

END_NAMESPACE_OPENTURNS
