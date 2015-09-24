//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblemImplementation allows to describe an optimization problem
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cstdlib>

#include "OptimizationProblemImplementation.hxx"
#include "ResourceMap.hxx"
#include "PersistentObjectFactory.hxx"
#include "SymmetricTensor.hxx"
#include "IdentityMatrix.hxx"
#include "QuadraticNumericalMathFunction.hxx"
#include "LinearNumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationProblemImplementation);

static Factory<OptimizationProblemImplementation> RegisteredFactory("OptimizationProblemImplementation");

/* Default constructor */
OptimizationProblemImplementation::OptimizationProblemImplementation()
  : PersistentObject()
  , objective_()
  , equalityConstraint_()
  , inequalityConstraint_()
  , bounds_()
  , minimization_(true)
  , dimension_(0)
{
  // Nothing to do
}

/*
 * @brief General multi-objective equality, inequality and bound constraints
 */
OptimizationProblemImplementation::OptimizationProblemImplementation(const NumericalMathFunction & objective,
    const NumericalMathFunction & equalityConstraint,
    const NumericalMathFunction & inequalityConstraint,
    const Interval & bounds)
  : PersistentObject()
  , objective_(objective)
  , equalityConstraint_(equalityConstraint)
  , inequalityConstraint_(inequalityConstraint)
  , bounds_(bounds)
  , minimization_(true)
  , dimension_(objective.getInputDimension())
{
  // Check if the input dimension of the objective, the constraints and the bounds are compatible
  if (hasEqualityConstraint() && (equalityConstraint.getInputDimension() != dimension_)) throw InvalidArgumentException(HERE) << "Error: the given equality constraints have an input dimension=" << equalityConstraint.getInputDimension() << " different from the input dimension=" << dimension_ << " of the objective.";
  if (hasInequalityConstraint() && (inequalityConstraint.getInputDimension() != dimension_)) throw InvalidArgumentException(HERE) << "Error: the given inequality constraints have an input dimension=" << inequalityConstraint.getInputDimension() << " different from the input dimension=" << dimension_ << " of the objective.";
  if (hasInequalityConstraint() && (inequalityConstraint.getInputDimension() != dimension_)) throw InvalidArgumentException(HERE) << "Error: the given inequality constraints have an input dimension=" << inequalityConstraint.getInputDimension() << " different from the input dimension=" << dimension_ << " of the objective.";
}

/* Constructor for nearest point problem */
OptimizationProblemImplementation::OptimizationProblemImplementation(const NumericalMathFunction & levelFunction,
                                                                     NumericalScalar levelValue)
  : PersistentObject()
  , objective_()
  , equalityConstraint_()
  , inequalityConstraint_()
  , bounds_()
  , levelValue_(levelValue)
  , minimization_(true)
  , dimension_(0)
{
  setLevelFunction(levelFunction);
}

/* Virtual constructor */
OptimizationProblemImplementation * OptimizationProblemImplementation::clone() const
{
  return new OptimizationProblemImplementation(*this);
}

/* Objective accessor */
NumericalMathFunction OptimizationProblemImplementation::getObjective() const
{
  return objective_;
}

void OptimizationProblemImplementation::setObjective(const NumericalMathFunction & objective)
{
  clearLevelFunction();

  objective_ = objective;
  // Update dimension_ member accordingly
  dimension_ = objective.getInputDimension();
}

Bool OptimizationProblemImplementation::hasMultipleObjective() const
{
  return objective_.getOutputDimension() > 1;
}

/* Equality constraint accessor */
NumericalMathFunction OptimizationProblemImplementation::getEqualityConstraint() const
{
  return equalityConstraint_;
}

void OptimizationProblemImplementation::setEqualityConstraint(const NumericalMathFunction & equalityConstraint)
{
  clearLevelFunction();
  equalityConstraint_ = equalityConstraint;
}

Bool OptimizationProblemImplementation::hasEqualityConstraint() const
{
  return equalityConstraint_.getEvaluation()->isActualImplementation();
}

/* Inequality constraint accessor */
NumericalMathFunction OptimizationProblemImplementation::getInequalityConstraint() const
{
  return inequalityConstraint_;
}

void OptimizationProblemImplementation::setInequalityConstraint(const NumericalMathFunction & inequalityConstraint)
{
  clearLevelFunction();
  inequalityConstraint_ = inequalityConstraint;
}

Bool OptimizationProblemImplementation::hasInequalityConstraint() const
{
  return inequalityConstraint_.getEvaluation()->isActualImplementation();
}

/* Bounds accessor */
Interval OptimizationProblemImplementation::getBounds() const
{
  return bounds_;
}

void OptimizationProblemImplementation::setBounds(const Interval & bounds)
{
  bounds_ = bounds;
}

Bool OptimizationProblemImplementation::hasBounds() const
{
  return bounds_.getDimension() == dimension_;
}

/* Level function accessor */
NumericalMathFunction OptimizationProblemImplementation::getLevelFunction() const
{
  return levelFunction_;
}

void OptimizationProblemImplementation::setLevelFunction(const NumericalMathFunction & levelFunction)
{
  if (levelFunction.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: level function has an output dimension=" << levelFunction.getOutputDimension() << " but only dimension 1 is supported.";

  levelFunction_ = levelFunction;
  dimension_ = levelFunction_.getInputDimension();
  // Update objective function
  const NumericalPoint center(dimension_);
  const NumericalPoint constant(1);
  const Matrix linear(dimension_, 1);
  const IdentityMatrix identity(dimension_);
  const SymmetricTensor quadratic(dimension_, 1, *(identity.getImplementation().get()));
  objective_ = QuadraticNumericalMathFunction(center, constant, linear, quadratic);
  setNearestPointConstraints();
}

Bool OptimizationProblemImplementation::hasLevelFunction() const
{
  return levelFunction_.getEvaluation()->isActualImplementation();
}

/* Level value accessor */
NumericalScalar OptimizationProblemImplementation::getLevelValue() const
{
  return levelValue_;
}

void OptimizationProblemImplementation::setLevelValue(NumericalScalar levelValue)
{
  levelValue_ = levelValue;
  // Update constraints
  if (hasLevelFunction()) setNearestPointConstraints();
}

void OptimizationProblemImplementation::setNearestPointConstraints()
{
  const NumericalPoint center(dimension_);
  const Matrix linear(dimension_, 1);
  LinearNumericalMathFunction constantFunction(center, NumericalPoint(1, levelValue_), linear.transpose());
  NumericalMathFunction equalityConstraint(levelFunction_);
  equalityConstraint_ = equalityConstraint.operator-(constantFunction);
  inequalityConstraint_ = NumericalMathFunction();
}

void OptimizationProblemImplementation::clearLevelFunction()
{
  levelFunction_ = NumericalMathFunction();
  levelValue_ = 0.0;
}

/* Dimension accessor */
UnsignedInteger OptimizationProblemImplementation::getDimension() const
{
  return dimension_;
}

/* Minimization accessor */
void OptimizationProblemImplementation::setMinimization(Bool minimization)
{
  minimization_ = minimization;
}

Bool OptimizationProblemImplementation::isMinimization() const
{
  return minimization_;
}

/* String converter */
String OptimizationProblemImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationProblemImplementation::GetClassName();
  if (hasLevelFunction())
  {
    oss << " level function=" << levelFunction_.__repr__()
        << " level value=" << levelValue_;
  }
  else
  {
    oss << " objective=" << objective_
        << " equality constraint=" << (hasEqualityConstraint() ? equalityConstraint_.__repr__() : "none")
        << " inequality constraint=" << (hasInequalityConstraint() ? inequalityConstraint_.__repr__() : "none");
  }
  oss << " bounds=" << (hasBounds() ? bounds_.__repr__() : "none")
      << " minimization=" << minimization_
      << " dimension=" << dimension_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void OptimizationProblemImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "objective_", objective_ );
  adv.saveAttribute( "equalityConstraint_", equalityConstraint_ );
  adv.saveAttribute( "inequalityConstraint_", inequalityConstraint_ );
  adv.saveAttribute( "bounds_", bounds_ );
  adv.saveAttribute( "minimization_", minimization_ );
  adv.saveAttribute( "dimension_", dimension_ );
}

/* Method load() reloads the object from the StorageManager */
void OptimizationProblemImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "objective_", objective_ );
  adv.loadAttribute( "equalityConstraint_", equalityConstraint_ );
  adv.loadAttribute( "inequalityConstraint_", inequalityConstraint_ );
  adv.loadAttribute( "bounds_", bounds_ );
  adv.loadAttribute( "minimization_", minimization_ );
  adv.loadAttribute( "dimension_", dimension_ );
}

END_NAMESPACE_OPENTURNS
