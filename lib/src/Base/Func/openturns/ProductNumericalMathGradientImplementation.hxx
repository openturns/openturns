//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math functions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PRODUCTNUMERICALMATHGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_PRODUCTNUMERICALMATHGRADIENTIMPLEMENTATION_HXX

#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/NumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ProductNumericalMathGradientImplementation
 *
 * The class that implement the composition of two numerical math functions,
 * its gradient and its hessian. This class has no implementation class
 */
class OT_API ProductNumericalMathGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<NumericalMathEvaluationImplementation>         EvaluationImplementation;
  typedef Pointer<NumericalMathGradientImplementation>           GradientImplementation;

  /** Default constructor */
  ProductNumericalMathGradientImplementation(const EvaluationImplementation & p_leftEvaluation,
      const GradientImplementation & p_leftGradient,
      const EvaluationImplementation & p_rightEvaluation,
      const GradientImplementation & p_rightGradient);


  /** Virtual constructor */
  virtual ProductNumericalMathGradientImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ProductNumericalMathGradientImplementation & other) const;

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

  ProductNumericalMathGradientImplementation() {};
  friend class Factory<ProductNumericalMathGradientImplementation>;

private:

  /** The evaluation of f in h = f . g */
  EvaluationImplementation p_leftEvaluation_;

  /** The gradient of f in h = f . g */
  GradientImplementation p_leftGradient_;

  /** The evaluation of g in h = f . g */
  EvaluationImplementation p_rightEvaluation_;

  /** The gradient of g in h = f . g */
  GradientImplementation p_rightGradient_;

}; /* class ProductNumericalMathGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTNUMERICALMATHGRADIENTIMPLEMENTATION_HXX */
