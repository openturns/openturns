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
#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StationaryCovarianceModel
 */

CLASSNAMEINIT(StationaryCovarianceModel);

static const Factory<StationaryCovarianceModel> Factory_StationaryCovarianceModel;

/* Constructor with parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const UnsignedInteger spatialDimension)
  : CovarianceModelImplementation(spatialDimension)
{
  // Nothing to do
}

/* Constructor with parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const UnsignedInteger spatialDimension,
                                                     const NumericalPoint & amplitude,
                                                     const NumericalPoint & scale)
  : CovarianceModelImplementation(spatialDimension, amplitude, scale)
{
  // Nothing to do
}

/** Standard constructor with amplitude and scale parameters parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const NumericalPoint & amplitude,
                                                     const NumericalPoint & scale)
  : CovarianceModelImplementation(amplitude, scale)
{
  // Nothing to do
}

StationaryCovarianceModel::StationaryCovarianceModel(const UnsignedInteger spatialDimension,
                                                     const NumericalPoint & amplitude,
                                                     const NumericalPoint & scale,
                                                     const CorrelationMatrix & spatialCorrelation)
  : CovarianceModelImplementation(spatialDimension, amplitude, scale, spatialCorrelation)
{
  // Nothing to do
}

/** Standard constructor with amplitude, scale and spatial correlation parameters parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const NumericalPoint & amplitude,
                                                     const NumericalPoint & scale,
                                                     const CorrelationMatrix & spatialCorrelation)
  : CovarianceModelImplementation(amplitude, scale, spatialCorrelation)
{
  // Nothing to do
}

StationaryCovarianceModel::StationaryCovarianceModel(const UnsignedInteger spatialDimension,
                                                     const NumericalPoint & scale,
                                                     const CovarianceMatrix & spatialCovariance)
  : CovarianceModelImplementation(spatialDimension, scale, spatialCovariance)
{
  // Nothing to do
}

/** Standard constructor with scale and spatial covariance parameters parameters */
StationaryCovarianceModel::StationaryCovarianceModel(const NumericalPoint & scale,
                                                     const CovarianceMatrix & spatialCovariance)
  : CovarianceModelImplementation(scale, spatialCovariance)
{
  // Nothing to do
}

/* Virtual constructor */
StationaryCovarianceModel * StationaryCovarianceModel::clone() const
{
  return new StationaryCovarianceModel(*this);
}

/* Computation of the covariance function */
CovarianceMatrix StationaryCovarianceModel::operator() (const NumericalPoint & s,
                                                        const NumericalPoint & t) const
{
  return (*this)(t - s);
}

CovarianceMatrix StationaryCovarianceModel::operator() (const NumericalPoint & tau) const
{
  const NumericalScalar rho = computeStandardRepresentative(tau);
  return CovarianceMatrix((spatialCovariance_ * rho).getImplementation());
}

NumericalScalar StationaryCovarianceModel::computeAsScalar(const NumericalPoint & s,
                                                           const NumericalPoint & t) const
{
  return computeAsScalar(t - s);
}

NumericalScalar StationaryCovarianceModel::computeAsScalar(const NumericalPoint & tau) const
{
  return (*this)(tau)(0, 0);
}

NumericalScalar StationaryCovarianceModel::computeStandardRepresentative(const NumericalPoint & s,
                                                                         const NumericalPoint & t) const
{
  return computeStandardRepresentative(t - s);
}

NumericalScalar StationaryCovarianceModel::computeStandardRepresentative(const NumericalPoint & tau) const
{
  throw NotYetImplementedException(HERE) << "In StationaryCovarianceModel::computeStandardRepresentative (const NumericalPoint & tau) const";
}

/* Discretize the covariance function on a given TimeGrid */
CovarianceMatrix StationaryCovarianceModel::discretize(const RegularGrid & timeGrid) const
{
  const UnsignedInteger size = timeGrid.getN();
  const NumericalScalar timeStep = timeGrid.getStep();
  const UnsignedInteger fullSize = size * dimension_;
  CovarianceMatrix covarianceMatrix(fullSize);

  // Fill-in the matrix by blocks
  for (UnsignedInteger rowIndex = 0; rowIndex < size; ++rowIndex)
  {
    // Only the lower part has to be filled-in
    const UnsignedInteger rowBase = rowIndex * dimension_;
    for (UnsignedInteger columnIndex = 0; columnIndex < rowIndex; ++columnIndex)
    {
      const UnsignedInteger columnBase = columnIndex * dimension_;
      const CovarianceMatrix localCovarianceMatrix(operator()( rowIndex * timeStep,  columnIndex * timeStep) );
      // We fill the covariance matrix using the previous local one
      // The full local covariance matrix has to be copied as it is
      // not copied on a symmetric position
      for (UnsignedInteger columnIndexLocal = 0; columnIndexLocal < dimension_; ++columnIndexLocal)
        for (UnsignedInteger rowIndexLocal = 0; rowIndexLocal < dimension_; ++rowIndexLocal)
          covarianceMatrix(rowBase + rowIndexLocal, columnBase + columnIndexLocal) = localCovarianceMatrix(rowIndexLocal, columnIndexLocal) ;
    } // column index of the block
  } // row index of the block
  return covarianceMatrix;
}

/* Is the underlying a stationary covariance model ? */
Bool StationaryCovarianceModel::isStationary() const
{
  return true;
}

/* String converter */
String StationaryCovarianceModel::__repr__() const
{
  OSS oss(true);
  oss << "class= " << StationaryCovarianceModel::GetClassName();
  return oss;
}

/* String converter */
String StationaryCovarianceModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class= " << StationaryCovarianceModel::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void StationaryCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void StationaryCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
