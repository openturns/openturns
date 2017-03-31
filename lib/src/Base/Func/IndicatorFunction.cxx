//                                               -*- C++ -*-
/**
 *  @brief The class that implements indicator functions.
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
#include "openturns/IndicatorFunction.hxx"
#include "openturns/IndicatorNumericalMathEvaluationImplementation.hxx"
#include "openturns/NoNumericalMathGradientImplementation.hxx"
#include "openturns/NoNumericalMathHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicatorFunction);

/* Default constructor */
IndicatorFunction::IndicatorFunction ()
  : NumericalMathFunction()
{
}


/* Parameter constructor */
IndicatorFunction::IndicatorFunction (const NumericalMathFunction & function,
                                      const ComparisonOperator & comparisonOperator,
                                      const NumericalScalar threshold)
  : NumericalMathFunction()
{
  const IndicatorNumericalMathEvaluationImplementation evaluation(function.getEvaluation(), comparisonOperator, threshold);
  setEvaluation(evaluation.clone());
  setGradient(NoNumericalMathGradientImplementation().clone());
  setHessian(NoNumericalMathHessianImplementation().clone());
}


/* Comparison operator */
Bool IndicatorFunction::operator ==(const IndicatorFunction & other) const
{
  return true;
}

/* String converter */
String IndicatorFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String IndicatorFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
