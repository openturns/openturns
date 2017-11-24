//                                               -*- C++ -*-
/**
 *  @brief Factory for Categorical distribution
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
#include "openturns/CategoricalFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CategoricalFactory)

static const Factory<CategoricalFactory> Factory_CategoricalFactory;

/* Default constructor */
CategoricalFactory::CategoricalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
CategoricalFactory * CategoricalFactory::clone() const
{
  return new CategoricalFactory(*this);
}


/* Here is the interface that all derived class must implement */

CategoricalFactory::Implementation CategoricalFactory::build(const Sample & sample) const
{
  return buildAsCategorical(sample, 0.0).clone();
}

CategoricalFactory::Implementation CategoricalFactory::build(const Sample & sample,
    const Scalar epsilon) const
{
  return buildAsCategorical(sample, epsilon).clone();
}

CategoricalFactory::Implementation CategoricalFactory::build() const
{
  return buildAsCategorical().clone();
}

Categorical CategoricalFactory::buildAsCategorical(const Sample & sample,
    const Scalar epsilon) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Categorical distribution from an empty sample";
  const Scalar p = 1.0 / Scalar(size);
  Categorical result(sample, Point(size, p));
  result.compactSupport(epsilon);
  result.setDescription(sample.getDescription());
  return result;
}

Categorical CategoricalFactory::buildAsCategorical() const
{
  return Categorical();
}


END_NAMESPACE_OPENTURNS
