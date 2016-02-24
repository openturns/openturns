//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/RootStrategyImplementation.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RootStrategyImplementation
 */

CLASSNAMEINIT(RootStrategyImplementation);

static const Factory<RootStrategyImplementation> RegisteredFactory;

/* Default constructor */
RootStrategyImplementation::RootStrategyImplementation()
  : PersistentObject()
  , solver_(new Brent())
  , maximumDistance_(ResourceMap::GetAsNumericalScalar( "RootStrategyImplementation-DefaultMaximumDistance" ))
  , stepSize_(ResourceMap::GetAsNumericalScalar( "RootStrategyImplementation-DefaultStepSize" ))
  , isAlreadyComputedOriginValue_(false)
  , originValue_(0.0)
{
  // Nothing to do
}

/* Parameters constructor */
RootStrategyImplementation::RootStrategyImplementation(const Solver & solver)
  : PersistentObject()
  , solver_(solver)
  , maximumDistance_(ResourceMap::GetAsNumericalScalar( "RootStrategyImplementation-DefaultMaximumDistance" ))
  , stepSize_(ResourceMap::GetAsNumericalScalar( "RootStrategyImplementation-DefaultStepSize" ))
  , isAlreadyComputedOriginValue_(false)
  , originValue_(0.0)
{
  // Nothing to do
}

/* Parameters constructor */
RootStrategyImplementation::RootStrategyImplementation(const Solver & solver,
    const NumericalScalar maximumDistance,
    const NumericalScalar stepSize)
  : PersistentObject()
  , solver_(solver)
  , maximumDistance_(maximumDistance)
  , stepSize_(stepSize)
  , isAlreadyComputedOriginValue_(false)
  , originValue_(0.0)
{
  // Nothing to do
}

/* Virtual constructor */
RootStrategyImplementation * RootStrategyImplementation::clone() const
{
  return new RootStrategyImplementation(*this);
}

/* Solve gives all the roots found applying the root strategy */
RootStrategyImplementation::NumericalScalarCollection RootStrategyImplementation::solve(const NumericalMathFunction & function,
    const NumericalScalar value)
{
  throw NotYetImplementedException(HERE) << "In RootStrategyImplementation::solve(const NumericalMathFunction & function, const NumericalScalar value)";
}

/* Solver accessor */
void RootStrategyImplementation::setSolver(const Solver & solver)
{
  solver_ = solver;
}

Solver RootStrategyImplementation::getSolver() const
{
  return solver_;
}

/* Maximum distance accessor */
void RootStrategyImplementation::setMaximumDistance(const NumericalScalar maximumDistance)
{
  maximumDistance_ = maximumDistance;
}

NumericalScalar RootStrategyImplementation::getMaximumDistance() const
{
  return maximumDistance_;
}

/* Step size accessor */
void RootStrategyImplementation::setStepSize(const NumericalScalar stepSize)
{
  stepSize_ = stepSize;
}

NumericalScalar RootStrategyImplementation::getStepSize() const
{
  return stepSize_;
}

/* Value of the performance function at the origin accessor */
void RootStrategyImplementation::setOriginValue(const NumericalScalar originValue)
{
  originValue_ = originValue;
  isAlreadyComputedOriginValue_ = true;
}

/* We have to store the value of the performance function at the origin for two reasons:
   + to save computations, as this value will be reused for each direction
   + to check if the origin is in the failure space or not, wich change the meaning of the roots
*/
NumericalScalar RootStrategyImplementation::getOriginValue() const
{
  if (!isAlreadyComputedOriginValue_) throw NotDefinedException(HERE);
  return originValue_;
}

/* String converter */
String RootStrategyImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << RootStrategyImplementation::GetClassName()
      << " solver=" << solver_
      << " maximumDistance=" << maximumDistance_
      << " stepSize=" << stepSize_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RootStrategyImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("solver_", solver_);
  adv.saveAttribute("maximumDistance_", maximumDistance_);
  adv.saveAttribute("stepSize_", stepSize_);
}

/* Method load() reloads the object from the StorageManager */
void RootStrategyImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("solver_", solver_);
  adv.loadAttribute("maximumDistance_", maximumDistance_);
  adv.loadAttribute("stepSize_", stepSize_);
}

END_NAMESPACE_OPENTURNS
