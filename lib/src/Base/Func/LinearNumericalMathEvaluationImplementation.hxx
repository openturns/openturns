//                                               -*- C++ -*-
/**
 * @brief Class for a linear numerical math function implementation
 *        of the form y = constant + <linear, x - c> where c is a
 *        dim(x) numerical point, linear a dim(x) by dim(y) matrix
 *        and  <linear, x - c> means Transpose(linear).(x - c)
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

#ifndef OPENTURNS_LINEARNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_LINEARNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "NumericalPoint.hxx"
#include "NumericalSample.hxx"
#include "Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearNumericalMathEvaluationImplementation
 */
class OT_API LinearNumericalMathEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  LinearNumericalMathEvaluationImplementation();

  /** Parameter constructor */
  LinearNumericalMathEvaluationImplementation(const NumericalPoint & center,
      const NumericalPoint & constant,
      const Matrix & linear);

  /** Virtual constructor */
  virtual LinearNumericalMathEvaluationImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const LinearNumericalMathEvaluationImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalPoint & inP) const;

  /** Operator () */
  virtual NumericalSample operator() (const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Accessor for the center */
  NumericalPoint getCenter() const;

  /** Accessor for the constant term */
  NumericalPoint getConstant() const;

  /** Accessor for the linear term */
  Matrix getLinear() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  NumericalPoint center_;
  NumericalPoint constant_;
  Matrix linear_;
}; /* class LinearNumericalMathEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
