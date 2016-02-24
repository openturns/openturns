//                                               -*- C++ -*-
/**
 *  @brief Second order polynomial response surface by least square
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
#ifndef OPENTURNS_QUADRATICLEASTSQUARES_HXX
#define OPENTURNS_QUADRATICLEASTSQUARES_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class QuadraticLeastSquares
 *
 * The class describes the concept of quadratic response surface
 */
class OT_API QuadraticLeastSquares
  : public PersistentObject
{
  CLASSNAME;
public:


  /** Constructor with parameters */
  QuadraticLeastSquares(const NumericalSample & dataIn,
                        const NumericalMathFunction & inputFunction);

  /** Constructor with parameters */
  QuadraticLeastSquares(const NumericalSample & dataIn,
                        const NumericalSample & dataOut);


  /** Virtual constructor */
  virtual QuadraticLeastSquares * clone() const;

  /** String converter */
  String __repr__() const;

  /** Response surface computation */
  void run();

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

  /** DataIn accessor */
  NumericalSample getDataIn() const;

  /** DataOut accessor */
  void setDataOut(const NumericalSample & dataOut);
  NumericalSample getDataOut();

protected:

private:
  NumericalSample dataIn_;
  NumericalSample dataOut_;
  NumericalMathFunction inputFunction_;
  NumericalMathFunction responseSurface_;
  NumericalPoint constant_;
  Matrix linear_;
  SymmetricTensor quadratic_;

}; /* class QuadraticLeastSquares */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICLEASTSQUARES_HXX */
