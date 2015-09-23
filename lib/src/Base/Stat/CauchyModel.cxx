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
#include <cmath>

#include "CauchyModel.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"
#include "Field.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CauchyModel
 */

CLASSNAMEINIT(CauchyModel);

static const Factory<CauchyModel> RegisteredFactory;

/* Constructor with parameters */
CauchyModel::CauchyModel()
  : SpectralModelImplementation()
  , amplitude_(NumericalPoint(1, 1.0))
  , scale_(NumericalPoint(1, 1.0))
  , spatialCorrelation_(0)
  , isDiagonal_(true)
{
  dimension_ = 1;
}

CauchyModel::CauchyModel(const NumericalPoint & amplitude,
                         const NumericalPoint & scale)
  : SpectralModelImplementation()
  , amplitude_(0)
  , scale_(0)
  , spatialCorrelation_(0)
  , isDiagonal_(true)
{
  if (amplitude.getDimension() != scale.getDimension() )
    throw InvalidArgumentException(HERE) << "Incompatible dimensions between amplitude and scale values";
  setAmplitude(amplitude);
  setScale(scale);
  dimension_ = amplitude.getDimension();
}

CauchyModel::CauchyModel(const NumericalPoint & amplitude,
                         const NumericalPoint & scale,
                         const CorrelationMatrix & spatialCorrelation)
  : SpectralModelImplementation()
  , amplitude_(0)
  , scale_(0)
  , spatialCorrelation_(0)
  , isDiagonal_(false)
{
  dimension_ = amplitude.getDimension();
  if (scale.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Incompatible dimensions between amplitude and scale values";
  if (spatialCorrelation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given spatial correlation has a dimension different from the scales and amplitudes.";
  setAmplitude(amplitude);
  setScale(scale);
  isDiagonal_ = spatialCorrelation.isDiagonal();
  if (!isDiagonal_) spatialCorrelation_ = spatialCorrelation;
}

CauchyModel::CauchyModel(const NumericalPoint & scale,
                         const CovarianceMatrix & spatialCovariance)
  : SpectralModelImplementation()
  , amplitude_(0)
  , scale_(scale)
  , spatialCorrelation_(0)
  , isDiagonal_(false)
{
  dimension_ = scale.getDimension();
  if (spatialCovariance.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given spatial covariance has a dimension different from the scales and amplitudes.";
  setScale(scale);
  NumericalPoint amplitude(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    amplitude[i] = sqrt(spatialCovariance(i, i));
  // Check that the amplitudes are valid
  setAmplitude(amplitude);
  // Convert the spatial covariance into a spatial correlation
  isDiagonal_ = spatialCovariance.isDiagonal();
  if (!isDiagonal_)
  {
    spatialCorrelation_ = CorrelationMatrix(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      for (UnsignedInteger j = 0; j < i; ++j)
        spatialCorrelation_(i, j) = spatialCovariance(i, j) / (amplitude[i] * amplitude[j]);
  } // !isDiagonal
}

/* Virtual constructor */
CauchyModel * CauchyModel::clone() const
{
  return new CauchyModel(*this);
}

/* Computation of the spectral density function */
HermitianMatrix CauchyModel::operator() (const NumericalScalar frequency) const
{
  const NumericalScalar scaledFrequencySquared(pow(2.0 * M_PI * fabs(frequency), 2));
  HermitianMatrix spectralDensityMatrix(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    spectralDensityMatrix(i, i) = 2.0 * amplitude_[i] * amplitude_[i] * scale_[i] / (1.0 + scale_[i] * scale_[i] * scaledFrequencySquared);
  if (!isDiagonal_)
    for (UnsignedInteger j = 0; j < dimension_ ; ++j)
      for (UnsignedInteger i = j + 1; i < dimension_ ; ++i)
        spectralDensityMatrix(i, j) = amplitude_[i] * spatialCorrelation_(i, j) * amplitude_[j] * (scale_[i] + scale_[j]) / (0.25 * (scale_[i] / scale_[j] + 2.0 + scale_[j] / scale_[i]) + scale_[i] * scale_[j] * scaledFrequencySquared);

  return spectralDensityMatrix;
}

/* String converter */
String CauchyModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CauchyModel::GetClassName();
  oss << " amplitude=" << amplitude_
      << " scale=" << scale_
      << " spatial correlation=" << spatialCorrelation_
      << " isDiagonal=" << isDiagonal_;
  return oss;
}

/* String converter */
String CauchyModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << CauchyModel::GetClassName();
  oss << " amplitude=" << amplitude_
      << " scale=" << scale_;
  if (!isDiagonal_)
    oss << " spatial correlation=\n" << spatialCorrelation_.__str__(offset);
  else
    oss << " no spatial correlation";
  return oss;
}

/* Amplitude accessor */
NumericalPoint CauchyModel::getAmplitude() const
{
  return amplitude_;
}

void CauchyModel::setAmplitude(const NumericalPoint & amplitude)
{
  for (UnsignedInteger index = 0; index < dimension_; ++index)
    if (amplitude[index] <= 0)
      throw InvalidArgumentException(HERE) << "Error - The component " << index << " of amplitude is non positive" ;
  amplitude_ = amplitude;
}

/* Scale accessor */
NumericalPoint CauchyModel::getScale() const
{
  return scale_;
}

void CauchyModel::setScale(const NumericalPoint & scale)
{
  for (UnsignedInteger index = 0; index < dimension_; ++index)
    if (scale[index] <= 0)
      throw InvalidArgumentException(HERE) << "Error - The component " << index << " of scale is non positive" ;
  scale_ = scale;
}

/* Spatial correlation accessor */
CorrelationMatrix CauchyModel::getSpatialCorrelation() const
{
  if (!isDiagonal_) return spatialCorrelation_;
  return CorrelationMatrix(dimension_);
}

/* Method save() stores the object through the StorageManager */
void CauchyModel::save(Advocate & adv) const
{
  SpectralModelImplementation::save(adv);
  adv.saveAttribute( "amplitude_", amplitude_);
  adv.saveAttribute( "scale_", scale_);
  adv.saveAttribute( "spatialCorrelation_", spatialCorrelation_);
  adv.saveAttribute( "isDiagonal_", isDiagonal_);
}

/* Method load() reloads the object from the StorageManager */
void CauchyModel::load(Advocate & adv)
{
  SpectralModelImplementation::load(adv);
  adv.loadAttribute( "amplitude_", amplitude_);
  adv.loadAttribute( "scale_", scale_);
  adv.loadAttribute( "spatialCorrelation_", spatialCorrelation_);
  adv.loadAttribute( "isDiagonal_", isDiagonal_);
}

END_NAMESPACE_OPENTURNS
