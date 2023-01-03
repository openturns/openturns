//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build an exponential covariance
 *  model, a second order model's implementation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_EXPONENTIALMODEL_HXX
#define OPENTURNS_EXPONENTIALMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/CovarianceModelImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

class RegularGrid;

/**
 * @class ExponentialModel
 */

class OT_API ExponentialModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:


  /** Constructor with input dimension */
  explicit ExponentialModel(const UnsignedInteger inputDimension = 1);


  /** Standard constructor with scale and amplitude parameters parameters */
  ExponentialModel(const Point & scale,
                   const Point & amplitude);

  /** Standard constructor with scale, amplitude and spatial correlation parameters */
  ExponentialModel(const Point & scale,
                   const Point & amplitude,
                   const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale and spatial covariance parameters */
  ExponentialModel(const Point & scale,
                   const CovarianceMatrix & spatialCovariance);

  /** Virtual copy constructor */
  ExponentialModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator()(const Point &tau) const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point &tau) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
                         const Collection<Scalar>::const_iterator &t_begin) const override;
#endif
  Scalar computeAsScalar(const Scalar tau) const override;

  /** Gradient */
  using CovarianceModelImplementation::partialGradient;
  Matrix partialGradient(const Point & s, const Point & t) const override;

  /** Discretize the covariance function on a given TimeGrid */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const RegularGrid & regularGrid) const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private :

} ; /* class ExponentialModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPONENTIALMODEL_HXX */
