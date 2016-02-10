//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math functions
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
#ifndef OPENTURNS_COMPOSEDNUMERICALMATHGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_COMPOSEDNUMERICALMATHGRADIENTIMPLEMENTATION_HXX

#include "NumericalMathGradientImplementation.hxx"
#include "NumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ComposedNumericalMathGradientImplementation
 *
 * The class that implement the composition of two numerical math functions,
 * its gradient and its hessian. This class has no implementation class
 */
class OT_API ComposedNumericalMathGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<NumericalMathEvaluationImplementation>         EvaluationImplementation;
  typedef Pointer<NumericalMathGradientImplementation>           GradientImplementation;

  /** Default constructor */
  ComposedNumericalMathGradientImplementation(const GradientImplementation & p_leftGradient,
      const EvaluationImplementation & p_rightFunction,
      const GradientImplementation & p_rightGradient);


  /** Virtual constructor */
  virtual ComposedNumericalMathGradientImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ComposedNumericalMathGradientImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  Matrix gradient(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  ComposedNumericalMathGradientImplementation() {};
  friend class Factory<ComposedNumericalMathGradientImplementation>;

private:

  /** The gradient of f in h = f o g */
  GradientImplementation p_leftGradient_;

  /** The function g in h = f o g */
  EvaluationImplementation p_rightFunction_;

  /** The gradient of g in h = f o g */
  GradientImplementation p_rightGradient_;

}; /* class ComposedNumericalMathGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDNUMERICALMATHGRADIENTIMPLEMENTATION_HXX */
