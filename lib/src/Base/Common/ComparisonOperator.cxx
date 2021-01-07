//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all ComparisonOperator
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
#include "openturns/ComparisonOperator.hxx"
#include "openturns/Less.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComparisonOperator)

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
Bool ComparisonOperator::operator()(const Scalar a,
                                    const Scalar b) const
{
  return (*getImplementation())(a, b);
}

Bool ComparisonOperator::compare(Scalar a, Scalar b) const
{
  return (*getImplementation())(a, b);
}


String ComparisonOperator::__repr__() const
{
  return getImplementation()->__repr__();
}


END_NAMESPACE_OPENTURNS
