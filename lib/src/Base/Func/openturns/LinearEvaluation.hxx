//                                               -*- C++ -*-
/**
 * @brief Class for a linear numerical math function implementation
 *        of the form y = constant + <linear, x - c> where c is a
 *        dim(x) numerical point, linear a dim(x) by dim(y) matrix
 *        and  <linear, x - c> means Transpose(linear).(x - c)
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_LINEARNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_LINEARNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearEvaluation
 */
class OT_API LinearEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  LinearEvaluation();

  /** Parameter constructor */
  LinearEvaluation(const Point & center,
                   const Point & constant,
                   const Matrix & linear);

  /** Virtual constructor */
  LinearEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const LinearEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator() (const Point & inP) const override;

  /** Operator () */
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

  /** Linearity accessors */
  Bool isLinear() const override;
  Bool isLinearlyDependent(const UnsignedInteger index) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  Point center_;
  Point constant_;
  Matrix linear_;
}; /* class LinearEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
