//                                               -*- C++ -*-
/**
 *  @brief ParametricGradient
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PARAMETRICGRADIENT_HXX
#define OPENTURNS_PARAMETRICGRADIENT_HXX

#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/ParametricEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametricGradient
 *
 * ParametricGradient implements the concept of parametric function for its gradient part
 */
class OT_API ParametricGradient
  : public NumericalMathGradientImplementation
{
  CLASSNAME;

public:
  typedef Pointer<ParametricEvaluation>                EvaluationPointer;

  /** Default constructor */
  ParametricGradient();

  /** Parameters constructor */
  ParametricGradient(const EvaluationPointer & p_evaluation);

  /** Virtual constructor method */
  ParametricGradient * clone() const;

  /** Gradient operator */
  using NumericalMathGradientImplementation::gradient;
  Matrix gradient(const NumericalPoint & point) const;
  Matrix gradient(const NumericalPoint & point,
                  const NumericalPoint & parameters) const;

  /** Evaluation accessor */
  ParametricEvaluation getEvaluation() const;

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
  EvaluationPointer p_evaluation_;

}; /* class ParametricGradient */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRICGRADIENT_HXX */
