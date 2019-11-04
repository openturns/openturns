//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector, i.e. a
 *        composite random vector built upon a boolean function which
 *        is described by a threshold and a comparison operator. The
 *        antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_THRESHOLDEVENT_HXX
#define OPENTURNS_THRESHOLDEVENT_HXX

#include "openturns/RandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ThresholdEvent
 *
 *
 */
class OT_API ThresholdEvent
  : public RandomVector
{
  CLASSNAME

public:

  /** Default constructor */
  ThresholdEvent();

  /** Constructor from comparison operator */
  ThresholdEvent(const RandomVector & antecedent,
                 const ComparisonOperator & op,
                 const Scalar threshold);

  /** Constructor from Interval */
  ThresholdEvent(const RandomVector & antecedent,
                 const Interval & interval);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

}; /* class ThresholdEvent */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_THRESHOLDEVENT_HXX */
