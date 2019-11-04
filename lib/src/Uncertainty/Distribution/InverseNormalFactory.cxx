//                                               -*- C++ -*-
/**
 *  @brief Factory for InverseNormal distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/InverseNormalFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseNormalFactory)

static const Factory<InverseNormalFactory> Factory_InverseNormalFactory;

/* Default constructor */
InverseNormalFactory::InverseNormalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
InverseNormalFactory * InverseNormalFactory::clone() const
{
  return new InverseNormalFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution InverseNormalFactory::build(const Sample & sample) const
{
  return buildAsInverseNormal(sample).clone();
}

Distribution InverseNormalFactory::build(const Point & parameters) const
{
  return buildAsInverseNormal(parameters).clone();
}

Distribution InverseNormalFactory::build() const
{
  return buildAsInverseNormal().clone();
}

InverseNormal InverseNormalFactory::buildAsInverseNormal(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an InverseNormal distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot build an InverseNormal distribution from a sample of dimension different from 1";
  const Scalar mu = sample.computeMean()[0];
  Scalar lambda = -1.0;
  if (ResourceMap::GetAsString("InverseNormalFactory-Method") == "MLE")
    // Maximum likelihood estimation
  {
    Scalar sum = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar xI = sample(i, 0);
      if (!(xI > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot build an InverseNormal distribution from a sample with non positive values";
      sum += 1.0 / sample(i, 0);
    }
    lambda = 1.0 / (sum / size - 1.0 / mu);
  }
  // Moments estimation
  else lambda = std::pow(mu, 3) / sample.computeCovariance()(0, 0);
  InverseNormal result(mu, lambda);
  result.setDescription(sample.getDescription());
  return result;
}

InverseNormal InverseNormalFactory::buildAsInverseNormal(const Point & parameters) const
{
  try
  {
    InverseNormal distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a InverseNormal distribution from the given parameters";
  }
}

InverseNormal InverseNormalFactory::buildAsInverseNormal() const
{
  return InverseNormal();
}

END_NAMESPACE_OPENTURNS
