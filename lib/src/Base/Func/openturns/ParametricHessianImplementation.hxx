//                                               -*- C++ -*-
/**
 *  @brief ParametricHessianImplementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#ifndef OPENTURNS_PARAMETRICHESSIANIMPLEMENTATION_HXX
#define OPENTURNS_PARAMETRICHESSIANIMPLEMENTATION_HXX

#include "openturns/NumericalMathHessianImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/ParametricEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametricHessianImplementation
 *
 * ParametricHessianImplementation implements the concept of parametric function for its hessian part
 */
class OT_API ParametricHessianImplementation
  : public NumericalMathHessianImplementation
{
  CLASSNAME;

public:
  typedef Pointer<ParametricEvaluationImplementation>                EvaluationImplementation;

  /** Default constructor */
  ParametricHessianImplementation();

  /** Parameters constructor */
  ParametricHessianImplementation(const EvaluationImplementation & p_evaluation);

  /** Virtual constructor method */
  ParametricHessianImplementation * clone() const;

  /** Hessian operator */
  using NumericalMathHessianImplementation::hessian;
  SymmetricTensor hessian(const NumericalPoint & point) const;
  SymmetricTensor hessian(const NumericalPoint & point,
                          const NumericalPoint & parameters) const;

  /** Evaluation accessor */
  ParametricEvaluationImplementation getEvaluation() const;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const;
  UnsignedInteger getParameterDimension() const;
  UnsignedInteger getOutputDimension() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** The underlying function */
  EvaluationImplementation p_evaluation_;

}; /* class ParametricHessianImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRICHESSIANIMPLEMENTATION_HXX */
