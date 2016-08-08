//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a spectral model using its spectral density function
 *  This is the interface class
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
#ifndef OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX
#define OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SpectralModelImplementation
 */

class OT_API SpectralModelImplementation
  : public PersistentObject
{

  CLASSNAME;

public:

  typedef Pointer<SpectralModelImplementation>    Implementation;

  /** Default constructor */
  SpectralModelImplementation();

  /** Standard constructor with amplitude and scale parameters parameters */
  SpectralModelImplementation(const NumericalPoint & amplitude,
                              const NumericalPoint & scale);

  /** Standard constructor with amplitude, scale and spatial correlation parameters parameters */
  SpectralModelImplementation(const NumericalPoint & amplitude,
                              const NumericalPoint & scale,
                              const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale and spatial covariance parameters parameters */
  SpectralModelImplementation(const NumericalPoint & scale,
                              const CovarianceMatrix & spatialCovariance);


  /** Virtual copy constructor */
  virtual SpectralModelImplementation * clone() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;
  UnsignedInteger getSpatialDimension() const;

  /** Computation of the spectral density function */
  virtual HermitianMatrix operator() (const NumericalScalar frequency) const;

  /** Standard representative */
  virtual NumericalComplex computeStandardRepresentative(const NumericalScalar frequency) const;

  /** Frequency grid accessors */
  virtual RegularGrid getFrequencyGrid() const;
  virtual void setFrequencyGrid(const RegularGrid & frequencyGrid);

  /** Amplitude accessor */
  virtual NumericalPoint getAmplitude() const;

  virtual void setAmplitude(const NumericalPoint & amplitude);

  /** Scale accessor */
  virtual NumericalPoint getScale() const;

  virtual void setScale(const NumericalPoint & scale);

  /** Spatial correlation accessor */
  virtual CorrelationMatrix getSpatialCorrelation() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /** Drawing method */
  virtual Graph draw(const UnsignedInteger rowIndex = 0,
                     const UnsignedInteger columnIndex = 0,
                     const Bool module = true) const;

protected:

  // set the covariance structure
  void updateSpatialCovariance();

  void setDimension(const UnsignedInteger dimension);

  /** dimension parameter */
  UnsignedInteger dimension_;

  /** Collection - Container for amplitude values  */
  NumericalPoint amplitude_;

  /** Collection - Container for scale values  */
  NumericalPoint scale_;

  /** Spatial dimension parameter */
  UnsignedInteger spatialDimension_;

  /** Correlation matrix of the spatial dependence structure */
  CorrelationMatrix spatialCorrelation_;

  /** Covariance matrix of the spatial dependence structure */
  HermitianMatrix spatialCovariance_;

  /** Flag to tell if the model is diagonal */
  Bool isDiagonal_;

  /** Frequency grid */
  RegularGrid frequencyGrid_;

} ; /* class SpectralModelImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX */
