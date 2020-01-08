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
#ifndef OPENTURNS_HANNINGFILTER_HXX
#define OPENTURNS_HANNINGFILTER_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/FilteringWindowsImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Hanning
 */

class OT_API Hanning
  : public FilteringWindowsImplementation
{

  CLASSNAME

public:


  /** Default constructor without parameters */
  Hanning();

  /** Virtual copy constructor */
  virtual Hanning * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Evaluation method  */
  Scalar operator()(const Scalar t) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

} ; /* class Hanning */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HANNINGFILTER_HXX */
