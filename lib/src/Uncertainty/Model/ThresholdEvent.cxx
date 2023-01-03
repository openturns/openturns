//                                               -*- C++ -*-
/**
 * @brief The class that implements an event random vector, i.e. a
 *        composite random vector built upon a boolean function which
 *        is described by a threshold and a comparison operator. The
 *        antecedent of the event must be a "classical" composite random
 *        vector in the context of OpenTURNS.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/ThresholdEvent.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ThresholdEventImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ThresholdEvent)

/* Default constructor */
ThresholdEvent::ThresholdEvent()
  : RandomVector(new ThresholdEventImplementation())
{
  // Nothing to do
}

/* Constructor from RandomVector */
ThresholdEvent::ThresholdEvent(const RandomVector & antecedent,
                               const ComparisonOperator & op,
                               const Scalar threshold)
  : RandomVector(new ThresholdEventImplementation(antecedent, op, threshold))
{
  // Nothing to do
}


/* Constructor from RandomVector */
ThresholdEvent::ThresholdEvent(const RandomVector & antecedent,
                               const Interval & interval)
  : RandomVector(new ThresholdEventImplementation(antecedent, interval))
{
  // Nothing to do
}


/* String converter */
String ThresholdEvent::__repr__() const
{
  return getImplementation()->__repr__();
}

String ThresholdEvent::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}



END_NAMESPACE_OPENTURNS
