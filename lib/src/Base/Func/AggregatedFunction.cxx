//                                               -*- C++ -*-
/**
 *  @brief The class that implements aggregated functions.
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
#include "openturns/AggregatedFunction.hxx"
#include "openturns/AggregatedNumericalMathEvaluationImplementation.hxx"
#include "openturns/AggregatedNumericalMathGradientImplementation.hxx"
#include "openturns/AggregatedNumericalMathHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AggregatedFunction);

/* Default constructor */
AggregatedFunction::AggregatedFunction ()
  : NumericalMathFunction()
{
}


/* Parameter constructor */
AggregatedFunction::AggregatedFunction (const NumericalMathFunctionCollection & functionCollection)
  : NumericalMathFunction()
{
  const AggregatedNumericalMathEvaluationImplementation evaluation(functionCollection);
  setEvaluation(evaluation.clone());
  setGradient(new AggregatedNumericalMathGradientImplementation(evaluation));
  setHessian(new AggregatedNumericalMathHessianImplementation(evaluation));
}


/* Comparison operator */
Bool AggregatedFunction::operator ==(const AggregatedFunction & other) const
{
  return true;
}

/* String converter */
String AggregatedFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String AggregatedFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
