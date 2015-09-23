//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "ExponentiallyDampedCosineModel.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExponentiallyDampedCosineModel
 */

CLASSNAMEINIT(ExponentiallyDampedCosineModel);

static const Factory<ExponentiallyDampedCosineModel> RegisteredFactory;

/* Constructor from spatial dimension */
ExponentiallyDampedCosineModel::ExponentiallyDampedCosineModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension)
  , frequency_(1.0)
{
  // Nothing to do
}

ExponentiallyDampedCosineModel::ExponentiallyDampedCosineModel(const UnsignedInteger spatialDimension,
    const NumericalScalar amplitude,
    const NumericalScalar scale,
    const NumericalScalar frequency)
  : StationaryCovarianceModel(spatialDimension, NumericalPoint(1, amplitude), NumericalPoint(1, scale))
  , frequency_(0.0)
{
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: the output dimension must be 1, here dimension=" << dimension_;
  setFrequency(frequency);
}

/* Virtual constructor */
ExponentiallyDampedCosineModel * ExponentiallyDampedCosineModel::clone() const
{
  return new ExponentiallyDampedCosineModel(*this);
}


/* Computation of the covariance function, stationary interface
 * C_{0,0}(tau) = amplitude_ * exp(-|tau| / scale_) * cos(2 * pi * frequency_ * |tau|)
 */
CovarianceMatrix ExponentiallyDampedCosineModel::operator() (const NumericalPoint & tau) const
{
  CovarianceMatrix covarianceMatrix(dimension_);

  covarianceMatrix(0, 0) = computeAsScalar(tau);
  return covarianceMatrix;
}

NumericalScalar ExponentiallyDampedCosineModel::computeAsScalar(const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: expected a shift of dimension=" << spatialDimension_ << ", got dimension=" << tau.getDimension();
  const NumericalScalar tauNorm(tau.norm());
  return (tauNorm == 0.0 ? amplitude_[0] * (1.0 + nuggetFactor_) : amplitude_[0] * exp(-tauNorm / scale_[0]) * cos(2.0 * M_PI * tauNorm));
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix ExponentiallyDampedCosineModel::discretize(const RegularGrid & timeGrid) const
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
  for (UnsignedInteger diag = 0; diag < size; ++diag)
  {
    const NumericalScalar covTau(computeAsScalar(NumericalPoint(1, diag * timeStep)));
    for (UnsignedInteger i = 0; i < size - diag; ++i) cov(i, i + diag) = covTau;
  }

  return cov;
}

/* Is it a stationary model ? */
Bool ExponentiallyDampedCosineModel::isStationary() const
{
  return true;
}

/* String converter */
String ExponentiallyDampedCosineModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ExponentiallyDampedCosineModel::GetClassName();
  oss << " input dimension=" << spatialDimension_
      << " amplitude=" << amplitude_[0]
      << " scale=" << scale_[0]
      << " frequency=" << frequency_;
  return oss;
}

/* String converter */
String ExponentiallyDampedCosineModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << ExponentiallyDampedCosineModel::GetClassName();
  oss << " input dimension=" << spatialDimension_
      << " amplitude=" << amplitude_[0]
      << " scale=" << scale_[0]
      << " frequency=" << frequency_;
  return oss;
}

/* Frequency accessor */
NumericalScalar ExponentiallyDampedCosineModel::getFrequency() const
{
  return frequency_;
}

void ExponentiallyDampedCosineModel::setFrequency(const NumericalScalar frequency)
{
  if (frequency <= 0.0) throw InvalidArgumentException(HERE) << "Error: the frequency must be positive.";
  frequency_ = frequency;
}

/* Method save() stores the object through the StorageManager */
void ExponentiallyDampedCosineModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute( "frequency_", frequency_);
}

/* Method load() reloads the object from the StorageManager */
void ExponentiallyDampedCosineModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute( "frequency_", frequency_);
}

END_NAMESPACE_OPENTURNS
