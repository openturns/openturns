//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_ABSOLUTEEXPONENTIAL_HXX
#define OPENTURNS_ABSOLUTEEXPONENTIAL_HXX

#include "openturns/StationaryCovarianceModel.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AbsoluteExponential
 */

class OT_API AbsoluteExponential
  : public StationaryCovarianceModel
{

  CLASSNAME;

public:

  /** Constructor based on spatial dimension*/
  explicit AbsoluteExponential(const UnsignedInteger spatialDimension = 1);

  /** Parameters constructor */
  AbsoluteExponential(const NumericalPoint & scale);

  /** Parameters constructor */
  AbsoluteExponential(const NumericalPoint & scale,
                      const NumericalPoint & amplitude);

  /** Virtual copy constructor */
  AbsoluteExponential * clone() const;

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

} ; /* class AbsoluteExponential */

END_NAMESPACE_OPENTURNS

#endif
