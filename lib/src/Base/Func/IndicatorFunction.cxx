//                                               -*- C++ -*-
/**
 *  @brief The class that implements indicator functions.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IndicatorFunction.hxx"
#include "openturns/IndicatorEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicatorFunction)

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
  const Pointer<IndicatorEvaluation> p_evaluation = new IndicatorEvaluation(function.getEvaluation(), comparisonOperator, threshold);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(NoGradient().clone());
  setHessian(NoHessian().clone());
}


/* Comparison operator */
Bool IndicatorFunction::operator ==(const IndicatorFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
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
