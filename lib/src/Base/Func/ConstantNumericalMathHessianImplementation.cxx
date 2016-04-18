//                                               -*- C++ -*-
/**
 *  @brief Class for a constant numerical math hessian implementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ConstantNumericalMathHessianImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantNumericalMathHessianImplementation);

static const Factory<ConstantNumericalMathHessianImplementation> RegisteredFactory;


/* Default constructor */
ConstantNumericalMathHessianImplementation::ConstantNumericalMathHessianImplementation()
  : NumericalMathHessianImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
ConstantNumericalMathHessianImplementation::ConstantNumericalMathHessianImplementation(const SymmetricTensor & constant)
  : NumericalMathHessianImplementation()
  , constant_(constant)
{
  /* Check if the dimensions of the constant term is compatible with the linear term */
}

/* Virtual constructor */
ConstantNumericalMathHessianImplementation * ConstantNumericalMathHessianImplementation::clone() const
{
  return new ConstantNumericalMathHessianImplementation(*this);
}

/* Comparison operator */
Bool ConstantNumericalMathHessianImplementation::operator ==(const ConstantNumericalMathHessianImplementation & other) const
{
  return (constant_ == other.constant_);
}

/* String converter */
String ConstantNumericalMathHessianImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConstantNumericalMathHessianImplementation::GetClassName()
      << " name=" << getName()
      << " constant=" << constant_;
  return oss;
}

/* String converter */
String ConstantNumericalMathHessianImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  const UnsignedInteger inputDimension = getInputDimension();
  const Description description(Description::BuildDefault(inputDimension, "x"));
  if (hasVisibleName()) oss << offset << getName() << ":" << Os::GetEndOfLine();
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    if (i == 0) oss << offset << "  (";
    oss << description[i];
    if (i == inputDimension - 1) oss << ")";
    else oss << ", ";
  }
  oss << " ->" << Os::GetEndOfLine();
  oss << constant_.__str__(offset + "  ");
  return oss;
}

/* Accessor for the constant term */
SymmetricTensor ConstantNumericalMathHessianImplementation::getConstant() const
{
  return constant_;
}

/* Here is the interface that all derived class must implement */

/* Hessian () */
SymmetricTensor ConstantNumericalMathHessianImplementation::hessian(const NumericalPoint & inP) const
{
  if (inP.getDimension() != constant_.getNbRows()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  ++callsNumber_;
  return constant_;
}

/* Accessor for input point dimension */
UnsignedInteger ConstantNumericalMathHessianImplementation::getInputDimension() const
{
  return constant_.getNbRows();
}

/* Accessor for output point dimension */
UnsignedInteger ConstantNumericalMathHessianImplementation::getOutputDimension() const
{
  return constant_.getNbSheets();
}

/* Method save() stores the object through the StorageManager */
void ConstantNumericalMathHessianImplementation::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save(adv);
  adv.saveAttribute( "constant_", constant_ );
}

/* Method load() reloads the object from the StorageManager */
void ConstantNumericalMathHessianImplementation::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load(adv);
  adv.loadAttribute( "constant_", constant_ );
}

END_NAMESPACE_OPENTURNS
