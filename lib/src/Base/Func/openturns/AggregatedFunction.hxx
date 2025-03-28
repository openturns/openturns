//                                               -*- C++ -*-
/**
 *  @brief The class that implements aggregated functions.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_AGGREGATEDFUNCTION_HXX
#define OPENTURNS_AGGREGATEDFUNCTION_HXX

#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class AggregatedFunction
 *
 */
class OT_API AggregatedFunction
  : public Function
{
  CLASSNAME
public:
  /* Default constructor */
  AggregatedFunction();

  /** Parameter constructor */
  explicit AggregatedFunction(const FunctionCollection & functionCollection);

  /** Comparison operator */
  using Function::operator ==;
  Bool operator ==(const AggregatedFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class QuadraticFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_AGGREGATEDFUNCTION_HXX */
