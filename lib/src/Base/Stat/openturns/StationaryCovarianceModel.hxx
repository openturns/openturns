//                                               -*- C++ -*-
/**
 *  @brief This class enables to build a covariance model
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_STATIONARYCOVARIANCEMODEL_HXX
#define OPENTURNS_STATIONARYCOVARIANCEMODEL_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class StationaryCovarianceModel
 */

class OT_API StationaryCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Constructor with input dimension */
  explicit StationaryCovarianceModel(const UnsignedInteger inputDimension = 1);

  /** Standard constructor with scale and amplitude parameters parameters */
  StationaryCovarianceModel(const Point & scale,
                            const Point & amplitude);

  /** Standard constructor with scale, amplitude and spatial correlation parameters parameters */
  StationaryCovarianceModel(const Point & scale,
                            const Point & amplitude,
                            const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale and spatial covariance parameters parameters */
  StationaryCovarianceModel(const Point & scale,
                            const CovarianceMatrix & spatialCovariance);

  /** Virtual copy constructor */
  virtual StationaryCovarianceModel * clone() const;

  /** Computation of the covariance matrix */
  using CovarianceModelImplementation::operator();
  virtual CovarianceMatrix operator() (const Point & s,
                                       const Point & t) const;
  virtual CovarianceMatrix operator() (const Point & tau) const;
  using CovarianceModelImplementation::computeAsScalar;
  virtual Scalar computeAsScalar (const Point & s,
                                  const Point & t) const;
  virtual Scalar computeAsScalar (const Point & tau) const;

  /** Computation of the covariance matrix */
  using CovarianceModelImplementation::computeStandardRepresentative;
  virtual Scalar computeStandardRepresentative(const Point & s,
      const Point & t) const;

  virtual Scalar computeStandardRepresentative(const Point & tau) const;

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
