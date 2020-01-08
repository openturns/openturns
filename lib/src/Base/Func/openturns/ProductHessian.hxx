//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math hessians
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
#ifndef OPENTURNS_PRODUCTHESSIAN_HXX
#define OPENTURNS_PRODUCTHESSIAN_HXX

#include "openturns/Hessian.hxx"
#include "openturns/Gradient.hxx"
#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ProductHessian
 *
 * The class that implement the composition of two numerical math hessians
 */
class OT_API ProductHessian
  : public HessianImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ProductHessian(const Evaluation & leftEvaluation,
                 const Gradient & leftGradient,
                 const Hessian & leftHessian,
                 const Evaluation & rightEvaluation,
                 const Gradient & rightGradient,
                 const Hessian & rightHessian);

  /** Virtual constructor */
  virtual ProductHessian * clone() const;

  /** Comparison operator */
  Bool operator ==(const ProductHessian & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method hessian() returns the symmetric tensor of the function at point */
  SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  ProductHessian() {};
  friend class Factory<ProductHessian>;
private:

  /** The function of f in h = f . g */
  Evaluation leftEvaluation_;

  /** The gradient of f in h = f . g */
  Gradient leftGradient_;

  /** The hessian of f in h = f . g */
  Hessian leftHessian_;

  /** The function g in h = f . g */
  Evaluation rightEvaluation_;

  /** The gradient of g in h = f . g */
  Gradient rightGradient_;

  /** The hessian of g in h = f . g */
  Hessian rightHessian_;

}; /* class ProductHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTHESSIAN_HXX */
