//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblemImplementation allows to describe an optimization problem
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdlib>

#include "openturns/OptimizationProblemImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationProblemImplementation)

static const Factory<OptimizationProblemImplementation> Factory_OptimizationProblemImplementation;

/* Default constructor */
OptimizationProblemImplementation::OptimizationProblemImplementation()
  : PersistentObject()
  , objective_()
  , equalityConstraint_()
  , inequalityConstraint_()
  , bounds_()
  , minimization_(true)
  , dimension_(0)
  , variablesType_(Indices(0, CONTINUOUS))
{
  // Nothing to do
}

OptimizationProblemImplementation::OptimizationProblemImplementation(const Function & objective)
  : PersistentObject()
  , objective_()
  , minimization_(true)
  , dimension_(objective.getInputDimension())
  , variablesType_(Indices(dimension_, CONTINUOUS))
{
  // Set objective function
  setObjective(objective);
}

/*
 * @brief General multi-objective equality, inequality and bound constraints
 */
OptimizationProblemImplementation::OptimizationProblemImplementation( const Function & objective,
    const Function & equalityConstraint,
    const Function & inequalityConstraint,
    const Interval & bounds)
  : PersistentObject()
  , objective_()
  , minimization_(true)
  , dimension_(objective.getInputDimension())
  , variablesType_(Indices(dimension_, CONTINUOUS))
{
  // Set objective function and variables types
  setObjective(objective);

  // Set constraints
  setEqualityConstraint(equalityConstraint);
  setInequalityConstraint(inequalityConstraint);

  // Set bounds
  setBounds(bounds);
}


/* Virtual constructor */
OptimizationProblemImplementation * OptimizationProblemImplementation::clone() const
{
  return new OptimizationProblemImplementation(*this);
}

/* Objective accessor */
Function OptimizationProblemImplementation::getObjective() const
{
  return objective_;
}

void OptimizationProblemImplementation::setObjective(const Function & objective)
{
  if (objective.getInputDimension() != objective_.getInputDimension())
  {
    LOGWARN(OSS() << "Clearing constraints, bounds and variables types");

    // Clear constraints
    if (equalityConstraint_.getEvaluation().getImplementation()->isActualImplementation() || inequalityConstraint_.getEvaluation().getImplementation()->isActualImplementation())
    {
      equalityConstraint_ = Function();
      inequalityConstraint_ = Function();
    }

    // Clear bounds
    bounds_ = Interval(0);

    // Clear variables types
    variablesType_ = Indices(dimension_, CONTINUOUS);

  }
  objective_ = objective;

  // Update dimension_ member accordingly
  dimension_ = objective.getInputDimension();

  // Update variablesType_ member accordingly
  variablesType_ = Indices(dimension_, CONTINUOUS);
}

Bool OptimizationProblemImplementation::hasMultipleObjective() const
{
  return objective_.getOutputDimension() > 1;
}

/* Equality constraint accessor */
Function OptimizationProblemImplementation::getEqualityConstraint() const
{
  return equalityConstraint_;
}

void OptimizationProblemImplementation::setEqualityConstraint(const Function & equalityConstraint)
{
  if ((equalityConstraint.getInputDimension() > 0) && (equalityConstraint.getInputDimension() != dimension_))
    throw InvalidArgumentException(HERE) << "Error: the given equality constraints have an input dimension=" << equalityConstraint.getInputDimension() << " different from the input dimension=" << dimension_ << " of the objective.";

  equalityConstraint_ = equalityConstraint;
}

Bool OptimizationProblemImplementation::hasEqualityConstraint() const
{
  return equalityConstraint_.getEvaluation().getImplementation()->isActualImplementation();
}

/* Inequality constraint accessor */
Function OptimizationProblemImplementation::getInequalityConstraint() const
{
  return inequalityConstraint_;
}

void OptimizationProblemImplementation::setInequalityConstraint(const Function & inequalityConstraint)
{
  if ((inequalityConstraint.getInputDimension() > 0) && (inequalityConstraint.getInputDimension() != dimension_)) throw InvalidArgumentException(HERE) << "Error: the given inequality constraints have an input dimension=" << inequalityConstraint.getInputDimension() << " different from the input dimension=" << dimension_ << " of the objective.";

  inequalityConstraint_ = inequalityConstraint;
}

Bool OptimizationProblemImplementation::hasInequalityConstraint() const
{
  return inequalityConstraint_.getEvaluation().getImplementation()->isActualImplementation();
}

/* Bounds accessor */
Interval OptimizationProblemImplementation::getBounds() const
{
  return bounds_;
}

void OptimizationProblemImplementation::setBounds(const Interval & bounds)
{
  if ((bounds.getDimension() > 0) && (bounds.getDimension() != dimension_)) throw InvalidArgumentException(HERE) << "Error: the given bounds are of dimension=" << bounds.getDimension() << " different from the input dimension=" << dimension_ << " of the objective.";

  bounds_ = bounds;
}

Bool OptimizationProblemImplementation::hasBounds() const
{
  return bounds_.getDimension() > 0;
}

/* Level function accessor */
Function OptimizationProblemImplementation::getLevelFunction() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getLevelFunction";
}

void OptimizationProblemImplementation::setLevelFunction(const Function & levelFunction)
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::setLevelFunction";
}

Bool OptimizationProblemImplementation::hasLevelFunction() const
{
  return false;
}

Function OptimizationProblemImplementation::getResidualFunction() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getResidualFunction";
}

void OptimizationProblemImplementation::setResidualFunction(const Function & residualFunction)
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::setResidualFunction";
}

Bool OptimizationProblemImplementation::hasResidualFunction() const
{
  return false;
}

/* Level value accessor */
Scalar OptimizationProblemImplementation::getLevelValue() const
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::getLevelValue";
}

void OptimizationProblemImplementation::setLevelValue(Scalar levelValue)
{
  throw NotYetImplementedException(HERE) << "in OptimizationProblemImplementation::setLevelValue";
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

// Variables type table
void OptimizationProblemImplementation::setVariablesType(const Indices & variablesType)
{
  if (variablesType.getSize() != getDimension())
    throw InvalidDimensionException(HERE) << "variables type table dimension is invalid (" << variablesType.getSize() << ", expected " << getDimension();

  variablesType_ = variablesType;
}

Indices OptimizationProblemImplementation::getVariablesType() const
{
  return variablesType_;
}

bool OptimizationProblemImplementation::isContinuous() const
{
  if (dimension_ == 0)
    return true;

  for (UnsignedInteger i = 0; i < dimension_; ++i)
    if (variablesType_[i] != CONTINUOUS) return false;

  return true;
}

/* String converter */
String OptimizationProblemImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << OptimizationProblemImplementation::GetClassName();
  oss << " objective=" << objective_
      << " equality constraint=" << (hasEqualityConstraint() ? equalityConstraint_.__repr__() : "none")
      << " inequality constraint=" << (hasInequalityConstraint() ? inequalityConstraint_.__repr__() : "none");
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
