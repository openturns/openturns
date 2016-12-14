//                                               -*- C++ -*-
/**
 *  @brief The class that implements composed functions.
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
#include "openturns/ComposedFunction.hxx"
#include "openturns/ComposedNumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComposedFunction);

/* Default constructor */
ComposedFunction::ComposedFunction ()
  : NumericalMathFunction()
{
}


/* Parameter constructor */
ComposedFunction::ComposedFunction (const NumericalMathFunction & left,
                        const NumericalMathFunction & right)
  : NumericalMathFunction(new ComposedNumericalMathFunction(left.getImplementation(), right.getImplementation()))
{
}


/* Comparison operator */
Bool ComposedFunction::operator ==(const ComposedFunction & other) const
{
  return true;
}

/* String converter */
String ComposedFunction::__repr__() const
{
  return getImplementation()->__repr__();
}

String ComposedFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
