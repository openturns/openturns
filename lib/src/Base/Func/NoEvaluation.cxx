//                                               -*- C++ -*-
/**
 * @brief Class for unavailable evaluation implementations
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

#include "openturns/NoEvaluation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NoEvaluation)

static const Factory<NoEvaluation> Factory_NoEvaluation;

/* Default constructor */
NoEvaluation::NoEvaluation()
  : EvaluationImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NoEvaluation * NoEvaluation::clone() const
{
  return new NoEvaluation(*this);
}

/* Comparison operator */
Bool NoEvaluation::operator ==(const NoEvaluation & ) const
{
  return true;
}

/* String converter */
String NoEvaluation::__repr__() const
{
  OSS oss;
  oss << "class=" << NoEvaluation::GetClassName()
      << " name=" << getName();
  return oss;
}

/* Test for actual implementation */
Bool NoEvaluation::isActualImplementation() const
{
  return false;
}






/* Here is the interface that all derived class must implement */

/* Operator () */
Point NoEvaluation::operator() (const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  const Point result;
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger NoEvaluation::getInputDimension() const
{
  return 0;
}

/* Accessor for output point dimension */
UnsignedInteger NoEvaluation::getOutputDimension() const
{
  return 0;
}

END_NAMESPACE_OPENTURNS
