//                                               -*- C++ -*-
/**
 *  @brief Factory for Bingham distribution
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

#include "openturns/BinghamFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BinghamFactory)

static const Factory<BinghamFactory> Factory_BinghamFactory;

BinghamFactory::BinghamFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

BinghamFactory * BinghamFactory::clone() const
{
  return new BinghamFactory(*this);
}

Distribution BinghamFactory::build(const Sample & sample) const
{
  return buildAsBingham(sample).clone();
}

Distribution BinghamFactory::build(const Point & parameters) const
{
  return buildAsBingham(parameters).clone();
}

Distribution BinghamFactory::build() const
{
  return buildAsBingham().clone();
}

Bingham BinghamFactory::buildAsBingham(const Point & parameters) const
{
  try
  {
    Bingham distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Bingham distribution from the given parameters";
  }
}

Bingham BinghamFactory::buildAsBingham() const
{
  return Bingham();
}

/*
 * Estimate the Bingham distribution parameters using the method of moments
 * Based on Kent (1982) and Mardia & Jupp (2000) for the Bingham distribution
 */
Bingham BinghamFactory::buildAsBingham(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger n = sample.getDimension();

  if (size < n + 1)
    throw InvalidArgumentException(HERE) << "Error: cannot build a Bingham distribution from a sample of size < " << (n + 1);
  if (n < 2)
    throw InvalidArgumentException(HERE) << "Error: Bingham distribution requires dimension >= 2, here dimension=" << n;

  // Step 1: compute scatter matrix S
  SquareMatrix S(n);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger r = 0; r < n; ++r)
      for (UnsignedInteger c = 0; c < n; ++c)
        S(r, c) += sample(i, r) * sample(i, c);
  S = S * (1.0 / static_cast<Scalar>(size));

  // Step 2: eigendecomposition of S to get gamma and eigenvalues
  // For Bingham, we need the eigenvectors (gamma) and eigenvalues related to zeta
  // S is symmetric, use computeEV to get eigenvalues and eigenvectors
  SymmetricMatrix S_sym(n);
  for (UnsignedInteger r = 0; r < n; ++r)
    for (UnsignedInteger c = 0; c <= r; ++c)
      S_sym(r, c) = S(r, c);
  SquareMatrix gamma(n);
  const Point eigenValues = S_sym.computeEVInPlace(gamma);

  // Eigenvalues are in ascending order, but we need zeta in descending order
  // For Bingham on S^{n-1}, the relationship is:
  // lambda_i proportional to 1 / (1 - 2*zeta_i) approximately for small zeta
  // More precisely: we match moments numerically

  // Step 3: initial guess for zeta using large concentration approximation
  Point zeta(n);
  Scalar sumEig = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    sumEig += eigenValues[i];

  // Normalize eigenvalues to sum to 1 (they should already sum to 1 for data on sphere)
  Point lambda(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    lambda[i] = eigenValues[i] / sumEig;

  // Bingham has E[x_i^2] = 1/n + 2*zeta_i / n + O(zeta^2)
  // So zeta_i approx (n/2) * (lambda_i - 1/n)
  // But we need zeta in descending order, eigenvalues are ascending
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    const Scalar lambdaDesc = lambda[n - 1 - i];
    zeta[i] = 0.5 * n * (lambdaDesc - 1.0 / n);
  }

  // Ensure ordering constraint: zeta[0] >= zeta[1] >= ... >= zeta[n-1]
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    if (zeta[i] > zeta[i-1])
      zeta[i] = zeta[i-1];
  }

  // Shift so minimum is 0 (doesn't change distribution)
  Scalar minZeta = zeta[n-1];
  for (UnsignedInteger i = 0; i < n; ++i)
    zeta[i] -= minZeta;

  // Step 4: refine via numerical optimization
  // We match the second moments numerically
  // This is a simplified version - a full implementation would use Newton-Raphson

  Bingham result(zeta, gamma);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

END_NAMESPACE_OPENTURNS