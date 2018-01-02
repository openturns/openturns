//                                               -*- C++ -*-
/**
 *  @brief The class that implements linear combination of vectors.
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/DualLinearCombinationEvaluation.hxx"
#include "openturns/DualLinearCombinationGradient.hxx"
#include "openturns/DualLinearCombinationHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DualLinearCombinationFunction)

/* Default constructor */
DualLinearCombinationFunction::DualLinearCombinationFunction ()
  : Function()
{
}


/* Parameter constructor */
DualLinearCombinationFunction::DualLinearCombinationFunction (const FunctionCollection & functionCollection,
    const Sample & coefficients)
  : Function()
{
  const DualLinearCombinationEvaluation evaluation(functionCollection, coefficients);
  setEvaluation(evaluation.clone());
  setGradient(new DualLinearCombinationGradient(evaluation));
  setHessian(new DualLinearCombinationHessian(evaluation));
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
