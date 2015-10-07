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
#ifndef OPENTURNS_STATIONARYCOVARIANCEMODEL_HXX
#define OPENTURNS_STATIONARYCOVARIANCEMODEL_HXX

#include "CovarianceModel.hxx"
#include "Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StationaryCovarianceModel
 */

class OT_API StationaryCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME;

public:

  /** Constructor with spatial dimension */
  explicit StationaryCovarianceModel(const UnsignedInteger spatialDimension = 1);

  /** Standard constructor with amplitude and scale parameters parameters */
  StationaryCovarianceModel(const UnsignedInteger spatialDimension,
                            const NumericalPoint & amplitude,
                            const NumericalPoint & scale);

  /** Standard constructor with amplitude, scale and spatial correlation parameters parameters */
  StationaryCovarianceModel(const UnsignedInteger spatialDimension,
                            const NumericalPoint & amplitude,
                            const NumericalPoint & scale,
                            const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale and spatial covariance parameters parameters */
  StationaryCovarianceModel(const UnsignedInteger spatialDimension,
                            const NumericalPoint & scale,
                            const CovarianceMatrix & spatialCovariance);

  /** Virtual copy constructor */
  virtual StationaryCovarianceModel * clone() const;

  /** Computation of the covariance matrix */
  using CovarianceModelImplementation::operator();
  virtual CovarianceMatrix operator() (const NumericalPoint & s,
                                       const NumericalPoint & t) const;
  virtual CovarianceMatrix operator() (const NumericalPoint & tau) const;
  using CovarianceModelImplementation::computeAsScalar;
  virtual NumericalScalar computeAsScalar (const NumericalPoint & s,
                                           const NumericalPoint & t) const;
  virtual NumericalScalar computeAsScalar (const NumericalPoint & tau) const;

  /** Computation of the covariance matrix */
  using CovarianceModelImplementation::computeStandardRepresentative;
  virtual NumericalScalar computeStandardRepresentative(const NumericalPoint & s,
                                                        const NumericalPoint & t) const;

  virtual NumericalScalar computeStandardRepresentative(const NumericalPoint & tau) const;

  /** Discretize the covariance function on a given TimeGrid */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const RegularGrid & timeGrid) const;

  /** Is the underlying a stationary covariance model ? */
  Bool isStationary() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

} ; /* class StationaryCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STATIONARYCOVARIANCEMODEL_HXX */
