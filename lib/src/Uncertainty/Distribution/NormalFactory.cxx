//                                               -*- C++ -*-
/**
 *  @brief Factory for Normal distribution
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
#include <cmath>
#include "openturns/NormalFactory.hxx"
#include "openturns/Chi.hxx"
#include "openturns/ComposedDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NormalFactory);

/* Default constructor */
NormalFactory::NormalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
NormalFactory * NormalFactory::clone() const
{
  return new NormalFactory(*this);
}

/* Here is the interface that all derived class must implement */

NormalFactory::Implementation NormalFactory::build(const NumericalSample & sample) const
{
  return buildAsNormal(sample).clone();
}

NormalFactory::Implementation NormalFactory::build() const
{
  return buildAsNormal().clone();
}

NormalFactory::Implementation NormalFactory::build(const NumericalPoint & parameters) const
{
  return buildAsNormal(parameters).clone();
}

Normal NormalFactory::buildAsNormal(const NumericalSample & sample) const
{
  if (sample.getSize() < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Normal distribution from a sample of size < 2";
  const NumericalPoint mean(sample.computeMean());
  const CovarianceMatrix covariance(sample.computeCovariance());
  Normal result(mean, covariance);
  result.setDescription(sample.getDescription());
  return result;
}

Normal NormalFactory::buildAsNormal(const NumericalPoint & parameters) const
{
  try
  {
    const UnsignedInteger size = parameters.getSize();
    // see EllipticalDistribution::setParameters
    const UnsignedInteger dimension = 0.5 * std::sqrt(9.0 + 8.0 * size) - 1.5;
    Normal distribution(dimension);
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException & ex)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Normal distribution from the given parameters" << ex;
  }
}

Normal NormalFactory::buildAsNormal() const
{
  return Normal();
}


DistributionFactoryResult NormalFactory::buildEstimator(const NumericalSample & sample) const
{
  if (sample.getDimension() > 1) return buildBootStrapEstimator(sample);
  Normal distribution(buildAsNormal(sample));
  NumericalScalar mu = distribution.getMean()[0];
  NumericalScalar sigma = distribution.getSigma()[0];
  const UnsignedInteger size = sample.getSize();
  ComposedDistribution::DistributionCollection coll;
  Normal muDistribution(mu, sigma / sqrt(1.0 * size));
  coll.add(muDistribution);
  Distribution sigmaDistribution(Chi(size - 1) * (1.0 / sqrt(1.0 * size - 1.0)));
  coll.add(sigmaDistribution);
  ComposedDistribution parametersDistribution(coll);
  DistributionFactoryResult result(distribution, parametersDistribution);
  return result;
}

END_NAMESPACE_OPENTURNS
