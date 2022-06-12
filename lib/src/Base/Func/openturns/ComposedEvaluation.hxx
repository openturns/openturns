//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical
 *        math functions implementations
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/Evaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComposedEvaluation
 *
 * The class that implement the composition of two numerical math functions implementations.
 */
class OT_API ComposedEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ComposedEvaluation(const Evaluation & leftFunction,
                     const Evaluation & rightFunction);


  /** Virtual constructor */
  ComposedEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const ComposedEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  using EvaluationImplementation::operator();
  Point operator() (const Point & inP) const override;

  Sample operator() (const Sample & inS) const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameter) override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;
  void setParameterDescription(const Description & description) override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Get the i-th marginal function */
  Evaluation getMarginal(const UnsignedInteger i) const override;

  /** Get the function corresponding to indices components */
  Evaluation getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Composed implementation accessor */
  Evaluation getLeftEvaluation() const;
  Evaluation getRightEvaluation() const;

  /** Linearity accessors */
  Bool isLinear() const override;
  Bool isLinearlyDependent(const UnsignedInteger index) const override;

  /** Is it safe to call in parallel? */
  Bool isParallel() const override;

protected:

  ComposedEvaluation() {};
  friend class Factory<ComposedEvaluation>;

private:

  /** The function f in h = f o g */
  Evaluation leftFunction_;

  /** The function g in h = f o g */
  Evaluation rightFunction_;

}; /* class ComposedEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDNUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
