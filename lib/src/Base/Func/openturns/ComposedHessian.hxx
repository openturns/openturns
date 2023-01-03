//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math hessians
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_COMPOSEDHESSIAN_HXX
#define OPENTURNS_COMPOSEDHESSIAN_HXX

#include "openturns/Hessian.hxx"
#include "openturns/Gradient.hxx"
#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ComposedHessian
 *
 * The class that implement the composition of two numerical math hessians
 */
class OT_API ComposedHessian
  : public HessianImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ComposedHessian(const Gradient & leftGradient,
                  const Hessian & leftHessian,
                  const Evaluation & rightFunction,
                  const Gradient & rightGradient,
                  const Hessian & rightHessian);


  /** Virtual constructor */
  ComposedHessian * clone() const override;

  /** Comparison operator */
  Bool operator ==(const ComposedHessian & other) const;

  /** String converter */
  String __repr__() const override;

  /** Method hessian() returns the symmetric tensor of the function at point */
  SymmetricTensor hessian(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  ComposedHessian() {};
  friend class Factory<ComposedHessian>;
private:

  /** The gradient of f in h = f o g */
  Gradient leftGradient_;

  /** The hessian of f in h = f o g */
  Hessian leftHessian_;

  /** The function g in h = f o g */
  Evaluation rightFunction_;

  /** The gradient of g in h = f o g */
  Gradient rightGradient_;

  /** The hessian of g in h = f o g */
  Hessian rightHessian_;

}; /* class ComposedHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDHESSIAN_HXX */
