//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a second order model
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
#ifndef OPENTURNS_SECONDORDERMODELIMPLEMENTATION_HXX
#define OPENTURNS_SECONDORDERMODELIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/SpectralModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

class RegularGrid;

/**
 * @class SecondOrderModelImplementation
 */

class OT_API SecondOrderModelImplementation
  : public PersistentObject
{

  CLASSNAME;

public:

  /** Default constructor without parameters */
  SecondOrderModelImplementation();

  /** Constructor with covariance model and spectral model */
  SecondOrderModelImplementation(const CovarianceModel & covarianceModel,
                                 const SpectralModel & spectralModel);

  /** Virtual copy constructor */
  virtual SecondOrderModelImplementation * clone() const;

  /** Dimension accessor */
  virtual UnsignedInteger getSpatialDimension() const;
  virtual UnsignedInteger getDimension() const;

  /** Computation of the covariance matrix */
  virtual CovarianceMatrix computeCovariance(const NumericalScalar s,
                                             const NumericalScalar t) const;

  virtual CovarianceMatrix computeCovariance(const NumericalScalar tau) const;

  virtual CovarianceMatrix computeCovariance(const NumericalPoint & s,
                                             const NumericalPoint & t) const;

  virtual CovarianceMatrix computeCovariance(const NumericalPoint & tau) const;

  /** SpectralModel and Covariance model accessor */
  SpectralModel getSpectralModel() const;
  CovarianceModel getCovarianceModel() const;
  void setModels(const CovarianceModel & covarianceModel,
                 const SpectralModel & spectralModel);

  /** Discretize the covariance function on a given TimeGrid */
  virtual CovarianceMatrix discretize(const RegularGrid & timeGrid) const;

  /** Computation of the spectral density function */
  virtual HermitianMatrix computeSpectralDensity(const NumericalScalar frequency) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /** The covariance model part - must be stationary */
  CovarianceModel covarianceModel_;

  /** The spectral model part */
  SpectralModel spectralModel_;

} ; /* class SecondOrderModelImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SECONDORDERMODELIMPLEMENTATION_HXX */
