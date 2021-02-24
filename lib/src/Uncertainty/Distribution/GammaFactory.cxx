//                                               -*- C++ -*-
/**
 *  @brief Factory for Gamma distribution
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
#include "openturns/GammaFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GammaFactory)

static const Factory<GammaFactory> Factory_GammaFactory;

/* Default constructor */
GammaFactory::GammaFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
GammaFactory * GammaFactory::clone() const
{
  return new GammaFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution GammaFactory::build(const Sample & sample) const
{
  return buildAsGamma(sample).clone();
}

Distribution GammaFactory::build(const Point & parameters) const
{
  return buildAsGamma(parameters).clone();
}

Distribution GammaFactory::build() const
{
  return buildAsGamma().clone();
}

Gamma GammaFactory::buildAsGamma(const Sample & sample) const
{
  UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Gamma distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a Gamma distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  const Scalar gamma = xMin - std::abs(xMin) / (2.0 + size);
  const Scalar mu = sample.computeMean()[0];
  const Scalar sigma = sample.computeStandardDeviation()[0];
  if (!SpecFunc::IsNormal(sigma)) throw InvalidArgumentException(HERE) << "Error: cannot build a Gamma distribution if data contains NaN or Inf";
  if (sigma == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a Gamma distribution from a constant sample.";
  Scalar lambda = (mu - gamma) / sigma;
  const Scalar k = lambda * lambda;
  lambda /= sigma;
  Gamma result(k, lambda, gamma);
  result.setDescription(sample.getDescription());
  return result;
}

Gamma GammaFactory::buildAsGamma(const Point & parameters) const
{
  try
  {
    Gamma distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Gamma distribution from the given parameters";
  }
}

Gamma GammaFactory::buildAsGamma() const
{
  return Gamma();
}

END_NAMESPACE_OPENTURNS
