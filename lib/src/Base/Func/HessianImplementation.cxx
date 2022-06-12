//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all hessian implementations
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HessianImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MarginalHessian.hxx"
#include "openturns/ComposedHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HessianImplementation)

static const Factory<HessianImplementation> Factory_HessianImplementation;

/* Default constructor */
HessianImplementation::HessianImplementation()
  : PersistentObject()
  , callsNumber_(0)
{
  // Nothing to do
}

/* Virtual constructor */
HessianImplementation * HessianImplementation::clone() const
{
  return new HessianImplementation(*this);
}

/* Comparison operator */
Bool HessianImplementation::operator ==(const HessianImplementation & ) const
{
  return true;
}

/* String converter */
String HessianImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << HessianImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* String converter */
String HessianImplementation::__str__(const String & ) const
{
  return OSS(false) << "HessianImplementation";
}

/* Test for actual implementation */
Bool HessianImplementation::isActualImplementation() const
{
  return true;
}

/* Here is the interface that all derived class must implement */

/* Hessian method */
SymmetricTensor HessianImplementation::hessian(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In HessianImplementation::hessian(const Point & inP) const";
}

/* Accessor for input point dimension */
UnsignedInteger HessianImplementation::getInputDimension() const
{
  throw NotYetImplementedException(HERE) << "In HessianImplementation::getInputDimension() const";
}

/* Accessor for output point dimension */
UnsignedInteger HessianImplementation::getOutputDimension() const
{
  throw NotYetImplementedException(HERE) << "In HessianImplementation::getOutputDimension() const";
}

/* Get the number of calls to operator() */
UnsignedInteger HessianImplementation::getCallsNumber() const
{
  return callsNumber_.get();
}

/* Parameters value and description accessor */
Point HessianImplementation::getParameter() const
{
  return parameter_;
}

void HessianImplementation::setParameter(const Point & parameter)
{
  parameter_ = parameter;
}

/* Get the i-th marginal function */
Hessian HessianImplementation::getMarginal(const UnsignedInteger i) const
{
  if (!(i < getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1], here index=" << i << " and outputDimension=" << getOutputDimension();
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Hessian HessianImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal hessian must be in the range [0, outputDimension-1] and must be different";
  Indices full(getOutputDimension());
  full.fill();
  if (indices == full) return clone();
  return new MarginalHessian(clone(), indices);
}

/* Method save() stores the object through the StorageManager */
void HessianImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "callsNumber_", static_cast<UnsignedInteger>(callsNumber_.get()) );
  adv.saveAttribute( "parameter_", parameter_ );
}

/* Method load() reloads the object from the StorageManager */
void HessianImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  UnsignedInteger callsNumber;
  adv.loadAttribute( "callsNumber_", callsNumber );
  callsNumber_ = callsNumber;
  adv.loadAttribute( "parameter_", parameter_ );
}


END_NAMESPACE_OPENTURNS
