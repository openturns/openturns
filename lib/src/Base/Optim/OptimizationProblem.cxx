//                                               -*- C++ -*-
/**
 *  @brief OptimizationProblem implements an algorithm for finding the
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OptimizationProblem.hxx"
#include "openturns/NearestPointProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OptimizationProblem)

/* Default constructor */
OptimizationProblem::OptimizationProblem() :
  TypedInterfaceObject<OptimizationProblemImplementation>(new OptimizationProblemImplementation())
{
  // Nothing to do
}

/* Constructor from an implementation */
OptimizationProblem::OptimizationProblem(const OptimizationProblemImplementation & implementation) :
  TypedInterfaceObject<OptimizationProblemImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
OptimizationProblem::OptimizationProblem(const Implementation & p_implementation) :
  TypedInterfaceObject<OptimizationProblemImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor with constraints, bounds */
OptimizationProblem::OptimizationProblem(const Function & objective)
  : TypedInterfaceObject<OptimizationProblemImplementation>(new OptimizationProblemImplementation(objective))
{
  // Nothing to do
}

/* Constructor with constraints, bounds */
OptimizationProblem::OptimizationProblem(const Function & objective,
    const Function & equalityConstraint,
    const Function & inequalityConstraint,
    const Interval & bounds):
  TypedInterfaceObject<OptimizationProblemImplementation>(new OptimizationProblemImplementation(objective, equalityConstraint, inequalityConstraint, bounds))
{
  // Nothing to do
}


/* Objective accessor */
Function OptimizationProblem::getObjective() const
{
  return getImplementation()->getObjective();
}

void OptimizationProblem::setObjective(const Function & objective)
{
  copyOnWrite();
  getImplementation()->setObjective(objective);
}

Bool OptimizationProblem::hasMultipleObjective() const
{
  return getImplementation()->hasMultipleObjective();
}

/* Equality constraint accessor */
Function OptimizationProblem::getEqualityConstraint() const
{
  return getImplementation()->getEqualityConstraint();
}

void OptimizationProblem::setEqualityConstraint(const Function & equalityConstraint)
{
  copyOnWrite();
  getImplementation()->setEqualityConstraint(equalityConstraint);
}

Bool OptimizationProblem::hasEqualityConstraint() const
{
  return getImplementation()->hasEqualityConstraint();
}

/* Inequality constraint accessor */
Function OptimizationProblem::getInequalityConstraint() const
{
  return getImplementation()->getInequalityConstraint();
}

void OptimizationProblem::setInequalityConstraint(const Function & inequalityConstraint)
{
  copyOnWrite();
  getImplementation()->setInequalityConstraint(inequalityConstraint);
}

Bool OptimizationProblem::hasInequalityConstraint() const
{
  return getImplementation()->hasInequalityConstraint();
}

/* Bounds accessor */
Interval OptimizationProblem::getBounds() const
{
  return getImplementation()->getBounds();
}

void OptimizationProblem::setBounds(const Interval & bounds)
{
  copyOnWrite();
  getImplementation()->setBounds(bounds);
}

Bool OptimizationProblem::hasBounds() const
{
  return getImplementation()->hasBounds();
}

/* Level function accessor */
Function OptimizationProblem::getLevelFunction() const
{
  return getImplementation()->getLevelFunction();
}

void OptimizationProblem::setLevelFunction(const Function & levelFunction)
{
  copyOnWrite();
  getImplementation()->setLevelFunction(levelFunction);
}

Bool OptimizationProblem::hasLevelFunction() const
{
  return getImplementation()->hasLevelFunction();
}

/* Level value accessor */
Scalar OptimizationProblem::getLevelValue() const
{
  return getImplementation()->getLevelValue();
}

void OptimizationProblem::setLevelValue(Scalar levelValue)
{
  copyOnWrite();
  getImplementation()->setLevelValue(levelValue);
}

/* Residual function accessor */
Function OptimizationProblem::getResidualFunction() const
{
  return getImplementation()->getResidualFunction();
}

void OptimizationProblem::setResidualFunction(const Function & levelFunction)
{
  copyOnWrite();
  getImplementation()->setResidualFunction(levelFunction);
}

Bool OptimizationProblem::hasResidualFunction() const
{
  return getImplementation()->hasResidualFunction();
}

/* Dimension accessor */
UnsignedInteger OptimizationProblem::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Minimization accessor */
void OptimizationProblem::setMinimization(Bool minimization, UnsignedInteger marginalIndex)
{
  copyOnWrite();
  getImplementation()->setMinimization(minimization, marginalIndex);
}

Bool OptimizationProblem::isMinimization(UnsignedInteger marginalIndex) const
{
  return getImplementation()->isMinimization(marginalIndex);
}

/* Variables type table */
void OptimizationProblem::setVariablesType(const Indices & variablesType)
{
  copyOnWrite();
  getImplementation()->setVariablesType(variablesType);
}

Indices OptimizationProblem::getVariablesType() const
{
  return getImplementation()->getVariablesType();
}

bool OptimizationProblem::isContinuous() const
{
  return getImplementation()->isContinuous();
}

/* String converter */
String OptimizationProblem::__repr__() const
{
  OSS oss(true);
  oss << "class=" << OptimizationProblem::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String OptimizationProblem::__str__(const String & ) const
{
  return __repr__();
}

END_NAMESPACE_OPENTURNS
