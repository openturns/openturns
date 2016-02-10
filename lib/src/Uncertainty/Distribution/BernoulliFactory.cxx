//                                               -*- C++ -*-
/**
 *  @brief Factory for Bernoulli distribution
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
#include "BernoulliFactory.hxx"
#include "DiscreteDistribution.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(BernoulliFactory);


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

BernoulliFactory::Implementation BernoulliFactory::build(const NumericalSample & sample) const
{
  return buildAsBernoulli(sample).clone();
}

BernoulliFactory::Implementation BernoulliFactory::build(const NumericalPoint & parameters) const
{
  return buildAsBernoulli(parameters).clone();
}

BernoulliFactory::Implementation BernoulliFactory::build() const
{
  return buildAsBernoulli().clone();
}

Bernoulli BernoulliFactory::buildAsBernoulli(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Bernoulli distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Bernoulli distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  NumericalScalar sum(0.0);
  const NumericalScalar supportEpsilon(ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon"));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar x(sample[i][0]);
    const int iX(static_cast<int>(round(x)));
    if ((std::abs(x - iX) > supportEpsilon) || ((iX != 0) && (iX != 1))) throw InvalidArgumentException(HERE) << "Error: can build a Bernoulli distribution only from a sample made of 0 and 1.";
    sum += x;
  }
  Bernoulli result(sum / size);
  result.setDescription(sample.getDescription());
  return result;
}

Bernoulli BernoulliFactory::buildAsBernoulli(const NumericalPoint & parameters) const
{
  try
  {
    Bernoulli distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Bernoulli distribution from the given parameters";
  }
}

Bernoulli BernoulliFactory::buildAsBernoulli() const
{
  return Bernoulli();
}

END_NAMESPACE_OPENTURNS
