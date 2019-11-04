//                                               -*- C++ -*-
/**
 *  @brief The class that implements linear combination of vectors.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
  const Pointer<DualLinearCombinationEvaluation> p_evaluation = new DualLinearCombinationEvaluation(functionCollection, coefficients);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new DualLinearCombinationGradient(p_evaluation));
  setHessian(new DualLinearCombinationHessian(p_evaluation));
}


/* Comparison operator */
Bool DualLinearCombinationFunction::operator ==(const DualLinearCombinationFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
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
