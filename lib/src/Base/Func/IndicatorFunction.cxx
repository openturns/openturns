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
#include "openturns/IndicatorEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicatorFunction);

/* Default constructor */
IndicatorFunction::IndicatorFunction ()
  : Function()
{
}


/* Parameter constructor */
IndicatorFunction::IndicatorFunction (const Function & function,
                                      const ComparisonOperator & comparisonOperator,
                                      const Scalar threshold)
  : Function()
{
  const IndicatorEvaluation evaluation(function.getEvaluation(), comparisonOperator, threshold);
  setEvaluation(evaluation.clone());
  setGradient(NoGradient().clone());
  setHessian(NoHessian().clone());
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
