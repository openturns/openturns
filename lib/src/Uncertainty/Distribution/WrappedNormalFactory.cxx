//                                               -*- C++ -*-
/**
 *  @brief Factory for WrappedNormal distribution
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

#include "openturns/WrappedNormalFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WrappedNormalFactory)

static const Factory<WrappedNormalFactory> Factory_WrappedNormalFactory;

WrappedNormalFactory::WrappedNormalFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

WrappedNormalFactory * WrappedNormalFactory::clone() const
{
  return new WrappedNormalFactory(*this);
}

Distribution WrappedNormalFactory::build(const Sample & sample) const
{
  return buildAsWrappedNormal(sample).clone();
}

Distribution WrappedNormalFactory::build(const Point & parameters) const
{
  return buildAsWrappedNormal(parameters).clone();
}

Distribution WrappedNormalFactory::build() const
{
  return buildAsWrappedNormal().clone();
}

WrappedNormal WrappedNormalFactory::buildAsWrappedNormal(const Point & parameters) const
{
  try
  {
    // The dimension is inferred from the size d^2 + d + 1 of the parameter vector
    const UnsignedInteger size = parameters.getSize();
    const Scalar root = (std::sqrt(static_cast<Scalar>(1 + 4 * (size - 1))) - 1.0) / 2.0;
    const UnsignedInteger d = static_cast<UnsignedInteger>(std::round(root));
    if (d * (d + 1) + 1 != size)
      throw InvalidArgumentException(HERE) << "Error: expected a parameter vector of size d^2 + d + 1 for some d >= 1, got size=" << size;

    Point mu(d);
    CovarianceMatrix sigma(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      mu[i] = parameters[i];
    UnsignedInteger idx = d;
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j < d; ++j)
        sigma(i, j) = parameters[idx++];
    const Scalar period = parameters[idx];
    return WrappedNormal(mu, sigma, period);
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a WrappedNormal distribution from the given parameters";
  }
}

WrappedNormal WrappedNormalFactory::buildAsWrappedNormal() const
{
  return WrappedNormal();
}

/*
 * Estimate the WrappedNormal parameters from a sample on the torus
 * The mean is the circular mean (Frechet mean on torus)
 * The covariance is estimated in the tangent space at the mean
 */
WrappedNormal WrappedNormalFactory::buildAsWrappedNormal(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger d = sample.getDimension();

  if (size < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a WrappedNormal distribution from a sample of size < 3";
  if (d < 1)
    throw InvalidArgumentException(HERE) << "Error: dimension must be >= 1, got dimension=" << d;

  const Scalar period = ResourceMap::GetAsScalar("WrappedNormalFactory-DefaultPeriod");

  // Step 1: Compute circular mean on torus
  // For each dimension, compute the circular mean
  Point mu(d);
  for (UnsignedInteger j = 0; j < d; ++j)
  {
    Scalar sumCos = 0.0;
    Scalar sumSin = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar angle = sample(i, j);
      sumCos += std::cos(2.0 * M_PI * angle / period);
      sumSin += std::sin(2.0 * M_PI * angle / period);
    }
    sumCos /= static_cast<Scalar>(size);
    sumSin /= static_cast<Scalar>(size);
    const Scalar meanAngle = std::atan2(sumSin, sumCos);
    mu[j] = period * meanAngle / (2.0 * M_PI);
    // Wrap to fundamental domain
    const Scalar halfP = period / 2.0;
    Scalar val = mu[j];
    val = std::fmod(val + halfP, period);
    if (val < 0.0) val += period;
    val -= halfP;
    mu[j] = val;
  }

  // Step 2: Estimate covariance in tangent space at mean
  CovarianceMatrix sigma(d);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Compute difference in tangent space (wrapped difference)
    Point diff(d);
    for (UnsignedInteger j = 0; j < d; ++j)
    {
      Scalar dAngle = sample(i, j) - mu[j];
      // Wrap difference to [-period/2, period/2)
      const Scalar halfP = period / 2.0;
      dAngle = std::fmod(dAngle + halfP, period);
      if (dAngle < 0.0) dAngle += period;
      dAngle -= halfP;
      diff[j] = dAngle;
    }

    // Outer product
    for (UnsignedInteger r = 0; r < d; ++r)
      for (UnsignedInteger c = 0; c < d; ++c)
        sigma(r, c) += diff[r] * diff[c];
  }
  // Scale sigma element-wise to avoid SymmetricMatrix return type
  const Scalar invSize = 1.0 / static_cast<Scalar>(size);
  for (UnsignedInteger r = 0; r < d; ++r)
    for (UnsignedInteger c = 0; c <= r; ++c)
      sigma(r, c) *= invSize;

  // Ensure positive definite
  SymmetricMatrix sigma_sym(sigma);
  SquareMatrix sigmaEigVec(d);
  Point sigmaEig = sigma_sym.computeEVInPlace(sigmaEigVec);
  for (UnsignedInteger i = 0; i < d; ++i)
    sigmaEig[i] = std::max(sigmaEig[i], SpecFunc::ScalarEpsilon);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger k = 0; k < d; ++k)
        sum += sigmaEigVec(i, k) * sigmaEig[k] * sigmaEigVec(j, k);
      sigma(i, j) = sum;
    }

  WrappedNormal result(mu, sigma, period);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

END_NAMESPACE_OPENTURNS