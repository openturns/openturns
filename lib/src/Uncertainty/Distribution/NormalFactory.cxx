//                                               -*- C++ -*-
/**
 *  @brief Factory for Normal distribution
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
#include <cmath>
#include "openturns/NormalFactory.hxx"
#include "openturns/NormalCopulaFactory.hxx"
#include "openturns/Chi.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NormalFactory)

static const Factory<NormalFactory> Factory_NormalFactory;

/* Default constructor */
NormalFactory::NormalFactory(const Bool robust)
  : DistributionFactoryImplementation()
  , robust_(robust)
{
  // Nothing to do
}

/* Virtual constructor */
NormalFactory * NormalFactory::clone() const
{
  return new NormalFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution NormalFactory::build(const Sample & sample) const
{
  return buildAsNormal(sample).clone();
}

Distribution NormalFactory::build() const
{
  return buildAsNormal().clone();
}

Distribution NormalFactory::build(const Point & parameters) const
{
  return buildAsNormal(parameters).clone();
}

Normal NormalFactory::buildAsNormal(const Sample & sample) const
{
  if (sample.getSize() < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Normal distribution from a sample of size < 2";
  // Robust estimator
  if (robust_)
    {
      const UnsignedInteger dimension = sample.getDimension();
      const Point levels = {0.25, 0.5, 0.75};
      const Sample quantiles(sample.computeQuantilePerComponent(levels));
      // Factor to conver inter-quartiles into standard deviation
      static const Scalar alpha = 1.0 / 1.3489795003921634;
      const Point std((quantiles[2] - quantiles[0]) * alpha);
      if (dimension == 1) return Normal(quantiles(1, 0), std[0]);
      // Robust estimation of the correlation using first Kendall's tau then Spearman's rho
      return Normal(quantiles[1], std, NormalCopulaFactory().buildAsNormalCopula(sample).getShapeMatrix());
    } // robust
  // MLE estimator
  const Point mean(sample.computeMean());
  const CovarianceMatrix covariance(sample.computeCovariance());
  Normal result(mean, covariance);
  result.setDescription(sample.getDescription());
  return result;
}

Normal NormalFactory::buildAsNormal(const Point & parameters) const
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


DistributionFactoryResult NormalFactory::buildEstimator(const Sample & sample) const
{
  if (sample.getDimension() > 1) return buildBootStrapEstimator(sample);
  Normal distribution(buildAsNormal(sample));
  Scalar mu = distribution.getMean()[0];
  Scalar sigma = distribution.getSigma()[0];
  const UnsignedInteger size = sample.getSize();
  ComposedDistribution::DistributionCollection coll;
  Normal muDistribution(mu, sigma / sqrt(1.0 * size));
  coll.add(muDistribution);
  Distribution sigmaDistribution(Chi(size - 1) * (sigma / sqrt(1.0 * size - 1.0)));
  coll.add(sigmaDistribution);
  ComposedDistribution parametersDistribution(coll);
  DistributionFactoryResult result(distribution, parametersDistribution);
  return result;
}

END_NAMESPACE_OPENTURNS
