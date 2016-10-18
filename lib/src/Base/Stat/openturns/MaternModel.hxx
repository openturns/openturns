//                                               -*- C++ -*-
/**
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

  CLASSNAME;

public:

  /** Constructor based on spatial dimension*/
  explicit MaternModel(const UnsignedInteger spatialDimension = 1);

  /** Parameters constructor */
  MaternModel(const NumericalPoint & scale,
              const NumericalScalar nu);

  MaternModel(const NumericalPoint & scale,
              const NumericalPoint & amplitude,
              const NumericalScalar nu);

  /** Virtual copy constructor */
  MaternModel * clone() const;

  /** Computation of the covariance function */
  using StationaryCovarianceModel::computeStandardRepresentative;
  NumericalScalar computeStandardRepresentative(const NumericalPoint & tau) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  // Reimplement the setScale method
  using StationaryCovarianceModel::setScale;
  void setScale(const NumericalPoint & scale);

  /** Nu accessor */
  NumericalScalar getNu() const;
  void setNu(const NumericalScalar nu);

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
  virtual void setFullParameter(const NumericalPoint & parameter);
  virtual NumericalPoint getFullParameter() const;
  virtual Description getFullParameterDescription() const;

private:

  void initialize();

  // The shape parameter
  NumericalScalar nu_;

  // The normalization factor
  NumericalScalar logNormalizationFactor_;

  // Scaling factor
  NumericalPoint sqrt2nuOverTheta_;

} ; /* class MaternModel */

END_NAMESPACE_OPENTURNS

#endif
