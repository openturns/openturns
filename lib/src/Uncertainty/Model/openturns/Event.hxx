//                                               -*- C++ -*-
/**
 *  @brief This class specializes the concept of RandomVector by constructing a Bernoulli
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_EVENT_HXX
#define OPENTURNS_EVENT_HXX

#include "openturns/RandomVector.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Event
 *
 * The class that implements all random vectors
 */
class OT_API Event
  : public RandomVector
{
  CLASSNAME

public:


  /** for save/load mecanism */
  Event();

  /** Constructor from RandomVector */
  Event(const RandomVector & antecedent,
        const ComparisonOperator & op,
        const Scalar threshold);

  /** Constructor from RandomVector */
  Event(const RandomVector & antecedent,
        const Domain & domain);

  /** Constructor from RandomVector */
  Event(const RandomVector & antecedent,
        const Interval & interval);

  /** Constructor from RandomVector */
  Event(const Process &  process,
        const Domain & domain);

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

private:

}; /* class Event */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EVENT_HXX */
