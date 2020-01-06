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
#ifndef OPENTURNS_SPHERICALMODEL_HXX
#define OPENTURNS_SPHERICALMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StationaryCovarianceModel.hxx"
#include "openturns/Point.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

class RegularGrid;

/**
 * @class SphericalModel
 */

class OT_API SphericalModel
  : public StationaryCovarianceModel
{

  CLASSNAME

public:


  /** Constructor with input dimension */
  explicit SphericalModel(const UnsignedInteger inputDimension = 1);

  /** Standard constructor with amplitude and range parameters parameters */
  SphericalModel(const Point & scale,
                 const Point & amplitude,
                 const Scalar radius = 1);

  /** Virtual copy constructor */
  virtual SphericalModel * clone() const;

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

  /** Radius accessor */
  Scalar getRadius() const;
  void setRadius(const Scalar radius);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Parameter accessor */
  virtual void setFullParameter(const Point & parameter);
  virtual Point getFullParameter() const;
  virtual Description getFullParameterDescription() const;

private :

  Scalar radius_;

} ; /* class SphericalModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPHERICALMODEL_HXX */
