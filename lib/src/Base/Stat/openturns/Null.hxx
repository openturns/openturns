//                                               -*- C++ -*-
/**
 *  @brief This class is a top-level class for the history mechanism of
 *  simulation algorithms. It delegates to its children the effective
 *  history strategy.
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_NULL_HXX
#define OPENTURNS_NULL_HXX

#include "openturns/HistoryStrategyImplementation.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Null
 */

class OT_API Null
  : public HistoryStrategyImplementation
{

  CLASSNAME

public:


  /** Constructor with parameters */
  Null();

  /** Virtual constructor */
  Null * clone() const override;

  /** Store the point according to the strategy */
  void store(const Point & point) override;
  void store(const Sample & sample) override;

  /** String converter */
  String __repr__() const override;

protected:

} ; /* class Null */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NULL_HXX */
