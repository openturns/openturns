//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math gradient implementations
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
#include "openturns/NoNumericalMathGradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NoNumericalMathGradientImplementation);

static const Factory<NoNumericalMathGradientImplementation> RegisteredFactory;

/* Default constructor */
NoNumericalMathGradientImplementation::NoNumericalMathGradientImplementation()
  : NumericalMathGradientImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NoNumericalMathGradientImplementation * NoNumericalMathGradientImplementation::clone() const
{
  return new NoNumericalMathGradientImplementation(*this);
}

/* Comparison operator */
Bool NoNumericalMathGradientImplementation::operator ==(const NoNumericalMathGradientImplementation & other) const
{
  return true;
}

/* String converter */
String NoNumericalMathGradientImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << NoNumericalMathGradientImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Test for actual implementation */
Bool NoNumericalMathGradientImplementation::isActualImplementation() const
{
  return false;
}







/* Here is the interface that all derived class must implement */

/* Gradient method */
Matrix NoNumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  return Matrix();
}

/* Accessor for input point dimension */
UnsignedInteger NoNumericalMathGradientImplementation::getInputDimension() const
{
  return 0;
}

/* Accessor for output point dimension */
UnsignedInteger NoNumericalMathGradientImplementation::getOutputDimension() const
{
  return 0;
}

END_NAMESPACE_OPENTURNS
