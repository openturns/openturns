//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math hessians
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
#ifndef OPENTURNS_COMPOSEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX
#define OPENTURNS_COMPOSEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX

#include "openturns/NumericalMathHessianImplementation.hxx"
#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/NumericalMathEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ComposedNumericalMathHessianImplementation
 *
 * The class that implement the composition of two numerical math hessians
 */
class OT_API ComposedNumericalMathHessianImplementation
  : public NumericalMathHessianImplementation
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<NumericalMathHessianImplementation>          HessianImplementation;
  typedef Pointer<NumericalMathGradientImplementation>         GradientImplementation;
  typedef Pointer<NumericalMathEvaluationImplementation>       EvaluationImplementation;

  /** Default constructor */
  ComposedNumericalMathHessianImplementation(const GradientImplementation & p_leftGradient,
      const HessianImplementation & p_leftHessian,
      const EvaluationImplementation & p_rightFunction,
      const GradientImplementation & p_rightGradient,
      const HessianImplementation & p_rightHessian);


  /** Virtual constructor */
  virtual ComposedNumericalMathHessianImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const ComposedNumericalMathHessianImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method hessian() returns the symmetric tensor of the function at point */
  SymmetricTensor hessian(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  ComposedNumericalMathHessianImplementation() {};
  friend class Factory<ComposedNumericalMathHessianImplementation>;
private:

  /** The gradient of f in h = f o g */
  GradientImplementation p_leftGradient_;

  /** The hessian of f in h = f o g */
  HessianImplementation p_leftHessian_;

  /** The function g in h = f o g */
  EvaluationImplementation p_rightFunction_;

  /** The gradient of g in h = f o g */
  GradientImplementation p_rightGradient_;

  /** The hessian of g in h = f o g */
  HessianImplementation p_rightHessian_;

}; /* class ComposedNumericalMathHessianImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDNUMERICALMATHHESSIANIMPLEMENTATION_HXX */
