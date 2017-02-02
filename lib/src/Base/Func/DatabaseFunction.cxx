//                                               -*- C++ -*-
/**
 *  @brief The class that implements a pre-computed function.
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
#include "openturns/DatabaseFunction.hxx"
#include "openturns/DatabaseNumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DatabaseFunction);

/* Default constructor */
DatabaseFunction::DatabaseFunction ()
  : NumericalMathFunction()
{
}


/* Parameter constructor */
DatabaseFunction::DatabaseFunction (const NumericalSample & inputSample,
                                    const NumericalSample & outputSample,
                                    const Bool enableCache)
  : NumericalMathFunction()
{
  setEvaluation(new DatabaseNumericalMathEvaluationImplementation(inputSample, outputSample, enableCache));
}


/* Comparison operator */
Bool DatabaseFunction::operator ==(const DatabaseFunction & other) const
{
  return true;
}

/* String converter */
String DatabaseFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String DatabaseFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
