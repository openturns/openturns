//                                               -*- C++ -*-
/**
 *  @brief The class that implements analytical functions.
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
#include "openturns/AnalyticalFunction.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/AnalyticalNumericalMathEvaluationImplementation.hxx"
#include "openturns/AnalyticalNumericalMathGradientImplementation.hxx"
#include "openturns/AnalyticalNumericalMathHessianImplementation.hxx"
#endif
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AnalyticalFunction);

/* Default constructor */
AnalyticalFunction::AnalyticalFunction ()
  : NumericalMathFunction()
{
}


/* Parameter constructor */
AnalyticalFunction::AnalyticalFunction (const String & inputVariablesName,
                                        const String & formula)
  : NumericalMathFunction()
{
  *this = AnalyticalFunction(Description(1, inputVariablesName), Description(1, formula));
}


/* Parameter constructor */
AnalyticalFunction::AnalyticalFunction (const Description & inputVariablesNames,
                                        const Description & formulas)
  : NumericalMathFunction()
{
#ifdef OPENTURNS_HAVE_MUPARSER
  const Description outputVariablesNames(Description::BuildDefault(formulas.getSize(), "y"));

  // Try to build an analytical gradient
  AnalyticalNumericalMathEvaluationImplementation evaluation(inputVariablesNames, outputVariablesNames, formulas);
  setEvaluation(evaluation.clone());
  try
  {
    setGradient(new AnalyticalNumericalMathGradientImplementation(evaluation));
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical gradient, using finite differences instead.");
    const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("CenteredFiniteDifferenceGradient-DefaultEpsilon");
    setGradient(new CenteredFiniteDifferenceGradient(epsilon, getEvaluation()));
  }
  try
  {
    setHessian(new AnalyticalNumericalMathHessianImplementation(evaluation));
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical hessian, using finite differences instead.");
    const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("CenteredFiniteDifferenceHessian-DefaultEpsilon");
    setHessian(new CenteredFiniteDifferenceHessian(epsilon, getEvaluation()));
  }
#else
  throw NotYetImplementedException(HERE) << "AnalyticalFunction requires muParser";
#endif
}

/* Comparison operator */
Bool AnalyticalFunction::operator ==(const AnalyticalFunction & other) const
{
  return true;
}

/* String converter */
String AnalyticalFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << AnalyticalFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String AnalyticalFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
