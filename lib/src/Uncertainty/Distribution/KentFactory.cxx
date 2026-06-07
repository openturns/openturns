//                                               -*- C++ -*-
/**
 *  @brief Factory for Kent distribution
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

#include "openturns/KentFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/IdentityMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KentFactory)

static const Factory<KentFactory> Factory_KentFactory;

KentFactory::KentFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

KentFactory * KentFactory::clone() const
{
  return new KentFactory(*this);
}

Distribution KentFactory::build(const Sample & sample) const
{
  return buildAsKent(sample).clone();
}

Distribution KentFactory::build(const Point & parameters) const
{
  return buildAsKent(parameters).clone();
}

Distribution KentFactory::build() const
{
  return buildAsKent().clone();
}

Kent KentFactory::buildAsKent(const Point & parameters) const
{
  try
  {
    Kent distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Kent distribution from the given parameters";
  }
}

Kent KentFactory::buildAsKent() const
{
  return Kent();
}



/*
 * Estimate the Kent distribution parameters using the method of moments
 * described in Kent (1982), J. R. Statist. Soc. B, 44(1), 71-80, Section 4.
 */
Kent KentFactory::buildAsKent(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: cannot build a Kent distribution from a sample of size < 3";
  if (sample.getDimension() != 3) throw InvalidArgumentException(HERE) << "Error: can build a Kent distribution only from a sample of dimension 3, here dimension=" << sample.getDimension();

  // Step 1: sample mean and scatter matrix
  const Point xBar = sample.computeMean();
  SquareMatrix S(3);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger r = 0; r < 3; ++r)
      for (UnsignedInteger c = 0; c < 3; ++c)
        S(r, c) += sample(i, r) * sample(i, c);
  S = S * (1.0 / static_cast<Scalar>(size));

  const Scalar normXBar = xBar.norm();

  // Step 2: build H matrix whose rows form an orthonormal basis
  // H maps xBar to (normXBar, 0, 0)^T
  SquareMatrix H(3);
  Point u(3);
  if (normXBar > 0.0)
  {
    u = xBar / normXBar;
  }
  else
  {
    u[0] = 1.0;
    u[1] = 0.0;
    u[2] = 0.0;
  }

  Point v(3);
  Point w(3);
  // v = cross(e3, u), normalized
  // If u is nearly aligned with e3, use cross(e1, u)
  const Scalar crossNorm = std::sqrt(u[0] * u[0] + u[1] * u[1]);
  if (crossNorm > SpecFunc::ScalarEpsilon)
  {
    v[0] = -u[1] / crossNorm;
    v[1] =  u[0] / crossNorm;
    v[2] = 0.0;
  }
  else
  {
    // u is aligned with e3, use cross(e1, u)
    v[0] = 0.0;
    v[1] =  u[2];
    v[2] = -u[1];
    const Scalar normV = v.norm();
    if (normV > 0.0) v /= normV;
  }
  // w = cross(u, v)
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];

  for (UnsignedInteger i = 0; i < 3; ++i)
  {
    H(0, i) = u[i];
    H(1, i) = v[i];
    H(2, i) = w[i];
  }

  // Step 3: dispersion in rotated frame B = H * S * H^T
  const SquareMatrix B(H * S * H.transpose());

  // Step 4: diagonalize the lower-right 2x2 block
  const Scalar b22 = B(1, 1);
  const Scalar b23 = B(1, 2);
  const Scalar b33 = B(2, 2);
  Scalar psi = 0.5 * std::atan2(2.0 * b23, b22 - b33);

  // Build K^T (transpose of rotation about axis 1 by psi)
  const Scalar cosPsi = std::cos(psi);
  const Scalar sinPsi = std::sin(psi);

  // Check ordering: T_22 should be > T_33
  const Scalar t22 = cosPsi * cosPsi * b22 + sinPsi * sinPsi * b33 - 2.0 * sinPsi * cosPsi * b23;
  const Scalar t33 = sinPsi * sinPsi * b22 + cosPsi * cosPsi * b33 + 2.0 * sinPsi * cosPsi * b23;
  if (t22 < t33)
  {
    psi += M_PI / 2.0;
  }

  const Scalar cosPsi2 = std::cos(psi);
  const Scalar sinPsi2 = std::sin(psi);
  SquareMatrix KT(3);
  KT(0, 0) = 1.0;
  KT(1, 1) = cosPsi2;
  KT(1, 2) = -sinPsi2;
  KT(2, 1) = sinPsi2;
  KT(2, 2) = cosPsi2;

  // Gamma = H^T * K^T
  const SquareMatrix gamma(H.transpose() * KT);

  // Step 5: observed moments
  const Scalar r1Obs = normXBar;
  // After full rotation, T = Gamma^T * S * Gamma = K * B * K^T
  const Scalar t22Final = cosPsi2 * cosPsi2 * b22 + sinPsi2 * sinPsi2 * b33 - 2.0 * sinPsi2 * cosPsi2 * b23;
  const Scalar t33Final = sinPsi2 * sinPsi2 * b22 + cosPsi2 * cosPsi2 * b33 + 2.0 * sinPsi2 * cosPsi2 * b23;
  const Scalar r2Obs = t22Final - t33Final;

  // Step 6: initial guess via large-kappa approximation
  Scalar kappa = 10.0;
  Scalar beta = 0.0;

  const Scalar denom1 = 2.0 - 2.0 * r1Obs - r2Obs;
  const Scalar denom2 = 2.0 - 2.0 * r1Obs + r2Obs;

  if ((denom1 > 0.0) && (denom2 > 0.0))
  {
    kappa = 1.0 / denom1 + 1.0 / denom2;
    beta = 0.5 * (1.0 / denom1 - 1.0 / denom2);
  }
  else if (denom2 > 0.0)
  {
    // r2Obs is too large relative to (1 - r1Obs), clamp beta such that 2*beta < kappa
    beta = 1.0 / denom2;
    kappa = 2.0 * beta + 0.1;
  }
  else
  {
    // Data is too dispersed, use a default
    kappa = 1.0;
    beta = 0.0;
  }

  // Ensure constraints
  if (!(kappa > 0.0)) kappa = 1.0;
  if (!(beta >= 0.0)) beta = 0.0;
  if (!(2.0 * beta < kappa)) beta = 0.5 * kappa * (1.0 - SpecFunc::ScalarEpsilon);

  // Step 7: refine via Newton-Raphson
  const Scalar absolutePrecision = ResourceMap::GetAsScalar("KentFactory-AbsolutePrecision");
  const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("Kent-MaximumIteration");
  const Scalar epsilon = SpecFunc::ScalarEpsilon;

  for (UnsignedInteger iter = 0; iter < maximumIteration; ++iter)
  {
    Point moments(Kent::ComputeMoments(kappa, beta));
    const Scalar r1 = moments[0];
    const Scalar r2 = moments[1];

    const Scalar f1 = r1 - r1Obs;
    const Scalar f2 = r2 - r2Obs;

    if ((std::abs(f1) < absolutePrecision) && (std::abs(f2) < absolutePrecision))
      break;

    // Numerical Jacobian via central differences
    // Use optimal step for central differences: h = cbrt(eps) * max(1, |x|)
    const Scalar hStep = std::pow(epsilon, 1.0 / 3.0);
    const Scalar hKappa = hStep * std::max(1.0, std::abs(kappa));
    const Scalar hBeta  = hStep * std::max(1.0, std::abs(beta));

    Point momentsP(Kent::ComputeMoments(kappa + hKappa, beta));
    Point momentsM(Kent::ComputeMoments(kappa - hKappa, beta));
    const Scalar df1dk = (momentsP[0] - momentsM[0]) / (2.0 * hKappa);
    const Scalar df2dk = (momentsP[1] - momentsM[1]) / (2.0 * hKappa);

    momentsP = Kent::ComputeMoments(kappa, beta + hBeta);
    // beta - hBeta might be negative; use forward difference if beta is small
    Scalar df1db = 0.0;
    Scalar df2db = 0.0;
    if (beta > hBeta)
    {
      momentsM = Kent::ComputeMoments(kappa, beta - hBeta);
      df1db = (momentsP[0] - momentsM[0]) / (2.0 * hBeta);
      df2db = (momentsP[1] - momentsM[1]) / (2.0 * hBeta);
    }
    else
    {
      df1db = (momentsP[0] - r1) / hBeta;
      df2db = (momentsP[1] - r2) / hBeta;
    }

    // Solve 2x2 system: J * d = -f
    const Scalar det = df1dk * df2db - df1db * df2dk;
    if (std::abs(det) < epsilon)
      break;

    const Scalar dKappa = -(f1 * df2db - f2 * df1db) / det;
    const Scalar dBeta  = -(df1dk * f2 - df2dk * f1) / det;

    // Update with damping
    Scalar step = 1.0;
    for (UnsignedInteger ls = 0; ls < 10; ++ls)
    {
      const Scalar newKappa = kappa + step * dKappa;
      const Scalar newBeta  = beta  + step * dBeta;

      if ((newKappa > 0.0) && (newBeta >= 0.0) && (2.0 * newBeta < newKappa))
      {
        kappa = newKappa;
        beta  = newBeta;
        break;
      }
      step *= 0.5;
    }
    if (step < 1.0)
    {
      // Damped step was rejected; try a different direction
      break;
    }
  }

  // Final constraint enforcement
  if (!(kappa > 0.0)) kappa = 1.0;
  if (!(beta >= 0.0)) beta = 0.0;
  if (!(2.0 * beta < kappa)) beta = 0.25 * kappa;

  Kent result(kappa, beta, gamma);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

END_NAMESPACE_OPENTURNS
