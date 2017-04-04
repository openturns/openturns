//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build an exponential covariance
 *  model, a second order model's implementation
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
#ifndef OPENTURNS_EXPONENTIALCAUCHYMODEL_HXX
#define OPENTURNS_EXPONENTIALCAUCHYMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/SecondOrderModelImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS


class RegularGrid;

/**
 * @class ExponentialCauchy
 */

class OT_API ExponentialCauchy
  : public SecondOrderModelImplementation
{

  CLASSNAME;

public:


  /** Default constructor without parameters */
  ExponentialCauchy();

  /** Standard constructor with scale and amplitude parameters parameters */
  ExponentialCauchy(const Point & scale,
                    const Point & amplitude);

  /** Virtual copy constructor */
  virtual ExponentialCauchy * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Amplitude accessor */
  Point getAmplitude() const;

  /** Scale accessor */
  Point getScale() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private :

} ; /* class SecondOrderModelImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPONENTIALCAUCHYMODEL_HXX */
