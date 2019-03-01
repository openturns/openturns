//                                               -*- C++ -*-
/**
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
#ifndef OPENTURNS_MATERNMODEL_HXX
#define OPENTURNS_MATERNMODEL_HXX

#include "openturns/StationaryCovarianceModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MaternModel
 */

class OT_API MaternModel
  : public StationaryCovarianceModel
{

  CLASSNAME

public:
  typedef CovarianceModelImplementation::ScaleParametrization ScaleParametrization;

  /** Constructor based on input dimension*/
  explicit MaternModel(const UnsignedInteger inputDimension = 1);

  /** Parameters constructor */
  MaternModel(const Point & scale,
              const Scalar nu);

  MaternModel(const Point & scale,
              const Point & amplitude,
              const Scalar nu);

  /** Virtual copy constructor */
  MaternModel * clone() const;

  /** Computation of the covariance function */
  using StationaryCovarianceModel::computeStandardRepresentative;
  Scalar computeStandardRepresentative(const Point & tau) const;
#ifndef SWIG
  Scalar computeStandardRepresentative(const Collection<Scalar>::const_iterator & s_begin,
                                       const Collection<Scalar>::const_iterator & t_begin) const;
#endif
  virtual void setScaleParametrization(const ScaleParametrization scaleParametrization);

  /** Gradient */
  virtual Matrix partialGradient(const Point & s,
                                 const Point & t) const;

  // Reimplement the setScale method
  using StationaryCovarianceModel::setScale;
  void setScale(const Point & scale);

  /** Nu accessor */
  Scalar getNu() const;
  void setNu(const Scalar nu);

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Parameter accessor */
  virtual void setFullParameter(const Point & parameter);
  virtual Point getFullParameter() const;
  virtual Description getFullParameterDescription() const;

private:

  // Private methods to set internal parameters
  void computeLogNormalizationFactor();
  void computeSqrt2nuOverTheta();

  // The shape parameter
  Scalar nu_;

  // The normalization factor
  Scalar logNormalizationFactor_;

  // Scaling factor
  Point sqrt2nuOverTheta_;

} ; /* class MaternModel */

END_NAMESPACE_OPENTURNS

#endif
