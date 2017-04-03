//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all linear functions
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/LinearFunction.hxx"
#include "openturns/LinearEvaluation.hxx"
#include "openturns/ConstantGradient.hxx"
#include "openturns/ConstantHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearFunction);

/* Default constructor */
LinearFunction::LinearFunction(const NumericalPoint & center,
    const NumericalPoint & constant,
    const Matrix & linear)
  : NumericalMathFunction(new LinearEvaluation(center, constant, linear.transpose()),
                          new ConstantGradient(linear.transpose()),
                          new ConstantHessian(SymmetricTensor(center.getDimension(), constant.getDimension())))
{
  // Nothing to do
}

/* Comparison operator */
Bool LinearFunction::operator ==(const LinearFunction & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String LinearFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LinearFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String LinearFunction::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << LinearFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__str__();
  return oss;
}

END_NAMESPACE_OPENTURNS
