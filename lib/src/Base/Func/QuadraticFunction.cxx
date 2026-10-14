//                                               -*- C++ -*-
/**
 *  @brief The class that implements quadratic functions.
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/QuadraticFunction.hxx"
#include "openturns/QuadraticEvaluation.hxx"
#include "openturns/LinearGradient.hxx"
#include "openturns/ConstantHessian.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuadraticFunction)

/* Default constructor */
QuadraticFunction::QuadraticFunction (const Point & center,
                                      const Point & constant,
                                      const Matrix & linear,
                                      const SymmetricTensor & quadratic)
  : Function(new QuadraticEvaluation(center, constant, linear, quadratic),
             new LinearGradient(center, linear.transpose(), quadratic),
             new ConstantHessian(quadratic))
{
  // Nothing to do
}

/* Comparison operator */
Bool QuadraticFunction::operator ==(const QuadraticFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
}

/* Center accessor */
Point QuadraticFunction::getCenter() const
{
  const QuadraticEvaluation * p_evaluation = dynamic_cast<const QuadraticEvaluation *>(getEvaluation().getImplementation().get());
  if (!p_evaluation) throw InvalidArgumentException(HERE) << "Error: the evaluation of the QuadraticFunction is not a QuadraticEvaluation.";
  return p_evaluation->getCenter();
}

/* Constant accessor */
Point QuadraticFunction::getConstant() const
{
  const QuadraticEvaluation * p_evaluation = dynamic_cast<const QuadraticEvaluation *>(getEvaluation().getImplementation().get());
  if (!p_evaluation) throw InvalidArgumentException(HERE) << "Error: the evaluation of the QuadraticFunction is not a QuadraticEvaluation.";
  return p_evaluation->getConstant();
}

/* Linear term accessor, as given to the constructor */
Matrix QuadraticFunction::getLinear() const
{
  const QuadraticEvaluation * p_evaluation = dynamic_cast<const QuadraticEvaluation *>(getEvaluation().getImplementation().get());
  if (!p_evaluation) throw InvalidArgumentException(HERE) << "Error: the evaluation of the QuadraticFunction is not a QuadraticEvaluation.";
  return p_evaluation->getLinear();
}

/* Quadratic term accessor, as given to the constructor */
SymmetricTensor QuadraticFunction::getQuadratic() const
{
  const QuadraticEvaluation * p_evaluation = dynamic_cast<const QuadraticEvaluation *>(getEvaluation().getImplementation().get());
  if (!p_evaluation) throw InvalidArgumentException(HERE) << "Error: the evaluation of the QuadraticFunction is not a QuadraticEvaluation.";
  return p_evaluation->getQuadratic();
}

/* String converter */
String QuadraticFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << QuadraticFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String QuadraticFunction::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << QuadraticFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
