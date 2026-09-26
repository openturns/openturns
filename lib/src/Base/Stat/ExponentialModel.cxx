//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/ExponentialModel.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExponentialModel
 */

CLASSNAMEINIT(ExponentialModel)

static const Factory<ExponentialModel> Factory_ExponentialModel;

/* Constructor from input dimension */
ExponentialModel::ExponentialModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
{
  isStationary_ = true;
}

/** Standard constructor with scale and amplitude parameters parameters */
ExponentialModel::ExponentialModel(const Point & scale,
                                   const Point & amplitude)
  : CovarianceModelImplementation(scale, amplitude)
{
  isStationary_ = true;
}

/** Standard constructor with scale, amplitude and spatial correlation parameters parameters */
ExponentialModel::ExponentialModel(const Point & scale,
                                   const Point & amplitude,
                                   const CorrelationMatrix & spatialCorrelation)
  : CovarianceModelImplementation(scale, amplitude, spatialCorrelation)
{
  isStationary_ = true;
}

/** Standard constructor with scale and spatial covariance parameters parameters */
ExponentialModel::ExponentialModel(const Point & scale,
                                   const CovarianceMatrix & spatialCovariance)
  : CovarianceModelImplementation(scale, spatialCovariance)
{
  isStationary_ = true;
}

/* Virtual constructor */
ExponentialModel * ExponentialModel::clone() const
{
  return new ExponentialModel(*this);
}

/* Comparison operators */
Bool ExponentialModel::operator ==(const ExponentialModel & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other);
}

Bool ExponentialModel::equals(const CovarianceModelImplementation & other) const
{
  const ExponentialModel * p_other = dynamic_cast<const ExponentialModel *>(&other);
  return p_other && (*this == *p_other);
}

SquareMatrix ExponentialModel::operator()(const Point &tau) const
{
  // L2 norm of tau / scale
  Scalar tauOverThetaNorm = 0.0;
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    tauOverThetaNorm += dx * dx;
  }
  tauOverThetaNorm = sqrt(tauOverThetaNorm);
  // Return value
  Scalar factor = 1.0;
  if (tauOverThetaNorm == 0.0)
    factor = 1.0 + nuggetFactor_;
  else
    factor = exp(-tauOverThetaNorm);
  SquareMatrix output(outputCovariance_);
  output.getImplementation()->symmetrize();
  return output * factor;
}

/* Computation of the covariance function, stationary interface
 * C_{i,j}(tau) = amplitude_i * R_{i,j} * amplitude_j  * (exp(-|tau / scale|) + nuggetFactor_ * delta(tau = 0))
 * C_{i,i}(tau) = amplitude_i^2  * (exp(-|tau / scale|) + nuggetFactor_ * delta(tau = 0))
 */
Scalar ExponentialModel::computeAsScalar(const Point &tau) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error : ExponentialModel::computeAsScalar(tau) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In ExponentialModel::computeStandardRepresentative: expected a shift of dimension=" << getInputDimension() << ", got dimension=" << tau.getDimension();

  // L2 norm of tau / scale
  Scalar tauOverThetaNorm = 0.0;
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    tauOverThetaNorm += dx * dx;
  }
  tauOverThetaNorm = sqrt(tauOverThetaNorm);
  // Return value
  return (tauOverThetaNorm == 0.0 ? amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_) : amplitude_[0] * amplitude_[0] * exp(-tauOverThetaNorm));
}

/* Computation of the covariance function, stationary interface
 * C_{i,j}(tau) = amplitude_i * R_{i,j} * amplitude_j  * exp(-|tau / scale|)
 * C_{i,i}(tau) = amplitude_i^2  * exp(-|tau / scale|)
 */
Scalar ExponentialModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  if (outputDimension_ != 1)
    throw InvalidArgumentException(HERE) << "Error : ExponentialModel::computeAsScalar(it, it) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;

  Scalar tauOverThetaNorm = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) / scale_[i];
    tauOverThetaNorm += dx * dx;
  }
  tauOverThetaNorm = sqrt(tauOverThetaNorm);
  return (tauOverThetaNorm == 0.0 ? amplitude_[0] * amplitude_[0] * (1.0 + nuggetFactor_) : amplitude_[0] * amplitude_[0] * exp(-tauOverThetaNorm));
}

Scalar ExponentialModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";

  const Scalar tauOverThetaNorm = std::abs(tau / scale_[0]);
  // Return value
  const CovarianceMatrix & outputCovariance = outputCovariance_;
  return (tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance(0, 0) * (1.0 + nuggetFactor_) : outputCovariance(0, 0) * exp(-tauOverThetaNorm));
}

/** Gradient */
Matrix ExponentialModel::partialGradient(const Point & s,
    const Point & t) const
{
  // Computation of the gradient
  if (s.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << getInputDimension();
  if (t.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << getInputDimension();

  Scalar norm = 0.0;
  Scalar dx = 0.0;
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    dx = (s[i] - t[i]) / scale_[i];
    norm += dx * dx;
  }
  if (norm == 0)
    throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the points t and s are equal. Covariance model has no derivate for that case.";
  norm = std::sqrt(norm);
  // Covariance matrix write S * rho(tau), so gradient writes Sigma * grad(rho) where * is a 'kronecker',
  SquareMatrix covariance(outputCovariance_);
  covariance.getImplementation()->symmetrize();
  Point covariancePoint(*covariance.getImplementation());
  // Finally assemble the final matrix
  const Scalar value = -std::exp(-norm) / norm;
  Matrix gradient(getInputDimension(), covariancePoint.getDimension());
  for (UnsignedInteger j = 0; j < covariancePoint.getDimension(); ++ j)
    for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
      gradient(i, j) = covariancePoint[j] * (s[i] - t[i]) / (scale_[i] * scale_[i]) * value;
  return gradient;
}

/** Hessian wrt s, for the first output component */
SymmetricMatrix ExponentialModel::partialHessian(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::partialHessian, the point s has dimension=" << s.getDimension() << ", expected dimension=" << getInputDimension();
  if (t.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::partialHessian, the point t has dimension=" << t.getDimension() << ", expected dimension=" << getInputDimension();

  Scalar norm = 0.0;
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    const Scalar dx = (s[i] - t[i]) / scale_[i];
    norm += dx * dx;
  }
  norm = std::sqrt(norm);
  // At zero norm the Hessian is not defined: fall back to the finite-difference implementation
  if (norm == 0.0) return CovarianceModelImplementation::partialHessian(s, t);
  // k_00(s, t) = C_00 * exp(-z) with z = ||tau/scale||
  // d^2 k / ds_a ds_b = C_00 * u_a u_b (z + 1) exp(-z) / z^3
  //   - delta_ab C_00 exp(-z) / (scale_a^2 z), with u_i = tau_i / scale_i^2
  const Scalar z = norm;
  const Scalar coefficient = outputCovariance_(0, 0);
  const Scalar exponent = std::exp(-z);
  const Scalar factor = coefficient * (z + 1.0) * exponent / (z * z * z);
  const Scalar diagonalFactor = -coefficient * exponent / z;
  Point u(inputDimension_);
  for (UnsignedInteger i = 0; i < inputDimension_; ++i) u[i] = (s[i] - t[i]) / (scale_[i] * scale_[i]);
  SymmetricMatrix hessian(inputDimension_);
  for (UnsignedInteger a = 0; a < inputDimension_; ++a)
  {
    for (UnsignedInteger b = 0; b < a; ++b)
      hessian(a, b) = factor * u[a] * u[b];
    hessian(a, a) = factor * u[a] * u[a] + diagonalFactor / (scale_[a] * scale_[a]);
  }
  return hessian;
}

/* Gradient wrt the parameters */
Matrix ExponentialModel::parameterGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::parameterGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << getInputDimension();
  if (t.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::parameterGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << getInputDimension();

  const Point tau(s - t);
  Scalar norm = 0.0;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i)
  {
    const Scalar dx = tau[i] / scale_[i];
    norm += dx * dx;
  }
  norm = std::sqrt(norm);
  const Bool isZero = (norm <= SpecFunc::ScalarEpsilon);

  if (outputDimension_ == 1)
  {
    const Scalar k = computeAsScalar(s, t);
    Point fullGradient(inputDimension_ + 2, 0.0);
    if (!isZero)
      for (UnsignedInteger i = 0; i < inputDimension_; ++i)
        fullGradient[i] = k * tau[i] * tau[i] / (norm * scale_[i] * scale_[i] * scale_[i]);
    else
      fullGradient[inputDimension_] = outputCovariance_(0, 0);
    fullGradient[inputDimension_ + 1] = 2.0 * k / amplitude_[0];
    return filterActiveParameterGradient(fullGradient);
  }

  // Multivariate case: C(i,j) = amplitude_i * R_ij * amplitude_j * rho(tau)
  const Scalar rho = isZero ? 1.0 + nuggetFactor_ : std::exp(-norm);
  // Scalar derivative of rho wrt the scales
  // d(log rho)/dscale_i = tau_i^2 / (norm * scale_i^3), so that the rho factor
  // cancels analytically when multiplying covariance(i,j) * d(rho)/dscale_i
  Point logRhoScaleGradient(inputDimension_, 0.0);
  if (!isZero)
    for (UnsignedInteger i = 0; i < inputDimension_; ++i)
      logRhoScaleGradient[i] = tau[i] * tau[i] / (norm * scale_[i] * scale_[i] * scale_[i]);
  const SquareMatrix covariance(operator()(s, t));
  // Full gradient wrt all the parameters, one row per parameter
  const UnsignedInteger fullSize = inputDimension_ + 1 + outputDimension_ + outputDimension_ * (outputDimension_ - 1) / 2;
  Matrix fullGradient(fullSize, outputDimension_ * outputDimension_);
  // Gradient wrt the scale parameters
  for (UnsignedInteger k = 0; k < inputDimension_; ++k)
  {
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < outputDimension_; ++j)
      for (UnsignedInteger i = 0; i < outputDimension_; ++i, ++index)
        fullGradient(k, index) = covariance(i, j) * logRhoScaleGradient[k];
  }
  // Gradient wrt the nugget factor
  if (isZero)
  {
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < outputDimension_; ++j)
      for (UnsignedInteger i = 0; i < outputDimension_; ++i, ++index)
        fullGradient(inputDimension_, index) = outputCovariance_(i, j);
  }
  // Gradient wrt the amplitude parameters
  for (UnsignedInteger k = 0; k < outputDimension_; ++k)
  {
    UnsignedInteger index = 0;
    for (UnsignedInteger j = 0; j < outputDimension_; ++j)
      for (UnsignedInteger i = 0; i < outputDimension_; ++i, ++index)
      {
        if (i == k) fullGradient(inputDimension_ + 1 + k, index) = covariance(i, j) / amplitude_[k];
        if (j == k) fullGradient(inputDimension_ + 1 + k, index) += covariance(i, j) / amplitude_[k];
      }
  }
  // Gradient wrt the output correlation parameters (lower triangle)
  if (outputDimension_ > 1)
  {
    UnsignedInteger correlationIndex = 0;
    for (UnsignedInteger i = 1; i < outputDimension_; ++i)
      for (UnsignedInteger j = 0; j < i; ++j, ++correlationIndex)
      {
        const Scalar derivative = amplitude_[i] * amplitude_[j] * rho;
        UnsignedInteger index = 0;
        for (UnsignedInteger q = 0; q < outputDimension_; ++q)
          for (UnsignedInteger p = 0; p < outputDimension_; ++p, ++index)
          {
            if (p == i && q == j) fullGradient(inputDimension_ + 1 + outputDimension_ + correlationIndex, index) = derivative;
            if (p == j && q == i) fullGradient(inputDimension_ + 1 + outputDimension_ + correlationIndex, index) = derivative;
          }
      }
  }
  return filterActiveParameterGradient(fullGradient);
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix ExponentialModel::discretize(const RegularGrid & timeGrid) const
{
  const UnsignedInteger size = timeGrid.getN();
  const UnsignedInteger fullSize = size * getOutputDimension();
  const Scalar timeStep = timeGrid.getStep();

  CovarianceMatrix cov(fullSize);

  // The stationary property of this model allows one to optimize the discretization
  // over a regular time grid: the large covariance matrix is block-diagonal
  // Fill the matrix by block-diagonal
  // The main diagonal has a specific treatment as only its lower triangular part
  // has to be copied
  const SquareMatrix covTau0( operator()( 0.0 ) );

  // Loop over the main diagonal block
  for (UnsignedInteger block = 0; block < size; ++block)
  {
    const UnsignedInteger base = block * getOutputDimension();
    // Copy of the lower triangle only
    for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
    {
      // The diagonal part
      cov( base + i,
           base + i ) = covTau0(i, i);
      // The lower off-diagonal part if needed
      if (!isDiagonal_)
        for (UnsignedInteger j = 0; j < i; ++j)
          cov( base + i,
               base + j ) = covTau0(i, j);
    } // Lower triangle
  } // block
  // Loop over the remaining diagonal blocks
  for (UnsignedInteger diag = 1; diag < size; ++diag)
  {
    const SquareMatrix covTau( operator()( diag * timeStep ) );

    // Loop over the main block diagonal
    for (UnsignedInteger block = 0; block < size - diag; ++block)
    {
      const UnsignedInteger base = block * getOutputDimension();
      const UnsignedInteger baseDiag = (block + diag) * getOutputDimension();
      // Copy of the full block
      for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
      {
        // The diagonal part
        cov(base + i, baseDiag + i) = covTau(i, i);
        // The off-diagonal part if needed
        if (!isDiagonal_)
        {
          for (UnsignedInteger j = 0; j < i; ++j)
            cov(base + i, baseDiag + j) = covTau(i, j);
          for (UnsignedInteger j = i + 1; j < getOutputDimension(); ++j)
            cov(base + i, baseDiag + j) = covTau(i, j);
        } // Off-diagonal
      } // Full block
    } // block
  } // Off-diagonal blocks

  return cov;
}

/* String converter */
String ExponentialModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ExponentialModel::GetClassName();
  oss << " scale=" << getScale()
      << " amplitude=" << getAmplitude()
      << " spatial correlation=" << getOutputCorrelation()
      << " isDiagonal=" << isDiagonal();
  return oss;
}

/* String converter */
String ExponentialModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << ExponentialModel::GetClassName();
  oss << "(scale=" << getScale()
      << ", amplitude=" << getAmplitude();
  if (!isDiagonal_)
    oss << ", spatial correlation=" << "\n" << offset << getOutputCorrelation().__str__(offset);
  else
    oss << ", no spatial correlation";
  oss << ")";
  return oss;
}

void ExponentialModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ExponentialModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
