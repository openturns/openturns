//                                               -*- C++ -*-
/**
 *  @brief Class for a quadratic numerical math function implementation
 *        of the form y = constant + <linear, x-c> + <<quadratic, x-c>, x-c>
 *        where constant is a dim(y) numerical point, c a dim(x) numerical
 *        point, linear is a dim(x) by dim(y) matrix, quadratic is a
 *        dim(x) by dim(x) by dim(y) symmetric tensor and <linear, x>
 *        means Transpose(linear).x, <quadratic, x> means
 *        Transpose_kj(quadratic).x
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

#ifndef OPENTURNS_QUADRATICEVALUATION_HXX
#define OPENTURNS_QUADRATICEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class QuadraticEvaluation
 *
 * This class offers an interface for an real numerical mathematical evaluation
 * implementations into the platform. It realizes the computation of the image vector
 * of a vector (aka Point) through the evaluation. The
 * evaluation may be loaded in an external way through a wrapper.
 */
class OT_API QuadraticEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  QuadraticEvaluation();

  /** Parameter constructor */
  QuadraticEvaluation(const Point & center,
                      const Point & constant,
                      const Matrix & linear,
                      const SymmetricTensor & quadratic);


  /** Virtual constructor */
  QuadraticEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const QuadraticEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Point & inP) const override;
  Sample operator() (const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for the center */
  Point getCenter() const;

  /** Accessor for the constant term */
  Point getConstant() const;

  /** Accessor for the linear term */
  Matrix getLinear() const;

  /** Accessor for the quadratic term */
  SymmetricTensor getQuadratic() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  Point center_;
  Point constant_;
  Matrix linear_;
  SymmetricTensor quadratic_;
}; /* class QuadraticEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUADRATICEVALUATION_HXX */
