//                                               -*- C++ -*-
/**
 *  @brief NearestPointProblem allows to describe an optimization problem
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/NearestPointProblem.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/QuadraticFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NearestPointProblem)

static const Factory<NearestPointProblem> Factory_NearestPointProblem;

/* Default constructor */
NearestPointProblem::NearestPointProblem()
  : OptimizationProblemImplementation()
  , levelValue_(0.0)
{
  // Nothing to do
}

/* Constructor for nearest point problem */
NearestPointProblem::NearestPointProblem(const Function & levelFunction,
    Scalar levelValue)
  : OptimizationProblemImplementation()
  , levelValue_(levelValue)
{
  setLevelFunction(levelFunction);
}

/* Virtual constructor */
NearestPointProblem * NearestPointProblem::clone() const
{
  return new NearestPointProblem(*this);
}


/* Level function accessor */
Function NearestPointProblem::getLevelFunction() const
{
  return levelFunction_;
}

void NearestPointProblem::setLevelFunction(const Function & levelFunction)
{
  if (levelFunction.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: level function has an output dimension=" << levelFunction.getOutputDimension() << " but only dimension 1 is supported.";

  levelFunction_ = levelFunction;
  dimension_ = levelFunction_.getInputDimension();
  // Update objective function
  const Point center(dimension_);
  const Point constant(1);
  const Matrix linear(dimension_, 1);
  const IdentityMatrix identity(dimension_);
  const SymmetricTensor quadratic(dimension_, 1, *(identity.getImplementation().get()));
  objective_ = QuadraticFunction(center, constant, linear, quadratic);
  setNearestPointConstraints();
  setVariablesType(Indices(dimension_, CONTINUOUS));
}

Bool NearestPointProblem::hasLevelFunction() const
{
  return true;
}

/* Level value accessor */
Scalar NearestPointProblem::getLevelValue() const
{
  return levelValue_;
}

void NearestPointProblem::setLevelValue(Scalar levelValue)
{
  levelValue_ = levelValue;
  if (levelFunction_.getEvaluation().getImplementation()->isActualImplementation()) setNearestPointConstraints();
}

void NearestPointProblem::setNearestPointConstraints()
{
  const Point center(dimension_);
  const Matrix linear(dimension_, 1);
  LinearFunction constantFunction(center, Point(1, levelValue_), linear.transpose());
  Function equalityConstraint(levelFunction_);
  equalityConstraint_ = equalityConstraint.operator - (constantFunction);
  inequalityConstraint_ = Function();
}

void NearestPointProblem::clearLevelFunction()
{
  if (levelFunction_.getEvaluation().getImplementation()->isActualImplementation())
  {
    LOGWARN(OSS() << "Clearing level function");
    levelFunction_ = Function();
  }
  levelValue_ = 0.0;
}


/* String converter */
String NearestPointProblem::__repr__() const
{
  OSS oss;
  oss << "class=" << NearestPointProblem::GetClassName();
  oss << " level function=" << levelFunction_.__repr__()
      << " level value=" << levelValue_
      << " dimension=" << dimension_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void NearestPointProblem::save(Advocate & adv) const
{
  OptimizationProblemImplementation::save(adv);
  adv.saveAttribute( "levelFunction_", levelFunction_ );
  adv.saveAttribute( "levelValue_", levelValue_ );
}

/* Method load() reloads the object from the StorageManager */
void NearestPointProblem::load(Advocate & adv)
{
  OptimizationProblemImplementation::load(adv);
  adv.loadAttribute( "levelFunction_", levelFunction_ );
  adv.loadAttribute( "levelValue_", levelValue_ );
}

END_NAMESPACE_OPENTURNS
