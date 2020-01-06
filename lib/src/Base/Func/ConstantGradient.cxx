//                                               -*- C++ -*-
/**
 *  @brief Class for a constant numerical math gradient implementation
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
#include "openturns/ConstantGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantGradient)

static const Factory<ConstantGradient> Factory_ConstantGradient;


/* Default constructor */
ConstantGradient::ConstantGradient()
  : GradientImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
ConstantGradient::ConstantGradient(const Matrix & constant)
  : GradientImplementation()
  , constant_(constant)
{
  /* Check if the dimensions of the constant term is compatible with the linear term */
}

/* Virtual constructor */
ConstantGradient * ConstantGradient::clone() const
{
  return new ConstantGradient(*this);
}

/* Comparison operator */
Bool ConstantGradient::operator ==(const ConstantGradient & other) const
{
  return (constant_ == other.constant_);
}

/* String converter */
String ConstantGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConstantGradient::GetClassName()
      << " name=" << getName()
      << " constant=" << constant_;
  return oss;
}

/* String converter */
String ConstantGradient::__str__(const String & offset) const
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
  oss << " ->" << Os::GetEndOfLine() << offset << "  ";
  oss << constant_.__str__(offset + "  ");
  return oss;
}

/* Accessor for the constant term */
Matrix ConstantGradient::getConstant() const
{
  return constant_;
}

/* Here is the interface that all derived class must implement */

/* Operator () */
Matrix ConstantGradient::gradient(const Point & inP) const
{
  if (inP.getDimension() != constant_.getNbRows()) throw InvalidArgumentException(HERE) << "Invalid input dimension";
  callsNumber_.increment();
  return constant_;
}

/* Accessor for input point dimension */
UnsignedInteger ConstantGradient::getInputDimension() const
{
  return constant_.getNbRows();
}

/* Accessor for output point dimension */
UnsignedInteger ConstantGradient::getOutputDimension() const
{
  return constant_.getNbColumns();
}

/* Method save() stores the object through the StorageManager */
void ConstantGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "constant_", constant_ );
}

/* Method load() reloads the object from the StorageManager */
void ConstantGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "constant_", constant_ );
}

END_NAMESPACE_OPENTURNS
