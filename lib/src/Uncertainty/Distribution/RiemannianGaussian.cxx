//                                               -*- C++ -*-
/**
 *  @brief The Riemannian Gaussian distribution on SPD manifold
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

#include <cmath>
#include <algorithm>
#include "openturns/RiemannianGaussian.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(RiemannianGaussian)

static const Factory<RiemannianGaussian> Factory_RiemannianGaussian;

RiemannianGaussian::RiemannianGaussian()
  : DistributionImplementation()
  , dimension_(3)
  , n_(2)
  , meanMatrix_(2)
  , sigma_(3)
  , epsilon_(ResourceMap::GetAsScalar("RiemannianGaussian-PositiveDefiniteThreshold"))
  , logNormalization_(0.0)
  , sigmaInv_(3)
  , sigmaDet_(1.0)
{
  meanMatrix_(0, 0) = 1.0;
  meanMatrix_(1, 1) = 1.0;
  meanMatrix_(0, 1) = 0.0;

  sigma_(0, 0) = 1.0;
  sigma_(1, 1) = 1.0;
  sigma_(2, 2) = 1.0;

  setName("RiemannianGaussian");
  setDimension(3);
  computeNormalization();
  updateSampler();
  computeRange();
}

RiemannianGaussian::RiemannianGaussian(const SymmetricMatrix & mean,
                                       const SquareMatrix & sigma,
                                       const Scalar epsilon)
  : DistributionImplementation()
  , dimension_(mean.getDimension() * (mean.getDimension() + 1) / 2)
  , n_(mean.getDimension())
  , meanMatrix_(mean)
  , sigma_(sigma.getDimension())
  , epsilon_(std::max(SpecFunc::ScalarEpsilon, epsilon))
  , logNormalization_(0.0)
  , sigmaInv_(sigma.getDimension())
  , sigmaDet_(1.0)
{
  const UnsignedInteger n = n_;
  const UnsignedInteger d = dimension_;

  if (n < 2)
    throw InvalidArgumentException(HERE) << "in RiemannianGaussian: matrix dimension must be >= 2, got n=" << n;
  if (sigma.getDimension() != d)
    throw InvalidArgumentException(HERE) << "in RiemannianGaussian: sigma must be of dimension " << d << ", got dimension=" << sigma.getDimension();

  // Check mean is positive definite
  SymmetricMatrix meanCheck(meanMatrix_);
  SquareMatrix meanEigVec(n);
  const Point meanEig = meanCheck.computeEVInPlace(meanEigVec);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    if (meanEig[i] <= epsilon_)
      throw InvalidArgumentException(HERE) << "in RiemannianGaussian: mean must be positive definite";
  }

  // Check sigma is positive definite
  // sigma is symmetric (covariance), convert to SymmetricMatrix for eigendecomposition
  SymmetricMatrix sigma_sym(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      sigma_sym(i, j) = sigma(i, j);
  SquareMatrix sigmaEigVec(d);
  const Point sigmaEig = sigma_sym.computeEVInPlace(sigmaEigVec);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (sigmaEig[i] <= epsilon_)
      throw InvalidArgumentException(HERE) << "in RiemannianGaussian: sigma must be positive definite";
  }

  sigma_ = sigma;
  setName("RiemannianGaussian");
  setDimension(d);
  computeNormalization();
  updateSampler();
  computeRange();
}

Bool RiemannianGaussian::operator ==(const RiemannianGaussian & other) const
{
  if (this == &other) return true;
  return (n_ == other.n_)
      && (meanMatrix_ == other.meanMatrix_)
      && (sigma_ == other.sigma_);
}

Bool RiemannianGaussian::equals(const DistributionImplementation & other) const
{
  const RiemannianGaussian* p_other = dynamic_cast<const RiemannianGaussian*>(&other);
  return p_other && (*this == *p_other);
}

String RiemannianGaussian::__repr__() const
{
  OSS oss(true);
  oss << "class=" << RiemannianGaussian::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mean=" << meanMatrix_
      << " sigma=" << sigma_;
  return oss;
}

String RiemannianGaussian::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mean = " << meanMatrix_ << ", sigma = " << sigma_ << ")";
  return oss;
}

RiemannianGaussian * RiemannianGaussian::clone() const
{
  return new RiemannianGaussian(*this);
}

void RiemannianGaussian::computeRange()
{
  Point lower(dimension_, -SpecFunc::MaxScalar);
  Point upper(dimension_, SpecFunc::MaxScalar);
  setRange(Interval(lower, upper));
}

SymmetricMatrix RiemannianGaussian::logMap(const SymmetricMatrix & x) const
{
  // Log map on SPD: log_mu(x) = mu^{1/2} * log(mu^{-1/2} * x * mu^{-1/2}) * mu^{1/2}
  // Using eigendecomposition for numerical stability

  // The eigenvectors are stored as the columns of the eigenvector matrix:
  // mean = V * diag(meanEig) * V^T. The spectral reconstruction below
  // therefore scales each column k of V by the value f(meanEig[k]).
  auto reconstruct = [&](const SquareMatrix & eigVec, const Point & fEig)
  {
    SymmetricMatrix result(n_);
    for (UnsignedInteger i = 0; i < n_; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        Scalar value = 0.0;
        for (UnsignedInteger k = 0; k < n_; ++k)
          value += eigVec(i, k) * fEig[k] * eigVec(j, k);
        result(i, j) = value;
      }
    return result;
  };

  // Compute mu^{-1/2} and mu^{1/2} - need to make a copy since
  // computeEVInPlace modifies the matrix
  SymmetricMatrix meanCopy(meanMatrix_);
  SquareMatrix meanEigVec(n_);
  const Point meanEig = meanCopy.computeEVInPlace(meanEigVec);

  Point invSqrt(n_);
  Point sqrtEig(n_);
  for (UnsignedInteger k = 0; k < n_; ++k)
  {
    sqrtEig[k] = std::sqrt(meanEig[k]);
    invSqrt[k] = 1.0 / sqrtEig[k];
  }
  const SymmetricMatrix meanInvSqrt = reconstruct(meanEigVec, invSqrt);
  const SymmetricMatrix meanSqrt = reconstruct(meanEigVec, sqrtEig);

  // Compute C = mu^{-1/2} * x * mu^{-1/2}, a symmetric positive definite matrix
  SquareMatrix CSq = meanInvSqrt * x * meanInvSqrt;
  SymmetricMatrix C(n_);
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      C(i, j) = CSq(i, j);

  // Log of C through its eigendecomposition
  SquareMatrix cEigVec(n_);
  const Point cEig = C.computeEVInPlace(cEigVec);

  Point logEig(n_);
  for (UnsignedInteger k = 0; k < n_; ++k)
    logEig[k] = std::log(std::max(epsilon_, cEig[k]));
  const SymmetricMatrix logC = reconstruct(cEigVec, logEig);

  // log_mu(x) = mu^{1/2} * log(C) * mu^{1/2}
  SquareMatrix resultSq = meanSqrt * logC * meanSqrt;
  SymmetricMatrix result(n_);
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      result(i, j) = resultSq(i, j);
  return result;
}

SymmetricMatrix RiemannianGaussian::expMap(const SymmetricMatrix & v) const
{
  // Exp map on SPD: exp_mu(v) = mu^{1/2} * exp(mu^{-1/2} * v * mu^{-1/2}) * mu^{1/2}

  // The eigenvectors are stored as the columns of the eigenvector matrix:
  // see the spectral reconstruction lambda in logMap for the convention.
  auto reconstruct = [&](const SquareMatrix & eigVec, const Point & fEig)
  {
    SymmetricMatrix result(n_);
    for (UnsignedInteger i = 0; i < n_; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        Scalar value = 0.0;
        for (UnsignedInteger k = 0; k < n_; ++k)
          value += eigVec(i, k) * fEig[k] * eigVec(j, k);
        result(i, j) = value;
      }
    return result;
  };

  // Compute mu^{-1/2} and mu^{1/2} - need to make a copy since
  // computeEVInPlace modifies the matrix
  SymmetricMatrix meanCopy(meanMatrix_);
  SquareMatrix meanEigVec(n_);
  const Point meanEig = meanCopy.computeEVInPlace(meanEigVec);

  Point invSqrt(n_);
  Point sqrtEig(n_);
  for (UnsignedInteger k = 0; k < n_; ++k)
  {
    sqrtEig[k] = std::sqrt(meanEig[k]);
    invSqrt[k] = 1.0 / sqrtEig[k];
  }
  const SymmetricMatrix meanInvSqrt = reconstruct(meanEigVec, invSqrt);
  const SymmetricMatrix meanSqrt = reconstruct(meanEigVec, sqrtEig);

  // Compute C = mu^{-1/2} * v * mu^{-1/2}
  SquareMatrix CSq = meanInvSqrt * v * meanInvSqrt;
  SymmetricMatrix C(n_);
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      C(i, j) = CSq(i, j);

  // Exp of C through its eigendecomposition
  SquareMatrix cEigVec(n_);
  const Point cEig = C.computeEVInPlace(cEigVec);

  Point expEig(n_);
  for (UnsignedInteger k = 0; k < n_; ++k)
    expEig[k] = std::exp(cEig[k]);
  const SymmetricMatrix expC = reconstruct(cEigVec, expEig);

  // exp_mu(v) = mu^{1/2} * exp(C) * mu^{1/2}
  SquareMatrix resultSq = meanSqrt * expC * meanSqrt;
  SymmetricMatrix result(n_);
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      result(i, j) = resultSq(i, j);
  return result;
}

Scalar RiemannianGaussian::computeLogExpJacobian(const SymmetricMatrix & v) const
{
  // Compute log-Jacobian determinant of the exponential map at v
  // The eigenvalues are those of mu^{-1/2} * v * mu^{-1/2}
  // Formula from the literature (see e.g. Pennec's work on SPD manifolds)

  // The eigenvectors are stored as the columns of the eigenvector matrix
  auto reconstruct = [&](const SquareMatrix & eigVec, const Point & fEig)
  {
    SymmetricMatrix result(n_);
    for (UnsignedInteger i = 0; i < n_; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        Scalar value = 0.0;
        for (UnsignedInteger k = 0; k < n_; ++k)
          value += eigVec(i, k) * fEig[k] * eigVec(j, k);
        result(i, j) = value;
      }
    return result;
  };

  // Compute mu^{-1/2} - need to make a copy since computeEVInPlace modifies the matrix
  SymmetricMatrix meanCopy(meanMatrix_);
  SquareMatrix meanEigVec(n_);
  const Point meanEig = meanCopy.computeEVInPlace(meanEigVec);

  Point invSqrt(n_);
  for (UnsignedInteger k = 0; k < n_; ++k)
    invSqrt[k] = 1.0 / std::sqrt(meanEig[k]);
  const SymmetricMatrix meanInvSqrt = reconstruct(meanEigVec, invSqrt);

  // Compute C = mu^{-1/2} * v * mu^{-1/2}
  SquareMatrix CSq = meanInvSqrt * v * meanInvSqrt;
  SymmetricMatrix C(n_);
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      C(i, j) = CSq(i, j);

  // Eigenvalues of C
  SquareMatrix cEigVec(n_);
  const Point cEig = C.computeEVInPlace(cEigVec);

  // Log-Jacobian formula
  // log|det(d exp_mu(v))| = sum_i cEig[i] + sum_{i<j} g(cEig[i], cEig[j])
  // where g(a, b) = max(a,b) - log|a-b| + log(1 - exp(-|a-b|)) for a != b
  // and g(a, a) = a + (a^2)/24 - (a^4)/2880 (limit case)
  Scalar logJac = 0.0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    logJac += cEig[i];

  for (UnsignedInteger i = 0; i < n_; ++i)
  {
    for (UnsignedInteger j = i + 1; j < n_; ++j)
    {
      const Scalar diff = cEig[i] - cEig[j];
      const Scalar ad = std::abs(diff);
      if (ad < 1e-4)
      {
        const Scalar avg = 0.5 * (cEig[i] + cEig[j]);
        logJac += avg + diff * diff / 24.0 - std::pow(diff, 4) / 2880.0;
      }
      else
      {
        logJac += std::max(cEig[i], cEig[j]) - std::log(ad) + std::log1p(-std::exp(-ad));
      }
    }
  }

  return logJac;
}

void RiemannianGaussian::computeNormalization()
{
  const UnsignedInteger d = dimension_;

  // Compute log determinant of sigma
  // sigma_ is symmetric, use SymmetricMatrix for eigendecomposition
  SymmetricMatrix sigma_sym(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      sigma_sym(i, j) = sigma_(i, j);
  SquareMatrix sigmaEigVec(d);
  const Point sigmaEig = sigma_sym.computeEVInPlace(sigmaEigVec);

  Scalar logDetSigma = 0.0;
  for (UnsignedInteger i = 0; i < d; ++i)
    logDetSigma += std::log(sigmaEig[i]);
  sigmaDet_ = std::exp(logDetSigma);

  // Inverse of sigma
  sigmaInv_ = sigma_.inverse();

  // Log normalization of the Gaussian density in exp-normal coordinates:
  // the density of the image of a centered tangent-space Gaussian through the
  // exponential map is the Gaussian density evaluated at the log-map image.
  logNormalization_ = 0.5 * d * std::log(2.0 * M_PI) + 0.5 * logDetSigma;
}

void RiemannianGaussian::computeMean() const
{
  // The Frechet mean of the wrapped tangent Gaussian is the exponential of
  // the zero tangent vector, i.e. the mean SPD matrix itself
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = i; j < n_; ++j)
      mean_[idx++] = meanMatrix_(i, j);
  isAlreadyComputedMean_ = true;
}

void RiemannianGaussian::computeCovariance() const
{
  // The covariance is measured in the tangent space at the mean, i.e. it is
  // the covariance of the unwrapped Gaussian
  CovarianceMatrix cov(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      cov(i, j) = sigma_(i, j);
  covariance_ = cov;
  isAlreadyComputedCovariance_ = true;
}

void RiemannianGaussian::updateSampler()
{
  // Cholesky decomposition of sigma for sampling in tangent space
  // Use CovarianceMatrix for Cholesky
  CovarianceMatrix sigma_cov(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      sigma_cov(i, j) = sigma_(i, j);
  TriangularMatrix chol = sigma_cov.computeCholeskyInPlace();
  // Note: chol is lower triangular L such that L * L^T = sigma
  // For sampling we need to generate z ~ N(0,I) then v = L * z
  // But we'll use eigendecomposition for correlated sampling instead
}

Point RiemannianGaussian::getRealization() const
{
  const UnsignedInteger d = dimension_;

  // Sample from N(0, sigma) in tangent space using eigendecomposition
  // This is more stable than Cholesky for near-singular matrices
  SymmetricMatrix sigma_sym(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      sigma_sym(i, j) = sigma_(i, j);
  SquareMatrix sigmaEigVec(d);
  const Point sigmaEig = sigma_sym.computeEVInPlace(sigmaEigVec);

  Point z(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    z[i] = DistFunc::rNormal();

  Point v(d);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    v[i] = 0.0;
    for (UnsignedInteger j = 0; j < d; ++j)
      v[i] += sigmaEigVec(i, j) * std::sqrt(sigmaEig[j]) * z[j];
  }

  // Convert vector to symmetric matrix
  SymmetricMatrix vMat(n_);
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = i; j < n_; ++j)
    {
      vMat(i, j) = v[idx++];
    }

  // Map to SPD manifold using exp map
  SymmetricMatrix sample = expMap(vMat);

  // Flatten to Point
  Point result(d);
  idx = 0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = i; j < n_; ++j)
    {
      result[idx++] = sample(i, j);
    }

  return result;
}

Scalar RiemannianGaussian::computePDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();

  const Scalar logPDF = computeLogPDF(point);
  return std::exp(logPDF);
}

Scalar RiemannianGaussian::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();

  // Convert point to symmetric matrix
  SymmetricMatrix x(n_);
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = i; j < n_; ++j)
    {
      x(i, j) = point[idx++];
    }

  // Check positive definiteness
  SquareMatrix xEigVec(n_);
  const Point xEig = x.computeEVInPlace(xEigVec);
  for (UnsignedInteger i = 0; i < n_; ++i)
  {
    if (xEig[i] <= epsilon_)
      return -SpecFunc::Infinity;
  }

  // Compute log map
  SymmetricMatrix v = logMap(x);

  // Flatten v to vector
  Point vVec(dimension_);
  idx = 0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = i; j < n_; ++j)
    {
      vVec[idx++] = v(i, j);
    }

  // Quadratic form: -0.5 * v^T * sigma^{-1} * v
  Scalar quadForm = 0.0;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      quadForm += vVec[i] * sigmaInv_(i, j) * vVec[j];

  // Log-Jacobian of the exponential map
  const Scalar logJac = computeLogExpJacobian(v);

  return -0.5 * quadForm - logNormalization_ - logJac;
}

Point RiemannianGaussian::getParameter() const
{
  const UnsignedInteger n = n_;
  const UnsignedInteger d = dimension_;
  Point point(n * (n + 1) / 2 + d * d);

  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = i; j < n; ++j)
      point[idx++] = meanMatrix_(i, j);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
      point[idx++] = sigma_(i, j);

  return point;
}

void RiemannianGaussian::setParameter(const Point & parameter)
{
  const UnsignedInteger n = n_;
  const UnsignedInteger d = dimension_;
  if (parameter.getSize() != n * (n + 1) / 2 + d * d)
    throw InvalidArgumentException(HERE) << "Error: expected " << (n * (n + 1) / 2 + d * d) << " values, got " << parameter.getSize();

  SymmetricMatrix mean(n);
  SquareMatrix sigma(d);
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = i; j < n; ++j)
      mean(i, j) = parameter[idx++];
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
      sigma(i, j) = parameter[idx++];

  const Scalar w = getWeight();
  *this = RiemannianGaussian(mean, sigma, epsilon_);
  setWeight(w);
}

Description RiemannianGaussian::getParameterDescription() const
{
  const UnsignedInteger n = n_;
  const UnsignedInteger d = dimension_;
  Description description(n * (n + 1) / 2 + d * d);

  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = i; j < n; ++j)
    {
      OSS oss;
      oss << "mean_" << i << "_" << j;
      description[idx++] = oss.str();
    }
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
    {
      OSS oss;
      oss << "sigma_" << i << "_" << j;
      description[idx++] = oss.str();
    }
  return description;
}

void RiemannianGaussian::setMean(const SymmetricMatrix & mean)
{
  const UnsignedInteger n = n_;
  if (mean.getDimension() != n)
    throw InvalidArgumentException(HERE) << "in RiemannianGaussian::setMean: mean must be of dimension " << n << ", got dimension=" << mean.getDimension();

  // Make a copy for eigendecomposition (computeEVInPlace modifies the matrix)
  SymmetricMatrix meanCopy(mean);
  SquareMatrix meanEigVec(n);
  const Point eigenValues = meanCopy.computeEVInPlace(meanEigVec);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    if (eigenValues[i] <= epsilon_)
      throw InvalidArgumentException(HERE) << "in RiemannianGaussian::setMean: mean must be positive definite";
  }

  if (mean != meanMatrix_)
  {
    meanMatrix_ = mean;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
    updateSampler();
    computeRange();
  }
}

SymmetricMatrix RiemannianGaussian::getMeanMatrix() const
{
  return meanMatrix_;
}

Point RiemannianGaussian::getMean() const
{
  // Flatten the symmetric matrix to a Point
  Point result(dimension_);
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    for (UnsignedInteger j = i; j < n_; ++j)
      result[idx++] = meanMatrix_(i, j);
  return result;
}

void RiemannianGaussian::setSigma(const SquareMatrix & sigma)
{
  const UnsignedInteger d = dimension_;
  if (sigma.getDimension() != d)
    throw InvalidArgumentException(HERE) << "in RiemannianGaussian::setSigma: sigma must be of dimension " << d << ", got dimension=" << sigma.getDimension();

  SymmetricMatrix sigma_sym(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      sigma_sym(i, j) = sigma(i, j);
  SquareMatrix sigmaEigVec(d);
  const Point sigmaEig = sigma_sym.computeEVInPlace(sigmaEigVec);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (sigmaEig[i] <= epsilon_)
      throw InvalidArgumentException(HERE) << "in RiemannianGaussian::setSigma: sigma must be positive definite";
  }

  if (sigma != sigma_)
  {
    sigma_ = sigma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
    updateSampler();
  }
}

SquareMatrix RiemannianGaussian::getSigma() const
{
  return sigma_;
}

Scalar RiemannianGaussian::getEpsilon() const
{
  return epsilon_;
}

Scalar RiemannianGaussian::computeEntropy() const
{
  const UnsignedInteger d = dimension_;
  return 0.5 * d * (1.0 + std::log(2.0 * M_PI)) + 0.5 * std::log(sigmaDet_);
}

Bool RiemannianGaussian::isContinuous() const
{
  return true;
}

void RiemannianGaussian::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("n_", n_);
  adv.saveAttribute("meanMatrix_", meanMatrix_);
  adv.saveAttribute("sigma_", sigma_);
  adv.saveAttribute("epsilon_", epsilon_);
  adv.saveAttribute("logNormalization_", logNormalization_);
  adv.saveAttribute("sigmaInv_", sigmaInv_);
  adv.saveAttribute("sigmaDet_", sigmaDet_);
}

void RiemannianGaussian::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("n_", n_);
  adv.loadAttribute("meanMatrix_", meanMatrix_);
  adv.loadAttribute("sigma_", sigma_);
  adv.loadAttribute("epsilon_", epsilon_);
  adv.loadAttribute("logNormalization_", logNormalization_);
  adv.loadAttribute("sigmaInv_", sigmaInv_);
  adv.loadAttribute("sigmaDet_", sigmaDet_);
  dimension_ = n_ * (n_ + 1) / 2;
  computeRange();
}

END_NAMESPACE_OPENTURNS