//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Os.hxx"

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
 * C_{i,j}(tau) = amplitude_i * R_{i,j} * amplitude_j  * exp(-|tau / scale|)
 * C_{i,i}(tau) = amplitude_i^2  * exp(-|tau / scale|)
 */
Scalar ExponentialModel::computeAsScalar(const Point &tau) const
{
  if (outputDimension_ > 1)
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
  return (tauOverThetaNorm <= SpecFunc::ScalarEpsilon ? outputCovariance_(0, 0) * (1.0 + nuggetFactor_) : outputCovariance_(0, 0) * exp(-tauOverThetaNorm));
}

/** Gradient */
Matrix ExponentialModel::partialGradient(const Point & s,
    const Point & t) const
{
  /* Computation of the gradient
   * dC_{i,j}(tau)/dtau_k = C_{i,j} * (-\frac{1}{2 * scale_i} -\frac{1}{2 * scale_j}) * factor, with factor = tau_k / absTau
    Note that if spatial dimension is 1, factor = sgn(tau_k)
   */
  if (s.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << getInputDimension();
  if (t.getDimension() != getInputDimension()) throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << getInputDimension();
  // Compute tau.norm() & (tau/scale_).norm()
  Scalar absTau = 0.0;
  Scalar absTauOverTheta = 0.0;
  Scalar dx = 1.0;
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    dx = (s[i] - t[i]);
    absTau += dx*dx;
    dx /= scale_[i];
    absTauOverTheta += dx * dx;
  }
  absTau = sqrt(absTau);
  if (absTau == 0)
    throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the points t and s are equal. Covariance model has no derivate for that case.";
  absTauOverTheta = sqrt(absTau);
  // TODO check & implement specific 1d output case (after inheritance from KronekerCovarianceModel)
  // Covariance matrix write S * rho(tau), so gradient writes Sigma * grad(rho) where * is a 'dot',
  // i.e. dC/dk= Sigma_{i,j} * drho/dk
  // TODO Remove explicit call to operator()
  SquareMatrix covariance(operator()(s, t));
  Point covariancePoint(*covariance.getImplementation());
  // Compute the gradient part (gradient of rho)
  Point factor(getInputDimension());
  for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
  {
    const Scalar tauI = s[i] - t[i];
    if ((getInputDimension() == 1.0) && (tauI < 0))  factor[i] = 1.0 / scale_[i] ;
    else if ((getInputDimension() == 1.0) && (tauI > 0))  factor[i] = -1.0 / scale_[i];
    // General case
    else factor[i] = -1.0 * tauI / (absTauOverTheta * scale_[i] * scale_[i]);
  }
  // Finally assemble the final matrix
  Matrix gradient(getInputDimension(), covariancePoint.getDimension());
  for (UnsignedInteger j = 0; j < covariancePoint.getDimension(); ++ j)
    for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
      gradient(i, j) = covariancePoint[j] * factor[i];
  return gradient;
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix ExponentialModel::discretize(const RegularGrid & timeGrid) const
{
  const UnsignedInteger size = timeGrid.getN();
  const UnsignedInteger fullSize = size * getOutputDimension();
  const Scalar timeStep = timeGrid.getStep();

  CovarianceMatrix cov(fullSize);

  // The stationary property of this model allows to optimize the discretization
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
    oss << ", spatial correlation=" << Os::GetEndOfLine() << offset << getOutputCorrelation().__str__(offset);
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
