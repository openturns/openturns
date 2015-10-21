//                                               -*- C++ -*-
/**
 *  @brief This class enables to build a covariance model
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
#ifndef OPENTURNS_COVARIANCEMODELIMPLEMENTATION_HXX
#define OPENTURNS_COVARIANCEMODELIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "CovarianceMatrix.hxx"
#include "Pointer.hxx"
#include "RegularGrid.hxx"
#include "Mesh.hxx"
#include "Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModelImplementation
 */

class OT_API CovarianceModelImplementation
  : public PersistentObject
{

  CLASSNAME;

public:

  typedef Pointer<CovarianceModelImplementation> Implementation;

  /** Dimension-based constructor */
  explicit CovarianceModelImplementation(const UnsignedInteger spatialDimension = 1);

  /** Standard constructor with amplitude and scale parameters parameters */
  CovarianceModelImplementation(const UnsignedInteger spatialDimension,
                                const NumericalPoint & amplitude,
                                const NumericalPoint & scale);

  /** Standard constructor with amplitude and scale parameters parameters */
  CovarianceModelImplementation(const NumericalPoint & amplitude,
                                const NumericalPoint & scale);

  /** Standard constructor with amplitude, scale and spatial correlation parameters parameters */
  CovarianceModelImplementation(const UnsignedInteger spatialDimension,
                                const NumericalPoint & amplitude,
                                const NumericalPoint & scale,
                                const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with amplitude, scale and spatial correlation parameters parameters */
  CovarianceModelImplementation(const NumericalPoint & amplitude,
                                const NumericalPoint & scale,
                                const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale and spatial covariance parameters parameters */
  CovarianceModelImplementation(const UnsignedInteger spatialDimension,
                                const NumericalPoint & scale,
                                const CovarianceMatrix & spatialCovariance);

  /** Standard constructor with scale and spatial covariance parameters parameters */
  CovarianceModelImplementation(const NumericalPoint & scale,
                                const CovarianceMatrix & spatialCovariance);

  /** Virtual copy constructor */
  virtual CovarianceModelImplementation * clone() const;

  /** Dimensions accessors */
  /** Dimension of the argument */
  virtual UnsignedInteger getSpatialDimension() const;
  /** Dimension of the values */
  virtual UnsignedInteger getDimension() const;

  /** Compute the covariance function */
  virtual CovarianceMatrix operator() (const NumericalScalar s,
                                       const NumericalScalar t) const;
  virtual CovarianceMatrix operator() (const NumericalPoint & s,
                                       const NumericalPoint & t) const;

  // compute standard representative computes the term \rho(s, t)
  virtual NumericalScalar computeStandardRepresentative(const NumericalPoint & s,
                                                        const NumericalPoint & t) const;

  virtual NumericalScalar computeStandardRepresentative(const NumericalScalar & s,
                                                        const NumericalScalar & t) const;

  virtual NumericalScalar computeStandardRepresentative(const NumericalPoint & tau) const;

  virtual NumericalScalar computeStandardRepresentative(const NumericalScalar & tau) const;

  // Special case for 1D model
  virtual NumericalScalar computeAsScalar (const NumericalScalar s,
                                           const NumericalScalar t) const;

  virtual NumericalScalar computeAsScalar (const NumericalPoint & s,
                                           const NumericalPoint & t) const;

  virtual CovarianceMatrix operator() (const NumericalScalar tau) const;
  virtual CovarianceMatrix operator() (const NumericalPoint & tau) const;

  // Special case for 1D model
  virtual NumericalScalar computeAsScalar (const NumericalScalar tau) const;
  virtual NumericalScalar computeAsScalar (const NumericalPoint & tau) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  /** Discretize the covariance function on a given TimeGrid/Mesh */
  virtual CovarianceMatrix discretize(const RegularGrid & timeGrid) const;

  virtual CovarianceMatrix discretize(const Mesh & mesh) const;
  virtual CovarianceMatrix discretize(const NumericalSample & vertices) const;
  virtual NumericalSample discretizeRow(const NumericalSample & vertices,
                                        const UnsignedInteger p) const;

  /** Is it a stationary covariance model ? */
  virtual Bool isStationary() const;

  /** Is it a diagonal covariance model ? */
  virtual Bool isDiagonal() const;

  /** Amplitude accessors */
  virtual NumericalPoint getAmplitude() const;
  virtual void setAmplitude(const NumericalPoint & amplitude);

  /** Scale accessors */
  virtual NumericalPoint getScale() const;
  virtual void setScale(const NumericalPoint & scale);

  /** Spatial correlation accessors */
  virtual CorrelationMatrix getSpatialCorrelation() const;
  virtual void setSpatialCorrelation(const CorrelationMatrix & correlation);

  /** Nugget factor accessor */
  virtual void setNuggetFactor(const NumericalScalar nuggetFactor);
  virtual NumericalScalar getNuggetFactor() const;

  /** Parameters accessor */
  virtual void setParameters(const NumericalPoint & parameters);
  virtual NumericalPointWithDescription getParameters() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Marginal accessor */
  virtual Implementation getMarginal(const UnsignedInteger index) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  // set the covariance structure
  void updateSpatialCovariance();

  /** Input dimension parameter */
  UnsignedInteger spatialDimension_;

  /** Input dimension parameter */
  UnsignedInteger dimension_;

  /** Collection - Container for amplitude values  */
  NumericalPoint amplitude_;

  /** Collection - Container for scale values  */
  NumericalPoint scale_;

  /** Correlation matrix of the spatial dependence structure */
  CorrelationMatrix spatialCorrelation_;

  /** Covariance matrix of the spatial dependence structure */
  CovarianceMatrix spatialCovariance_;

  /** Flag to tell if the model is diagonal */
  Bool isDiagonal_;

  /** Nugget factor */
  NumericalScalar nuggetFactor_;

} ; /* class CovarianceModelImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COVARIANCEMODELIMPLEMENTATION_HXX */
