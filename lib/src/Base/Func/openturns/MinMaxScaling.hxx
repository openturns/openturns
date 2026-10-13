//                                               -*- C++ -*-
/**
 *  @brief Min-max scaling
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MINMAXSCALING_HXX
#define OPENTURNS_MINMAXSCALING_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MinMaxScaling
 *
 * Min-max scaling, see issue #3104
 */
class OT_API MinMaxScaling
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  MinMaxScaling();

  /** Parameter constructor */
  MinMaxScaling(const Point & minimum,
                const Point & maximum);

  /** Virtual constructor */
  MinMaxScaling * clone() const override;

  /** Minimum accessor */
  Point getMinimum() const;

  /** Maximum accessor */
  Point getMaximum() const;

  /** Forward scaling accessor */
  Function getTransform() const;

  /** Inverse scaling accessor */
  Function getInverseTransform() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Lower bound */
  Point minimum_;

  /** Upper bound */
  Point maximum_;

} ; /* class MinMaxScaling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MINMAXSCALING_HXX */
