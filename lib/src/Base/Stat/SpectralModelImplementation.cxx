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
#include "openturns/SpectralModelImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpectralModelImplementation);
static const Factory<SpectralModelImplementation> RegisteredFactory;

/* Constructor with parameters */
SpectralModelImplementation::SpectralModelImplementation()
  : PersistentObject()
  , dimension_(1)
  , amplitude_(NumericalPoint(1, 1.0))
  , scale_(NumericalPoint(1, 1.0))
  , spatialDimension_(1)
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
  , frequencyGrid_()
{
  updateSpatialCovariance();
}

SpectralModelImplementation::SpectralModelImplementation(const NumericalPoint & amplitude,
                                                         const NumericalPoint & scale)
  : PersistentObject()
  , dimension_(amplitude.getDimension())
  , amplitude_(0)
  , scale_(0)
  , spatialDimension_(scale.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(true)
  , frequencyGrid_()
{
  setAmplitude(amplitude);
  setScale(scale);
  updateSpatialCovariance();
}

SpectralModelImplementation::SpectralModelImplementation(const NumericalPoint & amplitude,
                                                         const NumericalPoint & scale,
                                                         const CorrelationMatrix & spatialCorrelation)
  : PersistentObject()
  , dimension_(amplitude.getDimension())
  , amplitude_(0)
  , scale_(0)
  , spatialDimension_(scale.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(false)
  , frequencyGrid_()
{
  if (spatialCorrelation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given spatial correlation has a dimension different from the scales and amplitudes.";
  isDiagonal_ = spatialCorrelation.isDiagonal();
  if (!isDiagonal_) spatialCorrelation_ = spatialCorrelation;
  setAmplitude(amplitude);
  setScale(scale);
  updateSpatialCovariance();
}

SpectralModelImplementation::SpectralModelImplementation(const NumericalPoint & scale,
                                                         const CovarianceMatrix & spatialCovariance)
  : PersistentObject()
  , dimension_(spatialCovariance.getDimension())
  , amplitude_(0)
  , scale_(0)
  , spatialDimension_(scale.getDimension())
  , spatialCorrelation_(0)
  , spatialCovariance_(0)
  , isDiagonal_(false)
  , frequencyGrid_()
{
  // Check scale values
  setScale(scale);
  spatialCovariance_ = HermitianMatrix(dimension_);
  amplitude_ = NumericalPoint(dimension_);
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

/* Frequency grid accessors */
RegularGrid SpectralModelImplementation::getFrequencyGrid() const
{
  return frequencyGrid_;
}

void SpectralModelImplementation::setFrequencyGrid(const RegularGrid & frequencyGrid)
{
  frequencyGrid_ = frequencyGrid;
}

/* Computation of the spectral density function */
HermitianMatrix SpectralModelImplementation::operator() (const NumericalScalar frequency) const
{
  // Spectral density is given as the Fourier transform of the stationary covariance function
  // With the formal expression of stationary covariance, ie C(x,y) = S * rho( |x- y|/|scale|),
  // the dsp writes as S * \hat{rho}(f)
  NumericalComplex rho = computeStandardRepresentative(frequency);
  return spatialCovariance_ * rho;
}

/** Standard representative */
NumericalComplex SpectralModelImplementation::computeStandardRepresentative(const NumericalScalar frequency) const
{
  throw NotYetImplementedException(HERE) << "In SpectralModelImplementation::computeStandardRepresentative(const NumericalScalar frequency) const";
}

/* Amplitude accessor */
NumericalPoint SpectralModelImplementation::getAmplitude() const
{
  return amplitude_;
}

void SpectralModelImplementation::setAmplitude(const NumericalPoint & amplitude)
{
  for (UnsignedInteger index = 0; index < dimension_; ++index)
    if (amplitude[index] <= 0)
      throw InvalidArgumentException(HERE) << "Error - The component " << index << " of amplitude is non positive" ;
  amplitude_ = amplitude;
  updateSpatialCovariance();
}

/* Scale accessor */
NumericalPoint SpectralModelImplementation::getScale() const
{
  return scale_;
}

void SpectralModelImplementation::setScale(const NumericalPoint & scale)
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
                                        const Bool module) const
{
  if (rowIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given row index must be less than " << dimension_ << ", here rowIndex=" << rowIndex;
  if (columnIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given column index must be less than " << dimension_ << ", here columnIndex=" << columnIndex;
  const UnsignedInteger n(frequencyGrid_.getN());
  const NumericalScalar fMin(frequencyGrid_.getStart());
  const NumericalScalar fStep(frequencyGrid_.getStep());
  NumericalSample data(n, 2);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    const NumericalScalar f(fMin + i * fStep);
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
  adv.saveAttribute( "amplitude_", amplitude_);
  adv.saveAttribute( "scale_", scale_);
  adv.saveAttribute( "spatialDimension_", spatialDimension_);
  adv.saveAttribute( "spatialCorrelation_", spatialCorrelation_);
  adv.saveAttribute( "spatialCovariance_", spatialCovariance_);
  adv.saveAttribute( "isDiagonal_", isDiagonal_);
  adv.saveAttribute( "frequencyGrid_", frequencyGrid_);
}

/* Method load() reloads the object from the StorageManager */
void SpectralModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "dimension_", dimension_);
  adv.loadAttribute( "amplitude_", amplitude_);
  adv.loadAttribute( "scale_", scale_);
  adv.loadAttribute( "spatialDimension_", spatialDimension_);
  adv.loadAttribute( "spatialCorrelation_", spatialCorrelation_);
  adv.loadAttribute( "spatialCovariance_", spatialCovariance_);
  adv.loadAttribute( "isDiagonal_", isDiagonal_);
  adv.loadAttribute( "frequencyGrid_", frequencyGrid_);
}

END_NAMESPACE_OPENTURNS
