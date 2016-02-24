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
#ifndef OPENTURNS_SPECTRALMODEL_HXX
#define OPENTURNS_SPECTRALMODEL_HXX

#include "openturns/SpectralModelImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SpectralModel
 */

class OT_API SpectralModel
  : public TypedInterfaceObject<SpectralModelImplementation>
{

  CLASSNAME;

public:

  typedef SpectralModelImplementation::Implementation Implementation;

  /** Default constructor */
  SpectralModel();

  /** Copy constructors */
  SpectralModel(const SpectralModelImplementation & implementation);

  /** Constructor from implementation */
  SpectralModel(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  SpectralModel(SpectralModelImplementation * p_implementation);
#endif


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

  /** Drawing method */
  Graph draw(const UnsignedInteger rowIndex = 0,
             const UnsignedInteger columnIndex = 0,
             const Bool module = true) const;

  /** Amplitude accessor */
  NumericalPoint getAmplitude() const;

  void setAmplitude(const NumericalPoint & amplitude);

  /** Scale accessor */
  NumericalPoint getScale() const;

  void setScale(const NumericalPoint & scale);

  /** Spatial correlation accessor */
  CorrelationMatrix getSpatialCorrelation() const;

/** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class SpectralModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODEL_HXX */
