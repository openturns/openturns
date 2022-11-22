//                                               -*- C++ -*-
/**
 *  @brief Factory for Bernoulli distribution
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
#include "openturns/BernoulliFactory.hxx"
#include "openturns/DiscreteDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Binomial.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BernoulliFactory)

static const Factory<BernoulliFactory> Factory_BernoulliFactory;

/* Default constructor */
BernoulliFactory::BernoulliFactory():
  DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
BernoulliFactory * BernoulliFactory::clone() const
{
  return new BernoulliFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution BernoulliFactory::build(const Sample & sample) const
{
  return buildAsBernoulli(sample).clone();
}

Distribution BernoulliFactory::build(const Point & parameters) const
{
  return buildAsBernoulli(parameters).clone();
}

Distribution BernoulliFactory::build() const
{
  return buildAsBernoulli().clone();
}

Bernoulli BernoulliFactory::buildAsBernoulli(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Bernoulli distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Bernoulli distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  Scalar sum = 0.0;
  const Scalar supportEpsilon = ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon");
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar x = sample(i, 0);
    if (!SpecFunc::IsNormal(x))
      throw InvalidArgumentException(HERE) << "Error: cannot build a Bernoulli distribution if data contains NaN or Inf";
    const SignedInteger iX = static_cast<SignedInteger>(std::round(x));
    if ((std::abs(x - iX) > supportEpsilon) || ((iX != 0) && (iX != 1)))
      throw InvalidArgumentException(HERE) << "Error: can build a Bernoulli distribution only from a sample made of 0 and 1.";
    sum += x;
  }
  Bernoulli result(sum / size);
  result.setDescription(sample.getDescription());
  return result;
}

Bernoulli BernoulliFactory::buildAsBernoulli(const Point & parameters) const
{
  try
  {
    Bernoulli distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Bernoulli distribution from the given parameters";
  }
}

Bernoulli BernoulliFactory::buildAsBernoulli() const
{
  return Bernoulli();
}

DistributionFactoryResult BernoulliFactory::buildEstimator(const Sample & sample) const
{
  Bernoulli distribution(buildAsBernoulli(sample));
  const UnsignedInteger size = sample.getSize();
  Distribution parametersDistribution(Binomial(size, distribution.getP()) * (1.0 / size));
  DistributionFactoryResult result(distribution, parametersDistribution);
  return result;
}

END_NAMESPACE_OPENTURNS
