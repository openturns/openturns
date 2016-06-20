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
#ifndef OPENTURNS_GENERALIZEDEXPONENTIAL_HXX
#define OPENTURNS_GENERALIZEDEXPONENTIAL_HXX

#include "openturns/StationaryCovarianceModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralizedExponential
 */

class OT_API GeneralizedExponential
  : public StationaryCovarianceModel
{

  CLASSNAME;

public:

  /** Default constructor */
  explicit GeneralizedExponential(const UnsignedInteger spatialDimension = 1);

  /** Parameters constructor */
  GeneralizedExponential(const NumericalPoint & theta,
                         const NumericalScalar p);

  GeneralizedExponential(const NumericalPoint & theta,
                         const NumericalPoint & sigma,
                         const NumericalScalar p);

  /** Virtual copy constructor */
  GeneralizedExponential * clone() const;

  /** Computation of the covariance function */
  using StationaryCovarianceModel::computeStandardRepresentative;
  NumericalScalar computeStandardRepresentative(const NumericalPoint & tau) const;

  /** Gradient */
  virtual Matrix partialGradient(const NumericalPoint & s,
                                 const NumericalPoint & t) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** Exponent of the model */
  NumericalScalar p_;

} ; /* class GeneralizedExponential */

END_NAMESPACE_OPENTURNS

#endif
