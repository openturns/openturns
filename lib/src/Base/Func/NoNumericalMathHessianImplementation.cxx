//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math hessian implementations
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
#include "openturns/NoNumericalMathHessianImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NoNumericalMathHessianImplementation);

static const Factory<NoNumericalMathHessianImplementation> Factory_NoNumericalMathHessianImplementation;

/* Default constructor */
NoNumericalMathHessianImplementation::NoNumericalMathHessianImplementation()
  : NumericalMathHessianImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NoNumericalMathHessianImplementation * NoNumericalMathHessianImplementation::clone() const
{
  return new NoNumericalMathHessianImplementation(*this);
}

/* Comparison operator */
Bool NoNumericalMathHessianImplementation::operator ==(const NoNumericalMathHessianImplementation & other) const
{
  return true;
}

/* String converter */
String NoNumericalMathHessianImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << NoNumericalMathHessianImplementation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Test for actual implementation */
Bool NoNumericalMathHessianImplementation::isActualImplementation() const
{
  return false;
}









/* Here is the interface that all derived class must implement */

/* Hessian method */
SymmetricTensor NoNumericalMathHessianImplementation::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  return SymmetricTensor();
}

/* Accessor for input point dimension */
UnsignedInteger NoNumericalMathHessianImplementation::getInputDimension() const
{
  return 0;
}

/* Accessor for output point dimension */
UnsignedInteger NoNumericalMathHessianImplementation::getOutputDimension() const
{
  return 0;
}

END_NAMESPACE_OPENTURNS
