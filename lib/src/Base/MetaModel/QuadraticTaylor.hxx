//                                               -*- C++ -*-
/**
 *  @brief Second order polynomial response surface by Taylor expansion
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
#ifndef OPENTURNS_QUADRATICTAYLOR_HXX
#define OPENTURNS_QUADRATICTAYLOR_HXX

#include "OTprivate.hxx"
#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"
#include "Matrix.hxx"
#include "SymmetricTensor.hxx"
#include "NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class QuadraticTaylor
 *
 * The class describes the concept of quadratic taylor response surface
 */
class OT_API QuadraticTaylor
  : public PersistentObject
{
  CLASSNAME;
public:


  /** Default constructor */
  QuadraticTaylor();

  /** Constructor with parameters */
  QuadraticTaylor(const NumericalPoint & center,
                  const NumericalMathFunction & inputFunction);


  /** Virtual constructor */
  virtual QuadraticTaylor * clone() const;

  /** String converter */
  String __repr__() const;

  /** Response surface computation */
  void run();

  /** Center accessor */
  NumericalPoint getCenter() const;

  /** Constant accessor */
  NumericalPoint getConstant() const;

  /** Linear accessor */
  Matrix getLinear() const;

  /** Quadratic accessor */
  SymmetricTensor getQuadratic() const;

  /** Input function accessor */
  NumericalMathFunction getInputFunction() const;

  /** Response surface accessor */
  NumericalMathFunction getResponseSurface() const;

protected:

private:

  NumericalPoint center_;
  NumericalMathFunction inputFunction_;
  NumericalMathFunction responseSurface_;
  NumericalPoint constant_;
  Matrix linear_;
  SymmetricTensor quadratic_;

}; /* class QuadraticTaylor */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICTAYLOR_HXX */
