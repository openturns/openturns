//                                               -*- C++ -*-
/**
 *  @brief Factory for MeixnerDistribution distribution
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MeixnerDistributionFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MeixnerDistributionFactory)

static const Factory<MeixnerDistributionFactory> Factory_MeixnerDistributionFactory;

/* Default constructor */
MeixnerDistributionFactory::MeixnerDistributionFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
MeixnerDistributionFactory * MeixnerDistributionFactory::clone() const
{
  return new MeixnerDistributionFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution MeixnerDistributionFactory::build(const Sample & sample) const
{
  return buildAsMeixnerDistribution(sample).clone();
}

Distribution MeixnerDistributionFactory::build(const Point & parameters) const
{
  return buildAsMeixnerDistribution(parameters).clone();
}

Distribution MeixnerDistributionFactory::build() const
{
  return buildAsMeixnerDistribution().clone();
}

MeixnerDistribution MeixnerDistributionFactory::buildAsMeixnerDistribution(const Sample & sample) const
{
  UnsignedInteger size = sample.getSize();
  if (size < 4) throw InvalidArgumentException(HERE) << "Error: cannot build a MeixnerDistribution distribution from a sample of size less than 4.";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a MeixnerDistribution distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar gamma1 = sample.computeSkewness()[0];
  const Scalar gamma2 = sample.computeKurtosis()[0];
  const Scalar upperBound = 3.0 + 2.0 * gamma1 * gamma1;
  if (gamma2 <= upperBound) throw InvalidArgumentException(HERE) << "Error: cannot estimate a MeixnerDistribution distribution if the sample kurtosis=" << gamma2 << " is not greater than 2*skewness^2+3=" << upperBound;
  const Scalar m = sample.computeMean()[0];
  const Scalar s2 = sample.computeVariance()[0];
  const Scalar delta = 1.0 / (gamma2 - gamma1 * gamma1 - 3.0);
  const Scalar beta = ((0.0 < gamma1) - (gamma1 < 0.0)) * std::acos(2.0 - delta * (gamma2 - 3.0));
  const Scalar alpha = cbrt(s2 * (std::cos(beta) + 1.0));
  const Scalar mu = m - alpha * delta * std::tan(0.5 * beta);
  MeixnerDistribution result(alpha, beta, delta, mu);
  result.setDescription(sample.getDescription());
  return result;
}

MeixnerDistribution MeixnerDistributionFactory::buildAsMeixnerDistribution(const Point & parameters) const
{
  try
  {
    MeixnerDistribution distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a MeixnerDistribution distribution from the given parameters";
  }
}

MeixnerDistribution MeixnerDistributionFactory::buildAsMeixnerDistribution() const
{
  return MeixnerDistribution();
}

END_NAMESPACE_OPENTURNS
