//                                               -*- C++ -*-
/**
 *  @brief Factory for MatrixFisher distribution
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

#include "openturns/MatrixFisherFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Matrix.hxx"
#include <algorithm>
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MatrixFisherFactory)

static const Factory<MatrixFisherFactory> Factory_MatrixFisherFactory;

MatrixFisherFactory::MatrixFisherFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

MatrixFisherFactory * MatrixFisherFactory::clone() const
{
  return new MatrixFisherFactory(*this);
}

Distribution MatrixFisherFactory::build(const Sample & sample) const
{
  return buildAsMatrixFisher(sample).clone();
}

Distribution MatrixFisherFactory::build(const Point & parameters) const
{
  return buildAsMatrixFisher(parameters).clone();
}

Distribution MatrixFisherFactory::build() const
{
  return buildAsMatrixFisher().clone();
}

MatrixFisher MatrixFisherFactory::buildAsMatrixFisher(const Point & parameters) const
{
  try
  {
    MatrixFisher distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (const InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a MatrixFisher distribution from the given parameters";
  }
}

MatrixFisher MatrixFisherFactory::buildAsMatrixFisher() const
{
  return MatrixFisher();
}

/*
 * Estimate the MatrixFisher distribution parameters from a sample of rotation matrices
 * Based on the method of moments: match E[R] = U * diag(moments) * V^T
 */
MatrixFisher MatrixFisherFactory::buildAsMatrixFisher(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();

  if (size < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot build a MatrixFisher distribution from a sample of size < 3";
  if (sample.getDimension() != 9)
    throw InvalidArgumentException(HERE) << "Error: can build a MatrixFisher distribution only from a sample of dimension 9 (flattened 3x3), here dimension=" << sample.getDimension();

  // Step 1: compute mean matrix M = E[R]
  SquareMatrix M(3);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger r = 0; r < 3; ++r)
      for (UnsignedInteger c = 0; c < 3; ++c)
        M(r, c) += sample(i, r * 3 + c);
  }
  M = M * (1.0 / static_cast<Scalar>(size));

  // Step 2: SVD of M to get U, V and singular values
  Matrix U, vT;
  Matrix M_mat(M);
  Point s = M_mat.computeSVDInPlace(U, vT);
  SquareMatrix U_sq(U.getImplementation());
  SquareMatrix V(vT.getImplementation());
  V = V.transpose(); // V^T from SVD, we need V

  // Proper polar mode: the rotation closest to M. With D = diag(1, 1, det),
  // mode = U D V^T always lies in SO(3), with no column flip breaking the
  // factorization, including when det M < 0.
  SquareMatrix UVt = U_sq * V.transpose();
  const Scalar detUV = UVt.computeDeterminant();
  const Scalar orientation = (detUV < 0.0 ? -1.0 : 1.0);
  SquareMatrix mode(3);
  for (UnsignedInteger r = 0; r < 3; ++r)
    for (UnsignedInteger c = 0; c < 3; ++c)
    {
      mode(r, c) = U_sq(r, 0) * V(c, 0) + U_sq(r, 1) * V(c, 1) + orientation * U_sq(r, 2) * V(c, 2);
    }

  // Step 3: invert the moment equations E[R](f) = s by damped Newton-Raphson.
  // For the concentrations f, E[R] = mode * diag(d(f)) by rotation equivariance,
  // so only the diagonal moments in the mode frame are matched.
  // 0.5 is the standard threshold in the literature (Kato & Pennec 2018)
  // separating small concentration (s ~ kappa/3) from large concentration
  // (s ~ 1 - 1/(2*kappa)) approximations, used for the initial guess.
  const Scalar concentrationThreshold = 0.5;
  const Scalar maximumConcentration = ResourceMap::GetAsScalar("MatrixFisher-MaximumConcentration");
  Point concentration(3);
  for (UnsignedInteger i = 0; i < 3; ++i)
  {
    const Scalar si = std::max(SpecFunc::ScalarEpsilon, std::min(1.0 - SpecFunc::ScalarEpsilon, s[i]));
    if (si < concentrationThreshold)
      concentration[i] = 3.0 * si; // Small concentration: s ~ kappa/3
    else
      concentration[i] = 1.0 / (2.0 * (1.0 - si)); // Large concentration: s ~ 1 - 1/(2*kappa)
    concentration[i] = std::min(concentration[i], maximumConcentration);
  }

  // Extremely concentrated sample: every inverted concentration already
  // saturates the maximum, so the moment equations have no better finite
  // solution than the maximum concentration along the polar mode. Return it
  // directly, as the damped Newton iteration below could only stall at the
  // maximum while paying one full quadrature per candidate evaluation.
  Bool saturated = true;
  for (UnsignedInteger i = 0; i < 3; ++i)
    if (concentration[i] < maximumConcentration) saturated = false;
  if (saturated)
  {
    SquareMatrix F(3);
    for (UnsignedInteger i = 0; i < 3; ++i)
      F(i, i) = maximumConcentration;
    F = mode * F;
    MatrixFisher result(F);
    result.setDescription(sample.getDescription());
    adaptToKnownParameter(sample, &result);
    return result;
  }

  // Diagonal moments of a candidate: (mode^T E[R])_ii through the mean matrix
  auto diagonalMoments = [&](const Point & f)
  {
    SquareMatrix diagF(3);
    for (UnsignedInteger i = 0; i < 3; ++i)
      diagF(i, i) = f[i];
    const MatrixFisher candidate(mode * diagF);
    const Point meanFlat(candidate.getMean());
    SquareMatrix meanMatrix(3);
    for (UnsignedInteger r = 0; r < 3; ++r)
      for (UnsignedInteger c = 0; c < 3; ++c)
        meanMatrix(r, c) = meanFlat[r * 3 + c];
    const SquareMatrix pulled = mode.transpose() * meanMatrix;
    Point diagonal(3);
    for (UnsignedInteger i = 0; i < 3; ++i)
      diagonal[i] = pulled(i, i);
    return diagonal;
  };

  const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("MatrixFisherFactory-MaximumIterations");
  const Scalar residualPrecision = std::sqrt(SpecFunc::Precision);
  // Damped Newton-Raphson with Broyden rank-one Jacobian updates: each
  // candidate evaluation pays one full quadrature, so the finite-difference
  // Jacobian is evaluated once, then refreshed from the secant equation,
  // with a fresh finite-difference evaluation on stall.
  Point moments(3);
  Point residual(3);
  Scalar residualNorm = 0.0;
  Matrix jacobian(3, 3);
  Bool jacobianIsCurrent = false;
  Bool jacobianFromDifferences = false;
  Bool momentsAreCurrent = false;
  for (UnsignedInteger iteration = 0; iteration < maximumIterations; ++iteration)
  {
    if (!momentsAreCurrent)
    {
      moments = diagonalMoments(concentration);
      residualNorm = 0.0;
      for (UnsignedInteger i = 0; i < 3; ++i)
      {
        residual[i] = moments[i] - s[i];
        residualNorm = std::max(residualNorm, std::abs(residual[i]));
      }
    }
    momentsAreCurrent = false;
    if (residualNorm <= residualPrecision) break;
    if (!jacobianIsCurrent)
    {
      // Jacobian by central finite differences
      for (UnsignedInteger j = 0; j < 3; ++j)
      {
        const Scalar step = std::sqrt(SpecFunc::ScalarEpsilon) * std::max(1.0, std::abs(concentration[j]));
        Point fPlus(concentration);
        Point fMinus(concentration);
        fPlus[j] += step;
        fMinus[j] -= step;
        const Point momentsPlus = diagonalMoments(fPlus);
        const Point momentsMinus = diagonalMoments(fMinus);
        for (UnsignedInteger i = 0; i < 3; ++i)
          jacobian(i, j) = (momentsPlus[i] - momentsMinus[i]) / (2.0 * step);
      }
      jacobianIsCurrent = true;
      jacobianFromDifferences = true;
    }
    Point negativeResidual(3);
    for (UnsignedInteger i = 0; i < 3; ++i)
      negativeResidual[i] = -residual[i];
    const Point stepDirection(jacobian.solveLinearSystem(negativeResidual));
    // Damped update within the feasible concentrations, stop when stalled
    Scalar damping = 1.0;
    Bool improved = false;
    Point trial(concentration);
    Point trialMoments(3);
    Scalar trialNorm = residualNorm;
    for (UnsignedInteger halving = 0; halving < maximumIterations; ++halving)
    {
      for (UnsignedInteger i = 0; i < 3; ++i)
        trial[i] = std::min(maximumConcentration, concentration[i] + damping * stepDirection[i]);
      trialMoments = diagonalMoments(trial);
      trialNorm = 0.0;
      for (UnsignedInteger i = 0; i < 3; ++i)
        trialNorm = std::max(trialNorm, std::abs(trialMoments[i] - s[i]));
      if (trialNorm < residualNorm)
      {
        improved = true;
        break;
      }
      damping *= 0.5;
    }
    if (!improved)
    {
      // On stall, retry once with a fresh finite-difference Jacobian
      if (jacobianFromDifferences) break;
      jacobianIsCurrent = false;
      momentsAreCurrent = true;
      continue;
    }
    // Broyden rank-one update of the Jacobian from the secant equation
    const Point secantStep(trial - concentration);
    const Point secantChange(trialMoments - moments);
    if (secantStep.dot(secantStep) > SpecFunc::ScalarEpsilon)
    {
      Point predictedChange(3, 0.0);
      for (UnsignedInteger i = 0; i < 3; ++i)
        for (UnsignedInteger j = 0; j < 3; ++j)
          predictedChange[i] += jacobian(i, j) * secantStep[j];
      const Scalar curvature = secantStep.dot(secantStep);
      for (UnsignedInteger i = 0; i < 3; ++i)
        for (UnsignedInteger j = 0; j < 3; ++j)
          jacobian(i, j) += (secantChange[i] - predictedChange[i]) * secantStep[j] / curvature;
      jacobianFromDifferences = false;
    }
    concentration = trial;
    moments = trialMoments;
    residualNorm = trialNorm;
    for (UnsignedInteger i = 0; i < 3; ++i)
      residual[i] = moments[i] - s[i];
    momentsAreCurrent = true;
  }

  // Canonical form: sort the concentrations in descending order, permuting
  // the mode columns jointly so the estimated matrix is unchanged
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = i + 1; j < 3; ++j)
    {
      if (concentration[j] > concentration[i])
      {
        std::swap(concentration[i], concentration[j]);
        for (UnsignedInteger r = 0; r < 3; ++r)
          std::swap(mode(r, i), mode(r, j));
      }
    }

  // Build F = mode * diag(concentration)
  SquareMatrix F_diag_mat(3);
  F_diag_mat(0, 0) = concentration[0];
  F_diag_mat(1, 1) = concentration[1];
  F_diag_mat(2, 2) = concentration[2];
  SquareMatrix F = mode * F_diag_mat;

  MatrixFisher result(F);
  result.setDescription(sample.getDescription());
  adaptToKnownParameter(sample, &result);
  return result;
}

END_NAMESPACE_OPENTURNS