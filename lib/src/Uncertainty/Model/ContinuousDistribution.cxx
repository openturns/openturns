//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Continuous distributions
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>
#include <cstdlib>

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/GaussKronrod.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ContinuousDistribution)

/* Default constructor */
ContinuousDistribution::ContinuousDistribution()
  : DistributionImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
ContinuousDistribution * ContinuousDistribution::clone() const
{
  return new ContinuousDistribution(*this);
}

/* Comparison operator */
Bool ContinuousDistribution::operator ==(const ContinuousDistribution & other) const
{
  return (this == &other);
}

/* String converter */
String ContinuousDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << ContinuousDistribution::GetClassName();
  return oss;
}

/* Get the PDF of the distribution */
Scalar ContinuousDistribution::computePDF(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In ContinuousDistribution::computePDF(const Point & point) const";
}

/* Tell if the distribution is continuous */
Bool ContinuousDistribution::isContinuous() const
{
  return true;
}

END_NAMESPACE_OPENTURNS
