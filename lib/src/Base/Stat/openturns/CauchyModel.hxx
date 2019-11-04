//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build an exponential covariance
 *  model, a second order model's implementation
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
#ifndef OPENTURNS_CAUCHYMODEL_HXX
#define OPENTURNS_CAUCHYMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/SpectralModel.hxx"
#include "openturns/Point.hxx"
#include "openturns/CorrelationMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class TimeGrid;

/**
 * @class CauchyModel
 */

class OT_API CauchyModel
  : public SpectralModelImplementation
{

  CLASSNAME

public:


  /** Default constructor without parameters */
  CauchyModel();

  /** Standard constructor with scale and amplitude parameters parameters */
  CauchyModel(const Point & scale,
              const Point & amplitude);

  /** Standard constructor with scale, amplitude and spatial correlation parameters parameters */
  CauchyModel(const Point & scale,
              const Point & amplitude,
              const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale and spatial covariance parameters parameters */
  CauchyModel(const Point & scale,
              const CovarianceMatrix & spatialCovariance);

  /** Virtual copy constructor */
  virtual CauchyModel * clone() const;

  /** Computation of the spectral density function */
  using SpectralModelImplementation::computeStandardRepresentative;
  Complex computeStandardRepresentative(const Scalar frequency) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

} ; /* class CauchyModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CAUCHYMODEL_HXX */
