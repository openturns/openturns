//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math hessian implementations
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NoHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NoHessian)

static const Factory<NoHessian> Factory_NoHessian;

/* Default constructor */
NoHessian::NoHessian()
  : HessianImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NoHessian * NoHessian::clone() const
{
  return new NoHessian(*this);
}

/* Comparison operator */
Bool NoHessian::operator ==(const NoHessian & ) const
{
  return true;
}

/* String converter */
String NoHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << NoHessian::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Test for actual implementation */
Bool NoHessian::isActualImplementation() const
{
  return false;
}



/* Here is the interface that all derived class must implement */

/* Hessian method */
SymmetricTensor NoHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  return SymmetricTensor();
}

/* Accessor for input point dimension */
UnsignedInteger NoHessian::getInputDimension() const
{
  return 0;
}

/* Accessor for output point dimension */
UnsignedInteger NoHessian::getOutputDimension() const
{
  return 0;
}

END_NAMESPACE_OPENTURNS
