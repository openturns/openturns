//                                               -*- C++ -*-
/**
 *  @brief Null hessian
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
#include "openturns/NullHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NullHessian)

static const Factory<NullHessian> Factory_NullHessian;

/* Default constructor */
NullHessian::NullHessian()
  : HessianImplementation()
{
  // Nothing to do
}

NullHessian::NullHessian(const UnsignedInteger inputDimension, const UnsignedInteger outputDimension)
  : HessianImplementation()
  , inputDimension_(inputDimension)
  , outputDimension_(outputDimension)
{
  // Nothing to do
}

/* Virtual constructor */
NullHessian * NullHessian::clone() const
{
  return new NullHessian(*this);
}

/* Comparison operator */
Bool NullHessian::operator ==(const NullHessian & other) const
{
  if (this == &other) return true;
  return (inputDimension_ == other.inputDimension_) && (outputDimension_ == other.outputDimension_);
}

/* String converter */
String NullHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << NullHessian::GetClassName()
      << " name=" << getName()
      << " inputDimension=" << inputDimension_
      << " outputDimension=" << outputDimension_;
  return oss;
}

/* Here is the interface that all derived class must implement */

/* Hessian method */
SymmetricTensor NullHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  return SymmetricTensor(inputDimension_, outputDimension_);
}

/* Accessor for input point dimension */
UnsignedInteger NullHessian::getInputDimension() const
{
  return inputDimension_;
}

/* Accessor for output point dimension */
UnsignedInteger NullHessian::getOutputDimension() const
{
  return outputDimension_;
}

/* Method save() stores the object through the StorageManager */
void NullHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute("inputDimension_", inputDimension_);
  adv.saveAttribute("outputDimension_", outputDimension_);
}

/* Method load() reloads the object from the StorageManager */
void NullHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute("inputDimension_", inputDimension_);
  adv.loadAttribute("outputDimension_", outputDimension_);
}

END_NAMESPACE_OPENTURNS
