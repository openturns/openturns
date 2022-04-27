//                                               -*- C++ -*-
/**
 *  @brief The class that implements parametric functions.
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ParametricFunction.hxx"
#include "openturns/ParametricEvaluation.hxx"
#include "openturns/ParametricGradient.hxx"
#include "openturns/ParametricHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParametricFunction)

/* Default constructor */
ParametricFunction::ParametricFunction ()
  : Function()
{
}


/* Parameter constructor */
ParametricFunction::ParametricFunction (const Function & function,
                                        const Indices & set,
                                        const Point & referencePoint,
                                        const Bool parametersSet)
  : Function()
{
  const Pointer<ParametricEvaluation> p_evaluation = new ParametricEvaluation(function, set, referencePoint, parametersSet);
  setEvaluation(Evaluation(p_evaluation));
  setGradient(new ParametricGradient(p_evaluation));
  setHessian(new ParametricHessian(p_evaluation));
}


/* Comparison operator */
Bool ParametricFunction::operator ==(const ParametricFunction & other) const
{
  if (this == &other) return true;
  return getEvaluation() == other.getEvaluation();
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
