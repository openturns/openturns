//                                               -*- C++ -*-
/**
 *  @brief This class is a top-level class for the history mechanism.
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_HISTORYSTRATEGY_HXX
#define OPENTURNS_HISTORYSTRATEGY_HXX

#include "openturns/HistoryStrategyImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class HistoryStrategy
 */

class OT_API HistoryStrategy
  : public TypedInterfaceObject<HistoryStrategyImplementation>
{

  CLASSNAME

public:

  typedef Pointer<HistoryStrategyImplementation> Implementation;

  /** Default constructor */
  HistoryStrategy();

  /** Constructor with parameters */
  HistoryStrategy(const HistoryStrategyImplementation & implementation);

  /** Constructor with parameters */
  HistoryStrategy(Implementation & p_implementation);

  /** Store the point according to the strategy */
  void store(const Point & point);
  void store(const Sample & sample);

  /** Clear the history storage and change dimension of Point stored */
  void setDimension(const UnsignedInteger dimension);

  /** Clear the history storage */
  void clear();

  /** History sample accessor */
  Sample getSample() const;

  /** String converter */
  String __repr__() const override;

} ; /* class HistoryStrategy */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HISTORYSTRATEGY_HXX */
