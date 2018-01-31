//                                               -*- C++ -*-
/**
 *  @brief ParametricGradient
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PARAMETRICGRADIENT_HXX
#define OPENTURNS_PARAMETRICGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/ParametricEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametricGradient
 *
 * ParametricGradient implements the concept of parametric function for its gradient part
 */
class OT_API ParametricGradient
  : public GradientImplementation
{
  CLASSNAME

public:
  typedef Pointer<ParametricEvaluation>                EvaluationPointer;

  /** Default constructor */
  ParametricGradient();

  /** Parameters constructor */
  ParametricGradient(const EvaluationPointer & p_evaluation);

  /** Virtual constructor method */
  ParametricGradient * clone() const;

  /** Gradient operator */
  using GradientImplementation::gradient;
  Matrix gradient(const Point & point) const;

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
