//                                               -*- C++ -*-
/**
 *  @brief Factory for SquaredNormal distribution
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
#include "openturns/SquaredNormalFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SquaredNormalFactory)

static const Factory<SquaredNormalFactory> Factory_SquaredNormalFactory;

/* Default constructor */
SquaredNormalFactory::SquaredNormalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
SquaredNormalFactory * SquaredNormalFactory::clone() const
{
  return new SquaredNormalFactory(*this);
}


/* Here is the interface that all derived class must implement */

Distribution SquaredNormalFactory::build(const Sample & sample) const
{
  return buildAsSquaredNormal(sample).clone();
}

Distribution SquaredNormalFactory::build(const Point & parameters) const
{
  return buildAsSquaredNormal(parameters).clone();
}

Distribution SquaredNormalFactory::build() const
{
  return buildAsSquaredNormal().clone();
}

/* Algorithm associated with the method of moments */
SquaredNormal SquaredNormalFactory::buildMethodOfMoments(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: cannot build a SquaredNormal distribution from a sample of size < 2";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a SquaredNormal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const Scalar xMin = sample.getMin()[0];
  if (!std::isfinite(xMin)) throw InvalidArgumentException(HERE) << "Error: cannot build a SquaredNormal distribution if data contains NaN or Inf";
  if (xMin < 0.0) throw InvalidArgumentException(HERE) << "Error: SquaredNormal is defined for nonnegative data, but min=" << xMin;
  const Scalar mean = sample.computeMean()[0];
  const Scalar variance = sample.computeVariance()[0];
  if (!std::isfinite(mean) || !std::isfinite(variance))
    throw InvalidArgumentException(HERE) << "Error: cannot build a SquaredNormal distribution if data contains NaN or Inf";
  // Mean = mu^2 + sigma^2
  // Variance = 4*mu^2*sigma^2 + 2*sigma^4
  // Let s = sigma^2, then:
  //   m1 = u + s  => u = m1 - s
  //   m2 = 4*u*s + 2*s^2 = 4*(m1 - s)*s + 2*s^2 = 4*m1*s - 2*s^2
  // => 2*s^2 - 4*m1*s + m2 = 0  => s^2 - 2*m1*s + m2/2 = 0
  // => s = m1 - sqrt(m1^2 - m2/2) (the root with s <= m1)
  const Scalar m1 = mean;
  const Scalar m2 = variance;
  const Scalar discriminant = m1 * m1 - 0.5 * m2;
  if (discriminant < 0.0)
    throw InvalidArgumentException(HERE) << "Error: cannot build a SquaredNormal distribution from the given sample, discriminant=" << discriminant;
  const Scalar s = m1 - std::sqrt(discriminant);
  if (!(s > 0.0))
    throw InvalidArgumentException(HERE) << "Error: cannot build a SquaredNormal distribution from the given sample, sigma^2=" << s;
  const Scalar sigma = std::sqrt(s);
  const Scalar mu = std::sqrt(m1 - s);
  SquaredNormal result(mu, sigma);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

SquaredNormal SquaredNormalFactory::buildAsSquaredNormal(const Sample & sample) const
{
  return buildMethodOfMoments(sample);
}

SquaredNormal SquaredNormalFactory::buildAsSquaredNormal(const Point & parameters) const
{
  try
  {
    SquaredNormal distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a SquaredNormal distribution from the given parameters";
  }
}

SquaredNormal SquaredNormalFactory::buildAsSquaredNormal() const
{
  return SquaredNormal();
}

END_NAMESPACE_OPENTURNS
