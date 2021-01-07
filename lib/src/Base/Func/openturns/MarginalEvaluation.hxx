//                                               -*- C++ -*-
/**
 * @brief Marginal evaluation
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_MARGINALEVALUATION_HXX
#define OPENTURNS_MARGINALEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class MarginalEvaluation
 *
 * Marginal evaluation
 */
class OT_API MarginalEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MarginalEvaluation();

  MarginalEvaluation(const EvaluationImplementation & evaluation,
                     const Indices & indices);

#ifndef SWIG
  /** Parameters constructor */
  MarginalEvaluation(const Pointer<EvaluationImplementation> & p_evaluation,
                     const Indices & indices);
#endif

  /** Virtual constructor */
  MarginalEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const MarginalEvaluation & other) const;

  /** String converter */
  String __repr__() const override;

  /** Indices accessor */
  Indices getIndices() const;

  /** Evaluation accessor */
  Pointer<EvaluationImplementation> getEvaluation() const;

  /* Here is the interface that all derived class must implement */

  /** Evaluation operator */
  using EvaluationImplementation::operator();
  Point operator() (const Point & point) const override;

  Sample operator() (const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const Point & inP) const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameters) override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;
  void setParameterDescription(const Description & description) override;

  /** Invalid values check accessor */
  void setCheckOutput(const Bool checkOutput) override;
  Bool getCheckOutput() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** The underlying function */
  Pointer<EvaluationImplementation> p_evaluation_;

  /** The indices of the marginals */
  Indices indices_;

}; /* class MarginalEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NONOEVALUATION_HXX */
