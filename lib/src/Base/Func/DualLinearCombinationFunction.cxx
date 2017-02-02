//                                               -*- C++ -*-
/**
 *  @brief The class that implements linear combination of vectors.
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
#include "openturns/DualLinearCombinationFunction.hxx"
#include "openturns/DualLinearCombinationEvaluationImplementation.hxx"
#include "openturns/DualLinearCombinationGradientImplementation.hxx"
#include "openturns/DualLinearCombinationHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DualLinearCombinationFunction);

/* Default constructor */
DualLinearCombinationFunction::DualLinearCombinationFunction ()
  : NumericalMathFunction()
{
}


/* Parameter constructor */
DualLinearCombinationFunction::DualLinearCombinationFunction (const NumericalMathFunctionCollection & functionCollection,
                                                              const NumericalSample & coefficients)
  : NumericalMathFunction()
{
  const DualLinearCombinationEvaluationImplementation evaluation(functionCollection, coefficients);
  setEvaluation(evaluation.clone());
  setGradient(new DualLinearCombinationGradientImplementation(evaluation));
  setHessian(new DualLinearCombinationHessianImplementation(evaluation));
}


/* Comparison operator */
Bool DualLinearCombinationFunction::operator ==(const DualLinearCombinationFunction & other) const
{
  return true;
}

/* String converter */
String DualLinearCombinationFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String DualLinearCombinationFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
