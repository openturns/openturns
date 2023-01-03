//                                               -*- C++ -*-
/**
 *  @brief The class that implements linear numerical math functions.
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
#ifndef OPENTURNS_LINEARFUNCTION_HXX
#define OPENTURNS_LINEARFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class LinearFunction
 *
 * The class that simulates a linear numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the function, the gradient or the hessian.
 */
class OT_API LinearFunction
  : public Function
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  LinearFunction(const Point & center,
                 const Point & constant,
                 const Matrix & linear);


  /** Comparison operator */
  Bool operator ==(const LinearFunction & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class LinearFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARFUNCTION_HXX */
