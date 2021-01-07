//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a spectral model using its spectral density function
 *  This is the interface class
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

  CLASSNAME

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
  UnsignedInteger getOutputDimension() const;
  UnsignedInteger getInputDimension() const;

  /** Computation of the spectral density function */
  virtual HermitianMatrix operator() (const Scalar frequency) const;

  /** Standard representative */
  virtual Complex computeStandardRepresentative(const Scalar frequency) const;

  /** Drawing method */
  virtual Graph draw(const UnsignedInteger rowIndex = 0,
                     const UnsignedInteger columnIndex = 0,
                     const Scalar minimumFrequency = ResourceMap::GetAsScalar("SpectralModel-DefaultMinimumFrequency"),
                     const Scalar maximumFrequency = ResourceMap::GetAsScalar("SpectralModel-DefaultMaximumFrequency"),
                     const UnsignedInteger frequencyNumber = ResourceMap::GetAsUnsignedInteger("SpectralModel-DefaultFrequencyNumber"),
                     const Bool module = true) const;

  /** Amplitude accessor */
  Point getAmplitude() const;

  void setAmplitude(const Point & amplitude);

  /** Scale accessor */
  Point getScale() const;

  void setScale(const Point & scale);

  /** Spatial correlation accessor */
  CorrelationMatrix getOutputCorrelation() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

} ; /* class SpectralModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODEL_HXX */
