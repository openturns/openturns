//                                               -*- C++ -*-
/**
 *  @brief The class that implements quadratic numerical math functions.
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_QUADRATICNUMERICALMATHFUNCTION_HXX
#define OPENTURNS_QUADRATICNUMERICALMATHFUNCTION_HXX

#include "NumericalMathFunction.hxx"
#include "NumericalPoint.hxx"
#include "Matrix.hxx"
#include "SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class QuadraticNumericalMathFunction
 *
 * The class that simulates a linear numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the function, the gradient or the hessian.
 */
class OT_API QuadraticNumericalMathFunction
  : public NumericalMathFunction
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */

  /** Default constructor */
  QuadraticNumericalMathFunction (const NumericalPoint & center,
                                  const NumericalPoint & constant,
                                  const Matrix & linear,
                                  const SymmetricTensor & quadratic);


  /** Comparison operator */
  Bool operator ==(const QuadraticNumericalMathFunction & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

}; /* class QuadraticNumericalMathFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICNUMERICALMATHFUNCTION_HXX */
