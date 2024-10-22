//                                               -*- C++ -*-
/**
 *  @brief The class that implements linear combination of functions.
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
#ifndef OPENTURNS_LINEARCOMBINATIONFUNCTION_HXX
#define OPENTURNS_LINEARCOMBINATIONFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class LinearCombinationFunction
 *
 */
class OT_API LinearCombinationFunction
  : public Function
{
  CLASSNAME
public:
  /* Default constructor */
  LinearCombinationFunction();

  /** Parameter constructor */
  LinearCombinationFunction(const FunctionCollection & functionCollection,
                            const Point & coefficients);

  /** Comparison operator */
  using Function::operator ==;
  Bool operator ==(const LinearCombinationFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class LinearCombinationFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARCOMBINATIONFUNCTION_HXX */
