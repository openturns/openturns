//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the constant function
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ConstantFunction.hxx"
#include "openturns/ConstantEvaluation.hxx"
#include "openturns/ConstantGradient.hxx"
#include "openturns/NullHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ConstantFunction)

/* Default constructor */
ConstantFunction::ConstantFunction(const UnsignedInteger inputDimension, const Point & constant)
  : Function(new ConstantEvaluation(inputDimension, constant),
             new ConstantGradient(Matrix(inputDimension, constant.getDimension())),
             new NullHessian(inputDimension, constant.getDimension()))
{
  // Nothing to do
}

/* Comparison operator */
Bool ConstantFunction::operator ==(const ConstantFunction & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String ConstantFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ConstantFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String ConstantFunction::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << ConstantFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__str__();
  return oss;
}

END_NAMESPACE_OPENTURNS
