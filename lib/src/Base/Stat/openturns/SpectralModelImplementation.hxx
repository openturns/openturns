//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a spectral model using its spectral density function
 *  This is the interface class
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX
#define OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SpectralModelImplementation
 */

class OT_API SpectralModelImplementation
  : public PersistentObject
{

  CLASSNAME

public:

  typedef Pointer<SpectralModelImplementation>    Implementation;

  /** Default constructor */
  SpectralModelImplementation();

  /** Standard constructor with scale and amplitude parameters parameters */
  SpectralModelImplementation(const Point & scale,
                              const Point & amplitude);

  /** Standard constructor with scale, amplitude and spatial correlation parameters parameters */
  SpectralModelImplementation(const Point & scale,
                              const Point & amplitude,
                              const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale and spatial covariance parameters parameters */
  SpectralModelImplementation(const Point & scale,
                              const CovarianceMatrix & spatialCovariance);


  /** Virtual copy constructor */
  SpectralModelImplementation * clone() const override;

  /** Dimension accessor */
  UnsignedInteger getOutputDimension() const;
  UnsignedInteger getInputDimension() const;

  /** Computation of the spectral density function */
  virtual HermitianMatrix operator() (const Scalar frequency) const;

  /** Standard representative */
  virtual Complex computeStandardRepresentative(const Scalar frequency) const;

  /** Amplitude accessor */
  virtual Point getAmplitude() const;

  virtual void setAmplitude(const Point & amplitude);

  /** Scale accessor */
  virtual Point getScale() const;

  virtual void setScale(const Point & scale);

  /** Spatial correlation accessor */
  virtual CorrelationMatrix getOutputCorrelation() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Drawing method */
  virtual Graph draw(const UnsignedInteger rowIndex = 0,
                     const UnsignedInteger columnIndex = 0,
                     const Scalar minimumFrequency = ResourceMap::GetAsScalar("SpectralModel-DefaultMinimumFrequency"),
                     const Scalar maximumFrequency = ResourceMap::GetAsScalar("SpectralModel-DefaultMaximumFrequency"),
                     const UnsignedInteger frequencyNumber = ResourceMap::GetAsUnsignedInteger("SpectralModel-DefaultFrequencyNumber"),
                     const Bool module = true) const;

protected:

  // set the covariance structure
  void updateOutputCovariance();

  void setOutputDimension(const UnsignedInteger outputDimension);

  /** dimension parameter */
  UnsignedInteger outputDimension_;

  /** Container for scale values  */
  Point scale_;

  /** Container for amplitude values  */
  Point amplitude_;

  /** Spatial dimension parameter */
  UnsignedInteger inputDimension_;

  /** Correlation matrix of the output dependence structure */
  CorrelationMatrix outputCorrelation_;

  /** Covariance matrix of the output dependence structure */
  HermitianMatrix outputCovariance_;

  /** Flag to tell if the model is diagonal */
  Bool isDiagonal_;

} ; /* class SpectralModelImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECTRALMODELIMPLEMENTATION_HXX */
