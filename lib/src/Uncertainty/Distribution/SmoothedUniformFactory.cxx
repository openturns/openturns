//                                               -*- C++ -*-
/**
 *  @brief Factory for SmoothedUniform distribution
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
#include "openturns/SmoothedUniformFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SmoothedUniformFactory)

static const Factory<SmoothedUniformFactory> Factory_SmoothedUniformFactory;

/* Default constructor */
SmoothedUniformFactory::SmoothedUniformFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
SmoothedUniformFactory * SmoothedUniformFactory::clone() const
{
  return new SmoothedUniformFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution SmoothedUniformFactory::build(const Sample & sample) const
{
  return buildAsSmoothedUniform(sample).clone();
}

Distribution SmoothedUniformFactory::build(const Point & parameters) const
{
  return buildAsSmoothedUniform(parameters).clone();
}

Distribution SmoothedUniformFactory::build() const
{
  return buildAsSmoothedUniform().clone();
}

SmoothedUniform SmoothedUniformFactory::buildAsSmoothedUniform(const Sample & sample) const
{
  const Scalar size = sample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: cannot build a SmoothedUniform distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a SmoothedUniform distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar mean = sample.computeMean()[0];
  if (!SpecFunc::IsNormal(mean)) throw InvalidArgumentException(HERE) << "Error: cannot build an UserDefined distribution if data contains NaN or Inf";
  // method of moments
  const Scalar variance = sample.computeVariance()[0];
  if (!(variance > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot estimate a SmoothedUniform distribution from a constant sample.";
  const Scalar kurtosis = sample.computeKurtosis()[0];
  const Scalar delta = std::pow(120.0 * variance * variance * (3.0 - kurtosis), 0.25);
  const Scalar a = mean - 0.5 * delta;
  const Scalar b = mean + 0.5 * delta;
  const Scalar sigma = std::sqrt(variance - delta * delta / 12.0);
  const SmoothedUniform initial(a, b, sigma);
  // finish the job with MLE
  const MaximumLikelihoodFactory mleFactory(initial);
  const Point parameters(mleFactory.buildParameter(sample));
  return buildAsSmoothedUniform(parameters);
}

SmoothedUniform SmoothedUniformFactory::buildAsSmoothedUniform(const Point & parameters) const
{
  try
  {
    SmoothedUniform distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a SmoothedUniform distribution from the given parameters";
  }
}

SmoothedUniform SmoothedUniformFactory::buildAsSmoothedUniform() const
{
  return SmoothedUniform();
}

END_NAMESPACE_OPENTURNS
