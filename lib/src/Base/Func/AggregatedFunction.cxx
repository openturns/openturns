//                                               -*- C++ -*-
/**
 *  @brief The class that implements aggregated functions.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/AggregatedFunction.hxx"
#include "openturns/AggregatedEvaluation.hxx"
#include "openturns/AggregatedGradient.hxx"
#include "openturns/AggregatedHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AggregatedFunction)

/* Default constructor */
AggregatedFunction::AggregatedFunction ()
  : Function()
{
}


/* Parameter constructor */
AggregatedFunction::AggregatedFunction (const FunctionCollection & functionCollection)
  : Function()
{
  const Pointer<AggregatedEvaluation> p_evaluation(new AggregatedEvaluation(functionCollection));
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new AggregatedGradient(p_evaluation));
  setHessian(new AggregatedHessian(p_evaluation));
}


/* Comparison operator */
Bool AggregatedFunction::operator ==(const AggregatedFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
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
