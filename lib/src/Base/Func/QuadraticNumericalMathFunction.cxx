//                                               -*- C++ -*-
/**
 *  @brief The class that implements quadratic numerical math functions.
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
#include "openturns/QuadraticNumericalMathFunction.hxx"
#include "openturns/QuadraticNumericalMathEvaluationImplementation.hxx"
#include "openturns/LinearNumericalMathGradientImplementation.hxx"
#include "openturns/ConstantNumericalMathHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuadraticNumericalMathFunction);

/* Default constructor */
QuadraticNumericalMathFunction::QuadraticNumericalMathFunction (const NumericalPoint & center,
    const NumericalPoint & constant,
    const Matrix & linear,
    const SymmetricTensor & quadratic)
  : NumericalMathFunction(new QuadraticNumericalMathEvaluationImplementation(center, constant, linear, quadratic),
                          new LinearNumericalMathGradientImplementation(center, linear, quadratic),
                          new ConstantNumericalMathHessianImplementation(quadratic))
{
  // Nothing to do
}

/* Comparison operator */
Bool QuadraticNumericalMathFunction::operator ==(const QuadraticNumericalMathFunction & other) const
{
  return true;
}

/* String converter */
String QuadraticNumericalMathFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << QuadraticNumericalMathFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String QuadraticNumericalMathFunction::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << QuadraticNumericalMathFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__str__();
  return oss;
}

END_NAMESPACE_OPENTURNS
