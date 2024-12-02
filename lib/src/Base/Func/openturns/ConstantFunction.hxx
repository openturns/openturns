//                                               -*- C++ -*-
/**
 *  @brief The class that implements constant functions.
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
#ifndef OPENTURNS_CONSTANTFUNCTION_HXX
#define OPENTURNS_CONSTANTFUNCTION_HXX

#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ConstantFunction
 *
 * The class that simulates a constant function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the function, the gradient or the hessian.
 */
class OT_API ConstantFunction
  : public Function
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  explicit ConstantFunction(const UnsignedInteger inputDimension = 1,
                            const Point & constant = {0.0});

  /** Comparison operator */
  using Function::operator ==;
  Bool operator ==(const ConstantFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class ConstantFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONSTANTFUNCTION_HXX */
