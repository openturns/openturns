//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math functions
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PRODUCTGRADIENT_HXX
#define OPENTURNS_PRODUCTGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/Gradient.hxx"
#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ProductGradient
 *
 * The class that implement the composition of two numerical math functions,
 * its gradient and its hessian. This class has no implementation class
 */
class OT_API ProductGradient
  : public GradientImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ProductGradient(const Evaluation & leftEvaluation,
                  const Gradient & leftGradient,
                  const Evaluation & rightEvaluation,
                  const Gradient & rightGradient);


  /** Virtual constructor */
  virtual ProductGradient * clone() const;

  /** Comparison operator */
  Bool operator ==(const ProductGradient & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  Matrix gradient(const Point & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  ProductGradient() {};
  friend class Factory<ProductGradient>;

private:

  /** The evaluation of f in h = f . g */
  Evaluation leftEvaluation_;

  /** The gradient of f in h = f . g */
  Gradient leftGradient_;

  /** The evaluation of g in h = f . g */
  Evaluation rightEvaluation_;

  /** The gradient of g in h = f . g */
  Gradient rightGradient_;

}; /* class ProductGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTGRADIENT_HXX */
