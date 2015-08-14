//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "LinearNumericalMathFunction.hxx"
#include "LinearNumericalMathEvaluationImplementation.hxx"
#include "ConstantNumericalMathGradientImplementation.hxx"
#include "ConstantNumericalMathHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearNumericalMathFunction);

/* Default constructor */
LinearNumericalMathFunction::LinearNumericalMathFunction(const NumericalPoint & center,
    const NumericalPoint & constant,
    const Matrix & linear)
  : NumericalMathFunction(new LinearNumericalMathEvaluationImplementation(center, constant, linear.transpose()),
                          new ConstantNumericalMathGradientImplementation(linear.transpose()),
                          new ConstantNumericalMathHessianImplementation(SymmetricTensor(center.getDimension(), constant.getDimension())))
{
  // Nothing to do
}

/* Comparison operator */
Bool LinearNumericalMathFunction::operator ==(const LinearNumericalMathFunction & other) const
{
  return true;
}

/* String converter */
String LinearNumericalMathFunction::__repr__() const
{
  OSS oss(true);
  oss << "class=" << LinearNumericalMathFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String LinearNumericalMathFunction::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << LinearNumericalMathFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__str__();
  return oss;
}

END_NAMESPACE_OPENTURNS
