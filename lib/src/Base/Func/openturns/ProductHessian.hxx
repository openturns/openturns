//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math hessians
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PRODUCTHESSIAN_HXX
#define OPENTURNS_PRODUCTHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ProductHessian
 *
 * The class that implement the composition of two numerical math hessians
 */
class OT_API ProductHessian
  : public HessianImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<HessianImplementation>          HessianPointer;
  typedef Pointer<GradientImplementation>         GradientPointer;
  typedef Pointer<EvaluationImplementation>       EvaluationPointer;

  /** Default constructor */
  ProductHessian(const EvaluationPointer & p_leftEvaluation,
      const GradientPointer & p_leftGradient,
      const HessianPointer & p_leftHessian,
      const EvaluationPointer & p_rightEvaluation,
      const GradientPointer & p_rightGradient,
      const HessianPointer & p_rightHessian);

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
  EvaluationPointer p_leftEvaluation_;

  /** The gradient of f in h = f . g */
  GradientPointer p_leftGradient_;

  /** The hessian of f in h = f . g */
  HessianPointer p_leftHessian_;

  /** The function g in h = f . g */
  EvaluationPointer p_rightEvaluation_;

  /** The gradient of g in h = f . g */
  GradientPointer p_rightGradient_;

  /** The hessian of g in h = f . g */
  HessianPointer p_rightHessian_;

}; /* class ProductHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTHESSIAN_HXX */
