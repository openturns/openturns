//                                               -*- C++ -*-
/**
 *  @brief The MatrixFisher distribution on SO(3)
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
#include "openturns/MatrixFisher.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/CovarianceMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MatrixFisher)

static const Factory<MatrixFisher> Factory_MatrixFisher;

MatrixFisher::MatrixFisher()
  : DistributionImplementation()
  , F_(3)
  , U_(3)
  , V_(3)
  , singularValues_(3)
  , epsilon_(ResourceMap::GetAsScalar("MatrixFisher-OrthogonalityThreshold"))
  , logNormalization_(0.0)
  , maxTrace_(0.0)
  , expectedMatrix_(3)
  , expectedSquaredMatrix_(3)
  , expectedTrace_(0.0)
{
  F_(0, 0) = 1.0;
  F_(1, 1) = 1.0;
  F_(2, 2) = 1.0;
  setName("MatrixFisher");
  setDimension(9); // 3x3 matrix flattened
  computeNormalization();
  updateSampler();
  computeRange();
}

MatrixFisher::MatrixFisher(const SquareMatrix & F)
  : DistributionImplementation()
  , F_(F.getDimension())
  , U_(F.getDimension())
  , V_(F.getDimension())
  , singularValues_(F.getDimension())
  , epsilon_(ResourceMap::GetAsScalar("MatrixFisher-OrthogonalityThreshold"))
  , logNormalization_(0.0)
  , maxTrace_(0.0)
  , expectedMatrix_(3)
  , expectedSquaredMatrix_(3)
  , expectedTrace_(0.0)
{
  const UnsignedInteger n = F.getDimension();
  if (n != 3)
    throw InvalidArgumentException(HERE) << "in MatrixFisher: F must be 3x3, got dimension=" << n;

  F_ = F;
  setName("MatrixFisher");
  setDimension(9); // Flattened 3x3
  computeNormalization();
  updateSampler();
  computeRange();
}

Bool MatrixFisher::operator ==(const MatrixFisher & other) const
{
  if (this == &other) return true;
  return (F_ == other.F_)
      && (U_ == other.U_)
      && (V_ == other.V_)
      && (singularValues_ == other.singularValues_);
}

Bool MatrixFisher::equals(const DistributionImplementation & other) const
{
  const MatrixFisher* p_other = dynamic_cast<const MatrixFisher*>(&other);
  return p_other && (*this == *p_other);
}

String MatrixFisher::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MatrixFisher::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " F=" << F_;
  return oss;
}

String MatrixFisher::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(F = " << F_ << ")";
  return oss;
}

MatrixFisher * MatrixFisher::clone() const
{
  return new MatrixFisher(*this);
}

void MatrixFisher::computeRange()
{
  // SO(3) matrices have elements in [-1, 1]
  Point lower(9, -1.0);
  Point upper(9, 1.0);
  setRange(Interval(lower, upper));
}

void MatrixFisher::computeNormalization()
{
  // The normalization constant a_0(F) = int_{SO(3)} exp(tr(F^T R)) dR is
  // computed by tensorized Gauss-Legendre quadrature over the ZYZ Euler
  // angles (phi, theta, psi), for which the normalized Haar measure reads
  // (1/8 pi^2) sin(theta) dphi dtheta dpsi with phi, psi in [0, 2 pi] and
  // theta in [0, pi]. The trace cannot exceed the sum of the singular values
  // of F, so we integrate exp(tr - maxTrace) <= 1 to avoid overflows and
  // exponentiate the maximum back at the end.
  const UnsignedInteger order = ResourceMap::GetAsUnsignedInteger("MatrixFisher-QuadratureOrder");
  const GaussLegendre quadrature(Indices(3, order));
  const Sample nodes(quadrature.getNodes());
  const Point weights(quadrature.getWeights());
  const UnsignedInteger size = weights.getSize();

  // Rotation R = Rz(phi) Ry(theta) Rz(psi); nodes are in [0, 1]^3
  auto buildRotation = [&](const Scalar tPhi, const Scalar tTheta, const Scalar tPsi)
  {
    const Scalar phi = 2.0 * M_PI * tPhi;
    const Scalar theta = M_PI * tTheta;
    const Scalar psi = 2.0 * M_PI * tPsi;
    const Scalar cPhi = std::cos(phi), sPhi = std::sin(phi);
    const Scalar cTheta = std::cos(theta), sTheta = std::sin(theta);
    const Scalar cPsi = std::cos(psi), sPsi = std::sin(psi);
    SquareMatrix R(3);
    // R = Rz(phi) * Ry(theta) * Rz(psi)
    R(0, 0) = cPhi * cTheta * cPsi - sPhi * sPsi;
    R(0, 1) = -cPhi * cTheta * sPsi - sPhi * cPsi;
    R(0, 2) = cPhi * sTheta;
    R(1, 0) = sPhi * cTheta * cPsi + cPhi * sPsi;
    R(1, 1) = -sPhi * cTheta * sPsi + cPhi * cPsi;
    R(1, 2) = sPhi * sTheta;
    R(2, 0) = -sTheta * cPsi;
    R(2, 1) = sTheta * sPsi;
    R(2, 2) = cTheta;
    return R;
  };

  // Moments of the singular values of F
  Matrix U, vT;
  Matrix F_mat(F_);
  const Point sigma = F_mat.computeSVDInPlace(U, vT);
  const Scalar maxTrace = sigma[0] + sigma[1] + sigma[2];

  Scalar sumWeighted = 0.0;
  Scalar sumTrWeighted = 0.0;
  Matrix sumRWeighted(3, 3);
  Matrix sumRFlatRFlatWeighted(9, 9);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar tPhi = nodes(i, 0);
    const Scalar tTheta = nodes(i, 1);
    const Scalar tPsi = nodes(i, 2);
    const SquareMatrix R(buildRotation(tPhi, tTheta, tPsi));
    Scalar trace = 0.0;
    for (UnsignedInteger r = 0; r < 3; ++r)
      for (UnsignedInteger c = 0; c < 3; ++c)
        trace += F_(r, c) * R(r, c);
    // Haar weight: sin(theta); the [0,1]^3 -> Euler box map has Jacobian 4 pi^3
    const Scalar sinTheta = std::sin(M_PI * tTheta);
    const Scalar weight = weights[i] * sinTheta * std::exp(trace - maxTrace);
    sumWeighted += weight;
    sumTrWeighted += weight * trace;
    for (UnsignedInteger r = 0; r < 3; ++r)
      for (UnsignedInteger c = 0; c < 3; ++c)
      {
        sumRWeighted(r, c) += weight * R(r, c);
        const UnsignedInteger idx = 3 * r + c;
        for (UnsignedInteger r2 = 0; r2 < 3; ++r2)
          for (UnsignedInteger c2 = 0; c2 < 3; ++c2)
            sumRFlatRFlatWeighted(idx, 3 * r2 + c2) += weight * R(r, c) * R(r2, c2);
      }
  }

  const Scalar integral = 4.0 * std::pow(M_PI, 3) * sumWeighted;
  logNormalization_ = maxTrace + std::log(integral);
  expectedTrace_ = (4.0 * std::pow(M_PI, 3) * sumTrWeighted) / integral;
  const Matrix scaledR((4.0 * std::pow(M_PI, 3) * sumRWeighted) / integral);
  expectedMatrix_ = SquareMatrix(scaledR.getImplementation());
  const Matrix scaledRFlatRFlat((4.0 * std::pow(M_PI, 3) * sumRFlatRFlatWeighted) / integral);
  expectedSquaredMatrix_ = SquareMatrix(scaledRFlatRFlat.getImplementation());
}

void MatrixFisher::updateSampler()
{
  // Compute SVD of F: F = U * diag(sigma) * V^T
  // Use OpenTURNS Matrix::computeSVDInPlace
  Matrix U, vT;
  // F_ is SquareMatrix, need to convert to Matrix for computeSVD
  Matrix F_mat(F_);
  singularValues_ = F_mat.computeSVDInPlace(U, vT);
  U_ = SquareMatrix(U.getImplementation());
  // V is the transpose of vT
  V_ = SquareMatrix(vT.getImplementation()).transpose();

  // Sort singular values in descending order and corresponding vectors
  // (SVD typically returns them in descending order already)
  // But we need to ensure U and V are in SO(3) (det = +1)
  Scalar detU = U_.computeDeterminant();
  Scalar detV = V_.computeDeterminant();
  if (detU < 0.0)
  {
    for (UnsignedInteger j = 0; j < 3; ++j)
      U_(j, 2) = -U_(j, 2);
  }
  if (detV < 0.0)
  {
    for (UnsignedInteger j = 0; j < 3; ++j)
      V_(j, 2) = -V_(j, 2);
  }

  // Maximum of tr(F^T R) = sum of the singular values of F
  maxTrace_ = singularValues_[0] + singularValues_[1] + singularValues_[2];
}

SquareMatrix MatrixFisher::sampleUniformRotation() const
{
  // Uniform element of SO(3): sample a unit quaternion uniformly on the
  // three-sphere (normalized standard Gaussian vector) and convert it.
  Point q(4);
  Scalar norm = 0.0;
  for (UnsignedInteger i = 0; i < 4; ++i)
  {
    q[i] = DistFunc::rNormal();
    norm += q[i] * q[i];
  }
  norm = std::sqrt(norm);
  for (UnsignedInteger i = 0; i < 4; ++i)
    q[i] /= norm;

  const Scalar q0 = q[0];
  const Scalar q1 = q[1];
  const Scalar q2 = q[2];
  const Scalar q3 = q[3];
  SquareMatrix R(3);
  R(0, 0) = 1.0 - 2.0 * (q2 * q2 + q3 * q3);
  R(0, 1) = 2.0 * (q1 * q2 - q0 * q3);
  R(0, 2) = 2.0 * (q1 * q3 + q0 * q2);
  R(1, 0) = 2.0 * (q1 * q2 + q0 * q3);
  R(1, 1) = 1.0 - 2.0 * (q1 * q1 + q3 * q3);
  R(1, 2) = 2.0 * (q2 * q3 - q0 * q1);
  R(2, 0) = 2.0 * (q1 * q3 - q0 * q2);
  R(2, 1) = 2.0 * (q2 * q3 + q0 * q1);
  R(2, 2) = 1.0 - 2.0 * (q1 * q1 + q2 * q2);
  return R;
}

Point MatrixFisher::getRealization() const
{
  // Rejection sampling from the uniform distribution on SO(3): the density
  // of the Matrix Fisher law is bounded by exp(maxTrace), so the acceptance
  // ratio reads exp(tr(F^T R) - maxTrace). This is statistically exact but
  // becomes inefficient for large concentration parameters.
  for (;;)
  {
    const SquareMatrix R(sampleUniformRotation());
    Scalar trace = 0.0;
    for (UnsignedInteger i = 0; i < 3; ++i)
      for (UnsignedInteger j = 0; j < 3; ++j)
        trace += F_(i, j) * R(i, j);
    if (RandomGenerator::Generate() < std::exp(trace - maxTrace_))
    {
      Point result(9);
      for (UnsignedInteger i = 0; i < 3; ++i)
        for (UnsignedInteger j = 0; j < 3; ++j)
          result[i * 3 + j] = R(i, j);
      return result;
    }
  }
}

Scalar MatrixFisher::computePDF(const Point & point) const
{
  if (point.getDimension() != 9)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=9, here dimension=" << point.getDimension();

  // Convert point to 3x3 matrix
  SquareMatrix R(3);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      R(i, j) = point[i * 3 + j];

  // Check orthogonality and det = 1
  SquareMatrix I = R * R.transpose();
  if ((I - IdentityMatrix(3)).frobeniusNorm() > epsilon_)
    return 0.0;
  if (std::abs(R.computeDeterminant() - 1.0) > epsilon_)
    return 0.0;

  return std::exp(computeLogPDF(point));
}

Scalar MatrixFisher::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 9)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=9, here dimension=" << point.getDimension();

  SquareMatrix R(3);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      R(i, j) = point[i * 3 + j];

  // Check orthogonality and det = 1
  SquareMatrix I = R * R.transpose();
  if ((I - IdentityMatrix(3)).frobeniusNorm() > epsilon_)
    return -SpecFunc::Infinity;
  if (std::abs(R.computeDeterminant() - 1.0) > epsilon_)
    return -SpecFunc::Infinity;

  // log p(R) = tr(F^T R) - log a_0(F)
  Scalar trace = 0.0;
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      trace += F_(i, j) * R(i, j);

  return trace - logNormalization_;
}

Point MatrixFisher::getParameter() const
{
  Point point(9);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      point[i * 3 + j] = F_(i, j);
  return point;
}

void MatrixFisher::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 9)
    throw InvalidArgumentException(HERE) << "Error: expected 9 values, got " << parameter.getSize();

  SquareMatrix F(3);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      F(i, j) = parameter[i * 3 + j];

  const Scalar w = getWeight();
  const Scalar epsilon = epsilon_;
  *this = MatrixFisher(F);
  setWeight(w);
  setEpsilon(epsilon);
}

Description MatrixFisher::getParameterDescription() const
{
  Description description(9);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
    {
      OSS oss;
      oss << "F_" << i << "_" << j;
      description[i * 3 + j] = oss.str();
    }
  return description;
}

void MatrixFisher::setF(const SquareMatrix & F)
{
  const UnsignedInteger n = F.getDimension();
  if (n != 3)
    throw InvalidArgumentException(HERE) << "in MatrixFisher::setF: F must be 3x3, got dimension=" << n;

  if (F != F_)
  {
    F_ = F;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
    updateSampler();
    computeRange();
  }
}

SquareMatrix MatrixFisher::getF() const
{
  return F_;
}

Scalar MatrixFisher::getEpsilon() const
{
  return epsilon_;
}

void MatrixFisher::setEpsilon(const Scalar epsilon)
{
  const Scalar eps = std::max(SpecFunc::ScalarEpsilon, epsilon);
  if (eps != epsilon_)
  {
    epsilon_ = eps;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
    updateSampler();
  }
}

Scalar MatrixFisher::computeEntropy() const
{
  // Entropy = log a_0(F) - E[tr(F^T R)]
  return logNormalization_ - expectedTrace_;
}

void MatrixFisher::computeMean() const
{
  // E[R] has been computed by quadrature in computeNormalization
  mean_ = Point(9);
  UnsignedInteger idx = 0;
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      mean_[idx++] = expectedMatrix_(i, j);
  isAlreadyComputedMean_ = true;
}

void MatrixFisher::computeCovariance() const
{
  // Cov[vec(R)] = E[vec(R)vec(R)^T] - E[vec(R)] E[vec(R)]^T,
  // both integrals evaluated by quadrature.
  // The distribution is 9-dimensional (flattened 3x3), so covariance is 9x9
  CovarianceMatrix cov(9);
  for (UnsignedInteger i = 0; i < 3; ++i)
    for (UnsignedInteger j = 0; j < 3; ++j)
      for (UnsignedInteger k = 0; k < 3; ++k)
        for (UnsignedInteger l = 0; l < 3; ++l)
          cov(3 * i + j, 3 * k + l) =
              expectedSquaredMatrix_(3 * i + j, 3 * k + l)
              - expectedMatrix_(i, j) * expectedMatrix_(k, l);
  covariance_ = cov;
  isAlreadyComputedCovariance_ = true;
}

Bool MatrixFisher::isContinuous() const
{
  return true;
}

void MatrixFisher::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("F_", F_);
  adv.saveAttribute("U_", U_);
  adv.saveAttribute("V_", V_);
  adv.saveAttribute("singularValues_", singularValues_);
  adv.saveAttribute("epsilon_", epsilon_);
  adv.saveAttribute("logNormalization_", logNormalization_);
  adv.saveAttribute("maxTrace_", maxTrace_);
  adv.saveAttribute("expectedMatrix_", expectedMatrix_);
  adv.saveAttribute("expectedSquaredMatrix_", expectedSquaredMatrix_);
  adv.saveAttribute("expectedTrace_", expectedTrace_);
}

void MatrixFisher::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("F_", F_);
  adv.loadAttribute("U_", U_);
  adv.loadAttribute("V_", V_);
  adv.loadAttribute("singularValues_", singularValues_);
  adv.loadAttribute("epsilon_", epsilon_);
  adv.loadAttribute("logNormalization_", logNormalization_);
  adv.loadAttribute("maxTrace_", maxTrace_);
  adv.loadAttribute("expectedMatrix_", expectedMatrix_);
  adv.loadAttribute("expectedSquaredMatrix_", expectedSquaredMatrix_);
  adv.loadAttribute("expectedTrace_", expectedTrace_);
  computeRange();
}

END_NAMESPACE_OPENTURNS