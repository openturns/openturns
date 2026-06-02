//                                               -*- C++
/**
 *  @brief Factory for InverseGamma distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/InverseGammaFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseGammaFactory)

static const Factory<InverseGammaFactory> Factory_InverseGammaFactory;

/* Default constructor */
InverseGammaFactory::InverseGammaFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
InverseGammaFactory * InverseGammaFactory::clone() const
{
  return new InverseGammaFactory(*this);
}

Distribution InverseGammaFactory::build(const Sample & sample) const
{
  return buildAsInverseGamma(sample).clone();
}

Distribution InverseGammaFactory::build(const Point & parameters) const
{
  return buildAsInverseGamma(parameters).clone();
}

Distribution InverseGammaFactory::build() const
{
  return buildAsInverseGamma().clone();
}

InverseGamma InverseGammaFactory::buildMethodOfMoments(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build an InverseGamma distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an InverseGamma distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  if (!std::isfinite(xMin)) throw InvalidArgumentException(HERE) << "Error: cannot build an InverseGamma distribution if data contains NaN or Inf";
  if (xMin <= 0.0) throw InvalidArgumentException(HERE) << "Error: InverseGamma is defined for positive data, but min=" << xMin;
  const Scalar mean = sample.computeMean()[0];
  const Scalar variance = sample.computeVariance()[0];
  // Mean = 1 / (lambda * (k - 1)), k > 1
  // Variance = 1 / (lambda^2 * (k - 1)^2 * (k - 2)), k > 2
  // => k = mean^2 / variance + 2
  // => lambda = 1 / (mean * (k - 1))
  const Scalar k = mean * mean / variance + 2.0;
  if (!(k > 2.0)) throw InvalidArgumentException(HERE) << "Error: cannot build an InverseGamma distribution from the given sample, k=" << k;
  const Scalar lambda = 1.0 / (mean * (k - 1.0));
  if (!(lambda > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot build an InverseGamma distribution from the given sample, lambda=" << lambda;
  InverseGamma result(k, lambda);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

InverseGamma InverseGammaFactory::buildAsInverseGamma(const Sample & sample) const
{
  return buildMethodOfMoments(sample);
}

InverseGamma InverseGammaFactory::buildAsInverseGamma(const Point & parameters) const
{
  try
  {
    InverseGamma distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build an InverseGamma distribution from the given parameters";
  }
}

InverseGamma InverseGammaFactory::buildAsInverseGamma() const
{
  return InverseGamma();
}

END_NAMESPACE_OPENTURNS
