//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build an exponential covariance
 *  model, a second order model's implementation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_EXPONENTIALLYDAMPEDCOSINEMODEL_HXX
#define OPENTURNS_EXPONENTIALLYDAMPEDCOSINEMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/Point.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

class RegularGrid;

/**
 * @class ExponentiallyDampedCosineModel
 */

class OT_API ExponentiallyDampedCosineModel
  : public StationaryCovarianceModel
{

  CLASSNAME

public:


  /** Constructor with input dimension */
  explicit ExponentiallyDampedCosineModel(const UnsignedInteger inputDimension = 1);

  /** Standard constructor with scale and amplitude parameters */
  ExponentiallyDampedCosineModel(const Point & scale,
                                 const Point & amplitude,
                                 const Scalar frequency);

  /** Virtual copy constructor */
  virtual ExponentiallyDampedCosineModel * clone() const;

  /** Computation of the covariance function, stationary interface */
  using StationaryCovarianceModel::computeStandardRepresentative;
  Scalar computeStandardRepresentative(const Point & tau) const;
#ifndef SWIG
  Scalar computeStandardRepresentative(const Collection<Scalar>::const_iterator & s_begin,
                                       const Collection<Scalar>::const_iterator & t_begin) const;
#endif

  using StationaryCovarianceModel::operator();
  CovarianceMatrix operator() (const Point & tau) const;
  Scalar computeAsScalar(const Point & tau) const;

  /** Discretize the covariance function on a given TimeGrid */
  using StationaryCovarianceModel::discretize;
  CovarianceMatrix discretize(const RegularGrid & regularGrid) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Frequency accessor */
  void setFrequency(const Scalar frequency);
  Scalar getFrequency() const;

protected:

  /** Parameter accessor */
  virtual void setFullParameter(const Point & parameter);
  virtual Point getFullParameter() const;
  virtual Description getFullParameterDescription() const;

public:

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private :

  /** Frequency */
  Scalar frequency_;

} ; /* class ExponentiallyDampedCosineModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPONENTIALLYDAMPEDCOSINEMODEL_HXX */
