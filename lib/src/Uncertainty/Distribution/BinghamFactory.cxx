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
#include "openturns/Matrix.hxx"
#include <algorithm>
#include <cmath>

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

  // Step 3: pair descending eigenvalues with descending concentrations.
  // computeEVInPlace returns ascending eigenvalues with matching columns,
  // so column i of the orientation matrix is permuted to hold the
  // eigenvector of the (n-1-i)-th ascending eigenvalue.
  Point lambdaDesc(n);
  Scalar sumEig = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    sumEig += eigenValues[i];

  // Normalize eigenvalues to sum to 1 (they should already sum to 1 for data on sphere)
  SquareMatrix sortedGamma(n);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    lambdaDesc[i] = eigenValues[n - 1 - i] / sumEig;
    for (UnsignedInteger r = 0; r < n; ++r)
      sortedGamma(r, i) = gamma(r, n - 1 - i);
  }
  gamma = sortedGamma;

  // Initial guess from the small-concentration expansion
  // E[x_i^2] = 1/n + 2*zeta_i / n + O(zeta^2)
  Point zeta(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    zeta[i] = 0.5 * n * (lambdaDesc[i] - 1.0 / n);

  // Shift so minimum is 0 (doesn't change distribution)
  Scalar minZeta = zeta[n - 1];
  for (UnsignedInteger i = 1; i < n; ++i)
    minZeta = std::min(minZeta, zeta[i]);
  for (UnsignedInteger i = 0; i < n; ++i)
    zeta[i] -= minZeta;

  // Step 4: refine by Newton-Raphson on the moment equations.
  // Adding a constant to zeta leaves the distribution unchanged, so the
  // gauge is fixed by zeta[n-1] = 0 and only the first n-1 concentrations
  // are solved from E[x_i^2](zeta) = lambdaDesc[i].
  const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("BinghamFactory-MaximumIterations");
  const Scalar residualPrecision = std::sqrt(SpecFunc::Precision);
  const UnsignedInteger freeDimension = n - 1;
  for (UnsignedInteger iteration = 0; iteration < maximumIterations; ++iteration)
  {
    const Bingham current(zeta, gamma);
    const Point moments(current.computeSecondMoments());
    Point residual(freeDimension);
    Scalar residualNorm = 0.0;
    for (UnsignedInteger i = 0; i < freeDimension; ++i)
    {
      residual[i] = moments[i] - lambdaDesc[i];
      residualNorm = std::max(residualNorm, std::abs(residual[i]));
    }
    if (residualNorm <= residualPrecision) break;
    // Jacobian by central finite differences of the second moments
    Matrix jacobian(freeDimension, freeDimension);
    for (UnsignedInteger j = 0; j < freeDimension; ++j)
    {
      const Scalar step = std::sqrt(SpecFunc::ScalarEpsilon) * std::max(1.0, std::abs(zeta[j]));
      Point zetaPlus(zeta);
      Point zetaMinus(zeta);
      zetaPlus[j] += step;
      zetaMinus[j] -= step;
      const Bingham plus(zetaPlus, gamma);
      const Bingham minus(zetaMinus, gamma);
      const Point momentsPlus(plus.computeSecondMoments());
      const Point momentsMinus(minus.computeSecondMoments());
      for (UnsignedInteger i = 0; i < freeDimension; ++i)
        jacobian(i, j) = (momentsPlus[i] - momentsMinus[i]) / (2.0 * step);
    }
    Point negativeResidual(freeDimension);
    for (UnsignedInteger i = 0; i < freeDimension; ++i)
      negativeResidual[i] = -residual[i];
    const Point stepDirection(jacobian.solveLinearSystem(negativeResidual));
    // Damped update: halve the step until the residual decreases
    Scalar damping = 1.0;
    Point trialZeta(zeta);
    for (UnsignedInteger halving = 0; halving < maximumIterations; ++halving)
    {
      for (UnsignedInteger i = 0; i < freeDimension; ++i)
        trialZeta[i] = zeta[i] + damping * stepDirection[i];
      // Gauge shift (leaves the law unchanged) then descending projection,
      // so the trial point always satisfies the constructor requirements
      const Scalar trialShift = trialZeta[n - 1];
      for (UnsignedInteger i = 0; i < n; ++i)
        trialZeta[i] -= trialShift;
      for (UnsignedInteger i = 1; i < n; ++i)
        trialZeta[i] = std::min(trialZeta[i], trialZeta[i - 1]);
      const Bingham trial(trialZeta, gamma);
      const Point trialMoments(trial.computeSecondMoments());
      Scalar trialNorm = 0.0;
      for (UnsignedInteger i = 0; i < freeDimension; ++i)
        trialNorm = std::max(trialNorm, std::abs(trialMoments[i] - lambdaDesc[i]));
      if (trialNorm < residualNorm)
      {
        zeta = trialZeta;
        break;
      }
      damping *= 0.5;
      if (halving + 1 == maximumIterations)
        zeta = trialZeta;
    }
  }

  // Restore the descending order (joint permutation leaves the law unchanged)
  // and the canonical shift, as Newton steps may have crossed levels
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = i + 1; j < n; ++j)
    {
      if (zeta[j] > zeta[i])
      {
        std::swap(zeta[i], zeta[j]);
        for (UnsignedInteger r = 0; r < n; ++r)
          std::swap(gamma(r, i), gamma(r, j));
      }
    }
  minZeta = zeta[n - 1];
  for (UnsignedInteger i = 0; i < n; ++i)
    zeta[i] -= minZeta;

  Bingham result(zeta, gamma);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

END_NAMESPACE_OPENTURNS