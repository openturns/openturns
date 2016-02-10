//                                               -*- C++ -*-
/**
 *  @brief ParametricGradientImplementation
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
#ifndef OPENTURNS_PARAMETRICGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_PARAMETRICGRADIENTIMPLEMENTATION_HXX

#include "NumericalMathGradientImplementation.hxx"
#include "StorageManager.hxx"
#include "ParametricEvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametricGradientImplementation
 *
 * ParametricGradientImplementation implements the concept of parametric function for its gradient part
 */
class OT_API ParametricGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;

public:
  typedef Pointer<ParametricEvaluationImplementation>                EvaluationImplementation;

  /** Default constructor */
  ParametricGradientImplementation();

  /** Parameters constructor */
  ParametricGradientImplementation(const EvaluationImplementation & p_evaluation);

  /** Virtual constructor method */
  ParametricGradientImplementation * clone() const;

  /** Gradient operator */
  using NumericalMathGradientImplementation::gradient;
  Matrix gradient(const NumericalPoint & point) const;
  Matrix gradient(const NumericalPoint & point,
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

}; /* class ParametricGradientImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRICGRADIENTIMPLEMENTATION_HXX */
