//                                               -*- C++ -*-
/**
 *  @brief This class is a top-level class for the history mechanism.
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/HistoryStrategy.hxx"
#include "openturns/Null.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class HistoryStrategy
 */

CLASSNAMEINIT(HistoryStrategy)

/* Defaultonstructor */
HistoryStrategy::HistoryStrategy()
  : TypedInterfaceObject<HistoryStrategyImplementation>(Null().clone())
{
  // Nothing to do
}

/* Constructor with parameters */
HistoryStrategy::HistoryStrategy(Implementation & p_implementation)
  : TypedInterfaceObject<HistoryStrategyImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor with parameters */
HistoryStrategy::HistoryStrategy(const HistoryStrategyImplementation & implementation)
  : TypedInterfaceObject<HistoryStrategyImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Store the point according to the strategy */
void HistoryStrategy::store(const Point & point)
{
  copyOnWrite();
  getImplementation()->store(point);
}

void HistoryStrategy::store(const Sample & sample)
{
  copyOnWrite();
  getImplementation()->store(sample);
}

/* Clear the history storage and change dimension of Point stored */
void HistoryStrategy::setDimension(const UnsignedInteger dimension)
{
  copyOnWrite();
  getImplementation()->setDimension(dimension);
}

/* Clear the history storage */
void HistoryStrategy::clear()
{
  copyOnWrite();
  getImplementation()->clear();
}

/* History sample accessor */
Sample HistoryStrategy::getSample() const
{
  return getImplementation()->getSample();
}

/* String converter */
String HistoryStrategy::__repr__() const
{
  OSS oss;
  oss << "class=" << HistoryStrategy::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}



END_NAMESPACE_OPENTURNS
