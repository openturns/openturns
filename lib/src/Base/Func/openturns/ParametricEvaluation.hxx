//                                               -*- C++ -*-
/**
 *  @brief ParametricEvaluation
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
#ifndef OPENTURNS_PARAMETRICEVALUATION_HXX
#define OPENTURNS_PARAMETRICEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametricEvaluation
 *
 * ParametricEvaluation implements the concept of parametric function for its evaluation part
 */
class OT_API ParametricEvaluation
  : public EvaluationImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  ParametricEvaluation();

  /** Parameters constructor */
  ParametricEvaluation(const Function & function,
                       const Indices & set,
                       const Point & referencePoint,
                       const Bool parametersSet = true);

  /** Virtual constructor method */
  ParametricEvaluation * clone() const;

  /** Evaluation operator */
  using EvaluationImplementation::operator();
  Point operator() (const Point & point) const;

  Sample operator() (const Sample & inS) const;

  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const Point & inP) const;

  /** Parameters positions accessor */
  Indices getParametersPositions() const;

  /** Input positions accessor */
  Indices getInputPositions() const;

  /** Function accessor */
  Function getFunction() const;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const;
  UnsignedInteger getParameterDimension() const;
  UnsignedInteger getOutputDimension() const;

  /** Parameters accessor */
  void setParameter(const Point & parameters);

  /** Linearity accessors */
  Bool isLinear() const;
  Bool isLinearlyDependent(const UnsignedInteger index) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  friend class ParametricGradient;
  friend class ParametricHessian;

  /** The underlying function */
  Function function_;

  /** The position of the parameters */
  Indices parametersPositions_;

  /** The position of the input */
  Indices inputPositions_;

}; /* class ParametricEvaluation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRICEVALUATION_HXX */
