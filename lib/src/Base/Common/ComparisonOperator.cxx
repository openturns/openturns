//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComparisonOperator
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
#include "ComparisonOperator.hxx"
#include "Less.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComparisonOperator);

/* Default constructor */
ComparisonOperator::ComparisonOperator()
  : TypedInterfaceObject<ComparisonOperatorImplementation>(new Less())
{
  // Nothing to do
}


/* Constructor from implementation */
ComparisonOperator::ComparisonOperator(const ComparisonOperatorImplementation & op)
  : TypedInterfaceObject<ComparisonOperatorImplementation>(op.clone())
{
  // Nothing to do
}


/* Return true if comparison succeeds */
Bool ComparisonOperator::operator()(const NumericalScalar a,
                                    const NumericalScalar b) const
{
  return (*getImplementation())(a, b);
}
Bool ComparisonOperator::compare(NumericalScalar a, NumericalScalar b) const
{
  return (*getImplementation())(a, b);
}




END_NAMESPACE_OPENTURNS
