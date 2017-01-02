//                                               -*- C++ -*-
/**
 *  @brief First order polynomial response surface by Taylor expansion
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
#include "openturns/LinearTaylor.hxx"
#include "openturns/LinearNumericalMathEvaluationImplementation.hxx"
#include "openturns/ConstantNumericalMathGradientImplementation.hxx"
#include "openturns/ConstantNumericalMathHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(LinearTaylor);

LinearTaylor::LinearTaylor()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with parameters */
LinearTaylor::LinearTaylor(const NumericalPoint & center,
                           const NumericalMathFunction & inputFunction)
  : PersistentObject(),
    center_(center),
    inputFunction_(inputFunction)
{
  // Nothing to do
}

/* Virtual constructor */
LinearTaylor * LinearTaylor::clone() const
{
  return new LinearTaylor(*this);
}

/* String converter */
String LinearTaylor::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " center=" << center_
      << " function=" << inputFunction_
      << " responseSurface=" << responseSurface_
      << " constant=" << constant_
      << " linear=" << linear_;
  return oss;
}

/* Response surface computation */
void LinearTaylor::run()
{
  /* Compute the three first terms of the Taylor expansion */
  constant_ = inputFunction_(center_);
  linear_ = inputFunction_.gradient(center_);
  /* Build the several implementations and set it into the response surface */
  responseSurface_.setEvaluation(new LinearNumericalMathEvaluationImplementation(center_, constant_, linear_));
  responseSurface_.setGradient(new ConstantNumericalMathGradientImplementation(linear_));
  responseSurface_.setHessian(new ConstantNumericalMathHessianImplementation(SymmetricTensor(center_.getDimension(), constant_.getDimension())));
  responseSurface_.setDescription(inputFunction_.getDescription());
}

/* Center accessor */
NumericalPoint LinearTaylor::getCenter() const
{
  return center_;
}

/* Constant accessor */
NumericalPoint LinearTaylor::getConstant() const
{
  return constant_;
}

/* Linear accessor */
Matrix LinearTaylor::getLinear() const
{
  return linear_;
}

/* Function accessor */
NumericalMathFunction LinearTaylor::getInputFunction() const
{
  return inputFunction_;
}

/* Response surface accessor */
NumericalMathFunction LinearTaylor::getResponseSurface() const
{
  return responseSurface_;
}

END_NAMESPACE_OPENTURNS
