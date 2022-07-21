//                                               -*- C++ -*-
/**
 *  @brief Class for a constant numerical math hessian implementation
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
#include "openturns/ConstantHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantHessian)

static const Factory<ConstantHessian> Factory_ConstantHessian;


/* Default constructor */
ConstantHessian::ConstantHessian()
  : HessianImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
ConstantHessian::ConstantHessian(const SymmetricTensor & constant)
  : HessianImplementation()
  , constant_(constant)
{
  /* Check if the dimensions of the constant term is compatible with the linear term */
}

/* Virtual constructor */
ConstantHessian * ConstantHessian::clone() const
{
  return new ConstantHessian(*this);
}

/* Comparison operator */
Bool ConstantHessian::operator ==(const ConstantHessian & other) const
{
  return (constant_ == other.constant_);
}

/* String converter */
String ConstantHessian::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConstantHessian::GetClassName()
      << " name=" << getName()
      << " constant=" << constant_;
  return oss;
}

/* String converter */
String ConstantHessian::__str__(const String & offset) const
{
  OSS oss(false);
  const UnsignedInteger inputDimension = getInputDimension();
  const Description description(Description::BuildDefault(inputDimension, "x"));
  if (hasVisibleName()) oss << getName() << ":" << Os::GetEndOfLine() << offset;
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    if (i == 0) oss << "  (";
    oss << description[i];
    if (i == inputDimension - 1) oss << ")";
    else oss << ", ";
  }
  oss << " ->" << Os::GetEndOfLine() << offset;
  oss << constant_.__str__(offset + "  ");
  return oss;
}

/* Accessor for the constant term */
SymmetricTensor ConstantHessian::getConstant() const
{
  return constant_;
}

/* Here is the interface that all derived class must implement */

/* Hessian () */
SymmetricTensor ConstantHessian::hessian(const Point & inP) const
{
  if (inP.getDimension() != constant_.getNbRows()) throw InvalidArgumentException(HERE) << "Invalid input dimension " << inP.getDimension() << ", expected " << constant_.getNbRows();
  callsNumber_.increment();
  return constant_;
}

/* Accessor for input point dimension */
UnsignedInteger ConstantHessian::getInputDimension() const
{
  return constant_.getNbRows();
}

/* Accessor for output point dimension */
UnsignedInteger ConstantHessian::getOutputDimension() const
{
  return constant_.getNbSheets();
}

/* Method save() stores the object through the StorageManager */
void ConstantHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "constant_", constant_ );
}

/* Method load() reloads the object from the StorageManager */
void ConstantHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute( "constant_", constant_ );
}

END_NAMESPACE_OPENTURNS
