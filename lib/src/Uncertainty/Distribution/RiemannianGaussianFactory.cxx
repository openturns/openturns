//                                               -*- C++ -*-
/**
 *  @brief Factory for RiemannianGaussian distribution
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

#include "openturns/RiemannianGaussianFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RiemannianGaussianFactory)

static const Factory<RiemannianGaussianFactory> Factory_RiemannianGaussianFactory;

RiemannianGaussianFactory::RiemannianGaussianFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

RiemannianGaussianFactory * RiemannianGaussianFactory::clone() const
{
  return new RiemannianGaussianFactory(*this);
}

Distribution RiemannianGaussianFactory::build(const Sample & sample) const
{
  return buildAsRiemannianGaussian(sample).clone();
}

Distribution RiemannianGaussianFactory::build(const Point & parameters) const
{
  return buildAsRiemannianGaussian(parameters).clone();
}

Distribution RiemannianGaussianFactory::build() const
{
  return buildAsRiemannianGaussian().clone();
}

RiemannianGaussian RiemannianGaussianFactory::buildAsRiemannianGaussian(const Point & parameters) const
{
  try
  {
    RiemannianGaussian distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a RiemannianGaussian distribution from the given parameters";
  }
}

RiemannianGaussian RiemannianGaussianFactory::buildAsRiemannianGaussian() const
{
  return RiemannianGaussian();
}

/*
 * Estimate the Riemannian Gaussian parameters from a sample of SPD matrices
 * The mean is the Frechet mean on SPD (using simplified approach)
 * The covariance is estimated in the tangent space at the mean
 */
RiemannianGaussian RiemannianGaussianFactory::buildAsRiemannianGaussian(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();

  if (size < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a RiemannianGaussian distribution from a sample of size < 3";

  const UnsignedInteger d = sample.getDimension();
  // Find n such that n*(n+1)/2 = d
  UnsignedInteger n = 0;
  for (UnsignedInteger k = 2; k <= 10; ++k)
  {
    if (k * (k + 1) / 2 == d)
    {
      n = k;
      break;
    }
  }
  if (n == 0)
    throw InvalidArgumentException(HERE) << "Error: sample dimension " << d << " does not correspond to a symmetric matrix dimension";

  // Step 1: Convert samples to symmetric matrices
  std::vector<SymmetricMatrix> samples(size, SymmetricMatrix(n));
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    UnsignedInteger idx = 0;
    for (UnsignedInteger r = 0; r < n; ++r)
      for (UnsignedInteger c = r; c < n; ++c)
      {
        samples[i](r, c) = sample(i, idx++);
      }
  }

  // Step 2: Compute the Frechet mean on SPD using the Karcher (gradient) algorithm
  // First initialize with the arithmetic mean projected onto SPD
  SymmetricMatrix mean(n);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger r = 0; r < n; ++r)
      for (UnsignedInteger c = r; c < n; ++c)
        mean(r, c) += samples[i](r, c);
  }
  for (UnsignedInteger r = 0; r < n; ++r)
    for (UnsignedInteger c = r; c < n; ++c)
      mean(r, c) /= static_cast<Scalar>(size);

  // Ensure positive definite by eigenvalue correction
  SymmetricMatrix meanCopy(mean);
  SquareMatrix meanEigVec(n);
  Point meanEig = meanCopy.computeEVInPlace(meanEigVec);
  for (UnsignedInteger i = 0; i < n; ++i)
    meanEig[i] = std::max(meanEig[i], SpecFunc::ScalarEpsilon);
  // Reconstruct the positive definite mean
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger k = 0; k < n; ++k)
        sum += meanEigVec(i, k) * meanEig[k] * meanEigVec(j, k);
      mean(i, j) = sum;
    }

  // Refine the Frechet mean with the Karcher iterations:
  // m <- exp_m(average_i log_m(x_i)) until the average tangent vanishes
  SquareMatrix identityTangent(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    identityTangent(i, i) = 1.0;
  Scalar scale = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      scale += mean(i, j) * mean(i, j) * (j > i ? 2.0 : 1.0);
  scale = std::sqrt(scale);
  const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("RiemannianGaussianFactory-MaximumIteration");
  const Scalar tolerance = ResourceMap::GetAsScalar("RiemannianGaussianFactory-Tolerance");
  const Scalar stepSize = ResourceMap::GetAsScalar("RiemannianGaussianFactory-StepSize");
  for (UnsignedInteger iter = 0; iter < maximumIteration; ++iter)
  {
    RiemannianGaussian ref(mean, identityTangent);
    SymmetricMatrix vAverage(n);
    Scalar normV = 0.0;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const SymmetricMatrix v = ref.logMap(samples[i]);
      for (UnsignedInteger r = 0; r < n; ++r)
        for (UnsignedInteger c = r; c < n; ++c)
        {
          const Scalar value = v(r, c);
          vAverage(r, c) += value;
          normV += value * value * (c > r ? 2.0 : 1.0);
        }
    }
    normV = std::sqrt(normV / static_cast<Scalar>(size));
    if (normV <= tolerance * (1.0 + scale))
      break;
    for (UnsignedInteger r = 0; r < n; ++r)
      for (UnsignedInteger c = r; c < n; ++c)
        vAverage(r, c) /= static_cast<Scalar>(size);
    mean = ref.expMap(vAverage * stepSize);
  }

  // Step 3: estimate the covariance in the tangent space at the Frechet
  // mean with the affine-invariant logarithmic map. The scatter uses
  // orthonormal (Hilbert-Schmidt) coordinates, matching the convention
  // of the RiemannianGaussian class.
  const Scalar sqrt2 = std::sqrt(2.0);
  RiemannianGaussian tangentRef(mean, identityTangent);
  SquareMatrix sigma(d);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    // Affine-invariant logarithmic map at the estimated mean
    const SymmetricMatrix v = tangentRef.logMap(samples[i]);

    // Flatten v in orthonormal coordinates
    Point vVec(d);
    UnsignedInteger idx = 0;
    for (UnsignedInteger r = 0; r < n; ++r)
      for (UnsignedInteger c = r; c < n; ++c)
      {
        vVec[idx] = v(r, c);
        if (c > r) vVec[idx] *= sqrt2;
        ++idx;
      }

    // Outer product
    for (UnsignedInteger r = 0; r < d; ++r)
      for (UnsignedInteger c = 0; c < d; ++c)
        sigma(r, c) += vVec[r] * vVec[c];
  }
  sigma = sigma * (1.0 / static_cast<Scalar>(size));

  // Ensure the covariance is positive definite: clamp its eigenvalues to
  // twice the positive definiteness threshold of the RiemannianGaussian class
  const Scalar sigmaFloor =
      2.0 * ResourceMap::GetAsScalar("RiemannianGaussian-PositiveDefiniteThreshold");
  SymmetricMatrix sigma_sym(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      sigma_sym(i, j) = sigma(i, j);
  SquareMatrix sigmaEigVec(d);
  const Point sEigRaw = sigma_sym.computeEVInPlace(sigmaEigVec);
  // sigma_sym now holds the eigenvectors: clamp the eigenvalues and
  // reconstruct from this single, valid decomposition
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
    {
      Scalar sum = 0.0;
      for (UnsignedInteger k = 0; k < d; ++k)
        sum += sigmaEigVec(i, k) * std::max(sEigRaw[k], sigmaFloor) * sigmaEigVec(j, k);
      sigma(i, j) = sum;
    }

  RiemannianGaussian result(mean, sigma);
  result.setEpsilon(ResourceMap::GetAsScalar("RiemannianGaussian-PositiveDefiniteThreshold"));
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

END_NAMESPACE_OPENTURNS