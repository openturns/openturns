//                                               -*- C++ -*-
/**
 *  @brief ParametricEvaluationImplementation
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
#ifndef OPENTURNS_PARAMETRICEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_PARAMETRICEVALUATIONIMPLEMENTATION_HXX

#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/Indices.hxx"
#include "openturns/NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametricEvaluationImplementation
 *
 * ParametricEvaluationImplementation implements the concept of parametric function for its evaluation part
 */
class OT_API ParametricEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;

public:
  /** Default constructor */
  ParametricEvaluationImplementation();

  /** Parameters constructor */
  ParametricEvaluationImplementation(const NumericalMathFunction & function,
                                     const Indices & set,
                                     const NumericalPoint & referencePoint,
                                     const Bool parametersSet = true);

  /** Virtual constructor method */
  ParametricEvaluationImplementation * clone() const;

  /** Evaluation operator */
  using NumericalMathEvaluationImplementation::operator();
  NumericalPoint operator() (const NumericalPoint & point) const;
  NumericalSample operator() (const NumericalSample & inS) const;

  /** Parameters positions accessor */
  Indices getParametersPositions() const;

  /** Input positions accessor */
  Indices getInputPositions() const;

  /** Function accessor */
  NumericalMathFunction getFunction() const;

  /** Dimension accessor */
  UnsignedInteger getInputDimension() const;
  UnsignedInteger getParameterDimension() const;
  UnsignedInteger getOutputDimension() const;

  /** Parameters accessor */
  using NumericalMathEvaluationImplementation::setParameter;
  void setParameter(const NumericalPointWithDescription & parameters);
  void setParameter(const NumericalPoint & parameters);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  ParametricEvaluationImplementation(const ParametricEvaluationImplementation & evaluation,
                                     const Indices & set,
                                     const NumericalPoint & referencePoint);

private:

  friend class ParametricGradientImplementation;
  friend class ParametricHessianImplementation;

  /** The underlying function */
  NumericalMathFunction function_;

  /** The position of the parameters */
  Indices parametersPositions_;

  /** The position of the input */
  Indices inputPositions_;

}; /* class ParametricEvaluationImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRICEVALUATIONIMPLEMENTATION_HXX */
