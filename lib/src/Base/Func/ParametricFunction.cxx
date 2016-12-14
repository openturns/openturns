//                                               -*- C++ -*-
/**
 *  @brief The class that implements parametric functions.
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ParametricFunction.hxx"
#include "openturns/ParametricEvaluationImplementation.hxx"
#include "openturns/ParametricGradientImplementation.hxx"
#include "openturns/ParametricHessianImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricFunction);

/* Default constructor */
ParametricFunction::ParametricFunction ()
  : NumericalMathFunction()
{
}


/* Parameter constructor */
ParametricFunction::ParametricFunction (const NumericalMathFunction & function,
                                        const Indices & set,
                                        const NumericalPoint & referencePoint,
                                        const Bool parametersSet)
  : NumericalMathFunction()
{
  const Pointer<ParametricEvaluationImplementation> p_evaluation = new ParametricEvaluationImplementation(function, set, referencePoint, parametersSet);
  setEvaluation(p_evaluation);
  setGradient(new ParametricGradientImplementation(p_evaluation));
  setHessian(new ParametricHessianImplementation(p_evaluation));
}


/* Comparison operator */
Bool ParametricFunction::operator ==(const ParametricFunction & other) const
{
  return true;
}

/* String converter */
String ParametricFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String ParametricFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
