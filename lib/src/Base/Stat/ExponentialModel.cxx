//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/ExponentialModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExponentialModel
 */

CLASSNAMEINIT(ExponentialModel);

static const Factory<ExponentialModel> Factory_ExponentialModel;

/* Constructor from spatial dimension */
ExponentialModel::ExponentialModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension)
{
  // Nothing to do
}

/* Constructor with parameters */
ExponentialModel::ExponentialModel(const UnsignedInteger spatialDimension,
                                   const NumericalPoint & amplitude,
                                   const NumericalPoint & scale)
  : StationaryCovarianceModel(spatialDimension, amplitude, scale)
{
  // Nothing to do
}

ExponentialModel::ExponentialModel(const UnsignedInteger spatialDimension,
                                   const NumericalPoint & amplitude,
                                   const NumericalPoint & scale,
                                   const CorrelationMatrix & spatialCorrelation)
  : StationaryCovarianceModel(spatialDimension, amplitude, scale, spatialCorrelation)
{
  // Nothing to do
}

ExponentialModel::ExponentialModel(const UnsignedInteger spatialDimension,
                                   const NumericalPoint & scale,
                                   const CovarianceMatrix & spatialCovariance)
  : StationaryCovarianceModel(spatialDimension, scale, spatialCovariance)
{
  // Nothing to do
}

/** Standard constructor with amplitude and scale parameters parameters */
ExponentialModel::ExponentialModel(const NumericalPoint & scale,
                                   const NumericalPoint & amplitude)
  : StationaryCovarianceModel(amplitude, scale)
{
  // Nothing to do
}

/** Standard constructor with amplitude, scale and spatial correlation parameters parameters */
ExponentialModel::ExponentialModel(const NumericalPoint & scale,
                                   const NumericalPoint & amplitude,
                                   const CorrelationMatrix & spatialCorrelation)
  : StationaryCovarianceModel(amplitude, scale, spatialCorrelation)
{
  // Nothing to do
}

/** Standard constructor with scale and spatial covariance parameters parameters */
ExponentialModel::ExponentialModel(const NumericalPoint & scale,
                                   const CovarianceMatrix & spatialCovariance)
  : StationaryCovarianceModel(scale, spatialCovariance)
{
  // Nothing to do
}

/* Virtual constructor */
ExponentialModel * ExponentialModel::clone() const
{
  return new ExponentialModel(*this);
}


/* Computation of the covariance function, stationary interface
 * C_{i,j}(tau) = amplitude_i * R_{i,j} * amplitude_j  * exp(-|tau / scale|)
 * C_{i,i}(tau) = amplitude_i^2  * exp(-|tau / scale|)
 */
NumericalScalar ExponentialModel::computeStandardRepresentative(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In ExponentialModel::computeStandardRepresentative: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  // Absolute value of tau / scale
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar tauOverThetaNorm = tauOverTheta.norm();
  // Return value
  return (tauOverThetaNorm == 0.0 ? 1.0 + nuggetFactor_ : exp(- tauOverThetaNorm ));
}


/** Gradient */
Matrix ExponentialModel::partialGradient(const NumericalPoint & s,
                                         const NumericalPoint & t) const
{
  /* Computation of the gradient
   * dC_{i,j}(tau)/dtau_k = C_{i,j} * (-\frac{1}{2 * scale_i} -\frac{1}{2 * scale_j}) * factor, with factor = tau_k / absTau
    Note that if spatial dimesnion is 1, factor = sgn(tau_k)
   */
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  const NumericalPoint tau(s - t);
  const NumericalScalar absTau = tau.norm();
  NumericalPoint tauOverTheta(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i) tauOverTheta[i] = tau[i] / scale_[i];
  const NumericalScalar absTauOverTheta = tauOverTheta.norm();

  // TODO check
  if (absTau == 0)
    throw InvalidArgumentException(HERE) << "ExponentialModel::partialGradient, the points t and s are equal. Covariance model has no derivate for that case.";
  // Covariance matrix write S * rho(tau), so gradient writes Sigma * grad(rho) where * is a 'dot',
  // i.e. dC/dk= Sigma_{i,j} * drho/dk
  CovarianceMatrix covariance(operator()(tau));
  // symmetrize if not diagonal
  if (!isDiagonal_) covariance.getImplementation()->symmetrize();
  NumericalPoint covariancePoint(*covariance.getImplementation());
  // Compute the gradient part (gradient of rho)
  NumericalPoint factor(spatialDimension_);
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i)
  {
    if ((spatialDimension_ == 1.0) && (tau[i] < 0))  factor[i] = 1.0 / scale_[i] ;
    else if ((spatialDimension_ == 1.0) && (tau[i] > 0))  factor[i] = -1.0 / scale_[i];
    // General case
    else factor[i] = -1.0 * tau[i] / (absTauOverTheta * scale_[i] * scale_[i]);
  }
  // Finally assemble the final matrix
  Matrix gradient(spatialDimension_, covariancePoint.getDimension());
  for (UnsignedInteger j = 0; j < covariancePoint.getDimension(); ++ j)
    for (UnsignedInteger i = 0; i < spatialDimension_; ++i)
      gradient(i, j) = covariancePoint[j] * factor[i];
  return gradient;
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix ExponentialModel::discretize(const RegularGrid & timeGrid) const
{
  const UnsignedInteger size(timeGrid.getN());
  const UnsignedInteger fullSize(size * dimension_);
  const NumericalScalar timeStep(timeGrid.getStep());

  CovarianceMatrix cov(fullSize);

  // The stationary property of this model allows to optimize the discretization
  // over a regular time grid: the large covariance matrix is block-diagonal
  // Fill the matrix by block-diagonal
  // The main diagonal has a specific treatment as only its lower triangular part
  // has to be copied
  const CovarianceMatrix covTau0( operator()( 0.0 ) );

  // Loop over the main diagonal block
  for (UnsignedInteger block = 0; block < size; ++block)
  {
    const UnsignedInteger base(block * dimension_);
    // Copy of the lower triangle only
    for (UnsignedInteger i = 0; i < dimension_; ++i)
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
    const CovarianceMatrix covTau( operator()( diag * timeStep ) );

    // Loop over the main block diagonal
    for (UnsignedInteger block = 0; block < size - diag; ++block)
    {
      const UnsignedInteger base(block * dimension_);
      const UnsignedInteger baseDiag((block + diag) * dimension_);
      // Copy of the full block
      for (UnsignedInteger i = 0; i < dimension_; ++i)
      {
        // The diagonal part
        cov(base + i, baseDiag + i) = covTau(i, i);
        // The off-diagonal part if needed
        if (!isDiagonal_)
        {
          for (UnsignedInteger j = 0; j < i; ++j)
            cov(base + i, baseDiag + j) = covTau(i, j);
          for (UnsignedInteger j = i + 1; j < dimension_; ++j)
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
  oss << " input dimension=" << spatialDimension_
      << " amplitude=" << getAmplitude()
      << " scale=" << getScale()
      << " spatial correlation=" << getSpatialCorrelation()
      << " isDiagonal=" << isDiagonal();
  return oss;
}

/* String converter */
String ExponentialModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << ExponentialModel::GetClassName();
  oss << "(input dimension=" << spatialDimension_
      << ", amplitude=" << getAmplitude()
      << ", scale=" << getScale();
  if (!isDiagonal_)
    oss << ", spatial correlation=\n" << getSpatialCorrelation().__str__(offset);
  else
    oss << ", no spatial correlation";
  oss << ")";
  return oss;
}

void ExponentialModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void ExponentialModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
}

END_NAMESPACE_OPENTURNS
