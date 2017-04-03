//                                               -*- C++ -*-
/**
 *  @brief The class SpectralModelImplementation defines the concept of
 *         spectral model for stationary processes
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SpectralModelImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpectralModelImplementation);
static const Factory<SpectralModelImplementation> Factory_SpectralModelImplementation;

/* Constructor with parameters */
SpectralModelImplementation::SpectralModelImplementation()
  : PersistentObject()
  , dimension_(1)
  , scale_(Point(1, 1.0))
  , amplitude_(Point(1, 1.0))
  , spatialDimension_(1)
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
{
  updateSpatialCovariance();
}

SpectralModelImplementation::SpectralModelImplementation(const Point & scale,
    const Point & amplitude)
  : PersistentObject()
  , dimension_(amplitude.getDimension())
  , scale_(0)
  , amplitude_(0)
  , spatialDimension_(scale.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
{
  setAmplitude(amplitude);
  setScale(scale);
  updateSpatialCovariance();
}

SpectralModelImplementation::SpectralModelImplementation(const Point & scale,
    const Point & amplitude,
    const CorrelationMatrix & spatialCorrelation)
  : PersistentObject()
  , dimension_(amplitude.getDimension())
  , scale_(0)
  , amplitude_(0)
  , spatialDimension_(scale.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(false)
{
  if (spatialCorrelation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given spatial correlation has a dimension different from the scales and amplitudes.";
  isDiagonal_ = spatialCorrelation.isDiagonal();
  if (!isDiagonal_) spatialCorrelation_ = spatialCorrelation;
  setAmplitude(amplitude);
  setScale(scale);
  updateSpatialCovariance();
}

SpectralModelImplementation::SpectralModelImplementation(const Point & scale,
    const CovarianceMatrix & spatialCovariance)
  : PersistentObject()
  , dimension_(spatialCovariance.getDimension())
  , scale_(0)
  , amplitude_(0)
  , spatialDimension_(scale.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(false)
{
  // Check scale values
  setScale(scale);
  spatialCovariance_ = HermitianMatrix(dimension_);
  amplitude_ = Point(dimension_);
  // As spatialCovariance is a covariance matrix, we convert it into an HermitianMatrix
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    amplitude_[i] = sqrt(spatialCovariance(i, i));
    spatialCovariance_(i, i) = spatialCovariance(i, i);
  }
  // Convert the spatial covariance into a spatial correlation
  isDiagonal_ = spatialCovariance.isDiagonal();
  if (!isDiagonal_)
  {
    spatialCorrelation_ = CorrelationMatrix(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        spatialCorrelation_(i, j) = spatialCovariance(i, j) / (amplitude_[i] * amplitude_[j]);
        spatialCovariance_(i, j) = spatialCovariance(i, j);
      }
    }
  }
}

/* Virtual constructor */
SpectralModelImplementation * SpectralModelImplementation::clone() const
{
  return new SpectralModelImplementation(*this);
}

/* Dimension accessor */
UnsignedInteger SpectralModelImplementation::getDimension() const
{
  return dimension_;
}

/* Dimension accessor */
void SpectralModelImplementation::setDimension(const UnsignedInteger dimension)
{
  dimension_ = dimension;
}

/* Dimension accessor */
UnsignedInteger SpectralModelImplementation::getSpatialDimension() const
{
  return spatialDimension_;
}

/* Computation of the spectral density function */
HermitianMatrix SpectralModelImplementation::operator() (const Scalar frequency) const
{
  // Spectral density is given as the Fourier transform of the stationary covariance function
  // With the formal expression of stationary covariance, ie C(x,y) = S * rho( |x- y|/|scale|),
  // the dsp writes as S * \hat{rho}(f)
  NumericalComplex rho = computeStandardRepresentative(frequency);
  return spatialCovariance_ * rho;
}

/** Standard representative */
NumericalComplex SpectralModelImplementation::computeStandardRepresentative(const Scalar frequency) const
{
  throw NotYetImplementedException(HERE) << "In SpectralModelImplementation::computeStandardRepresentative(const Scalar frequency) const";
}

/* Amplitude accessor */
Point SpectralModelImplementation::getAmplitude() const
{
  return amplitude_;
}

void SpectralModelImplementation::setAmplitude(const Point & amplitude)
{
  for (UnsignedInteger index = 0; index < dimension_; ++index)
    if (amplitude[index] <= 0)
      throw InvalidArgumentException(HERE) << "Error - The component " << index << " of amplitude is non positive" ;
  amplitude_ = amplitude;
  updateSpatialCovariance();
}

/* Scale accessor */
Point SpectralModelImplementation::getScale() const
{
  return scale_;
}

void SpectralModelImplementation::setScale(const Point & scale)
{
  for (UnsignedInteger index = 0; index < spatialDimension_; ++index)
    if (scale[index] <= 0)
      throw InvalidArgumentException(HERE) << "Error - The component " << index << " of scale is non positive" ;
  scale_ = scale;
}

/* Spatial correlation accessor */
CorrelationMatrix SpectralModelImplementation::getSpatialCorrelation() const
{
  if (!isDiagonal_) return spatialCorrelation_;
  return CorrelationMatrix(dimension_);
}

void SpectralModelImplementation::updateSpatialCovariance()
{
  spatialCovariance_ = HermitianMatrix(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    spatialCovariance_(j, j) = amplitude_[j] * amplitude_[j];
    if (!isDiagonal_)
    {
      for (UnsignedInteger i = j + 1; i < dimension_; ++i)
        spatialCovariance_(i, j) = spatialCorrelation_(i , j) * amplitude_[i] * amplitude_[j];
    }
  }
}

/* String converter */
String SpectralModelImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SpectralModelImplementation::GetClassName();
  return oss;
}

/* String converter */
String SpectralModelImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class=" << SpectralModelImplementation::GetClassName();
  return oss;
}

/* Drawing method */
Graph SpectralModelImplementation::draw(const UnsignedInteger rowIndex,
                                        const UnsignedInteger columnIndex,
                                        const Scalar minimumFrequency,
                                        const Scalar maximumFrequency,
                                        const UnsignedInteger frequencyNumber,
                                        const Bool module) const
{
  if (rowIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given row index must be less than " << dimension_ << ", here rowIndex=" << rowIndex;
  if (columnIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given column index must be less than " << dimension_ << ", here columnIndex=" << columnIndex;
  Sample data(frequencyNumber, 2);
  for (UnsignedInteger i = 0; i < frequencyNumber; ++i)
  {
    const Scalar f = (i * minimumFrequency + (frequencyNumber - i - 1.0) * maximumFrequency) / (frequencyNumber - 1.0);
    const NumericalComplex value((*this)(f)(rowIndex, columnIndex));
    data[i][0] = f;
    if (module) data[i][1] = std::abs(value);
    else data[i][1] = std::arg(value);
  }
  Graph graph(getName(), "f", String("spectral density") + (module ? String("module") : String("phase")), true, "topright");
  Curve curve(data);
  curve.setLineWidth(2);
  if (module) curve.setColor("red");
  else curve.setColor("blue");
  graph.add(curve);
  return graph;
}

/* Method save() stores the object through the StorageManager */
void SpectralModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "dimension_", dimension_);
  adv.saveAttribute( "scale_", scale_);
  adv.saveAttribute( "amplitude_", amplitude_);
  adv.saveAttribute( "spatialDimension_", spatialDimension_);
  adv.saveAttribute( "spatialCorrelation_", spatialCorrelation_);
  adv.saveAttribute( "spatialCovariance_", spatialCovariance_);
  adv.saveAttribute( "isDiagonal_", isDiagonal_);
}

/* Method load() reloads the object from the StorageManager */
void SpectralModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "dimension_", dimension_);
  adv.loadAttribute( "scale_", scale_);
  adv.loadAttribute( "amplitude_", amplitude_);
  adv.loadAttribute( "spatialDimension_", spatialDimension_);
  adv.loadAttribute( "spatialCorrelation_", spatialCorrelation_);
  adv.loadAttribute( "spatialCovariance_", spatialCovariance_);
  adv.loadAttribute( "isDiagonal_", isDiagonal_);
}

END_NAMESPACE_OPENTURNS
