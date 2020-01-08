//                                               -*- C++ -*-
/**
 *  @brief The class that implements an event based on a composite random vector whose
 *   antecedent has a spherical distribution
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
#ifndef OPENTURNS_STANDARDEVENT_HXX
#define OPENTURNS_STANDARDEVENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/Event.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class StandardEvent
 *
 * The class that implements an event based on a composite random vector whose
 * antecedent has a spherical distribution
 */
class OT_API StandardEvent
  : public RandomVector
{
  CLASSNAME

public:


  /** Default constructor */
  StandardEvent();

  /** Constructor from RandomVector */
  StandardEvent(const RandomVector & antecedent,
                const ComparisonOperator & op,
                const Scalar threshold);

  /** Constructor from Event */
  StandardEvent(const RandomVector & event);


protected:


private:

}; /* class StandardEvent */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STANDARDEVENT_HXX */
