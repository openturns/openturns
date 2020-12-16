//                                               -*- C++ -*-
/**
 * @brief Proxy class for EvaluationImplementation
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

#ifndef OPENTURNS_EVALUATIONPROXY_HXX
#define OPENTURNS_EVALUATIONPROXY_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EvaluationProxy
 *
 * This class offers an abstract interface for the implementation
 * of an real numerical mathematical function into the platform.
 */
class OT_API EvaluationProxy
  : public EvaluationImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  EvaluationProxy();

  /** Parameter constructor */
  explicit EvaluationProxy(const Evaluation & evaluation);

  /** Virtual constructor */
  EvaluationProxy * clone() const override;

  /** Comparison operator */
  Bool operator ==(const EvaluationProxy & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /** Description Accessor, i.e. the names of the input and output parameters */
  void setDescription(const Description & description) override;
  Description getDescription() const override;

  /** Input description Accessor, i.e. the names of the input parameters */
  Description getInputDescription() const override;
  void setInputDescription(const Description & inputDescription) override;

  /** Output description Accessor, i.e. the names of the Output parameters */
  Description getOutputDescription() const override;
  void setOutputDescription(const Description & outputDescription) override;

  /* Here is the interface that all derived class must implement */

  /** Test for actual implementation */
  Bool isActualImplementation() const override;

  /** Operator () */
  Point operator() (const Point & inP) const override;

  /** Operator () on a sample, not pure virtual because a generic implementation is given */
  Sample operator() (const Sample & inSample) const override;

  /** Operator () on a time series, not pure virtual because a generic implementation is given */
  Field operator() (const Field & inField) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for parameters dimension */
  UnsignedInteger getParameterDimension() const override;

  /** Get the i-th marginal evaluation */
  Evaluation getMarginal(const UnsignedInteger i) const override;

  /** Get the evaluation corresponding to indices components */
  Evaluation getMarginal(const Indices & indices) const override;

  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const Point & inP) const override;

  /** Parameters value accessor */
  Point getParameter() const override;
  void setParameter(const Point & parameters) override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;
  void setParameterDescription(const Description & description) override;

  /** Get the number of calls to operator() */
  UnsignedInteger getCallsNumber() const override;

  /** Linearity accessors */
  Bool isLinear() const override;
  Bool isLinearlyDependent(const UnsignedInteger index) const override;

  /** Is it safe to call in parallel? */
  Bool isParallel() const override;

  /** Invalid values check accessor */
  void setCheckOutput(const Bool checkOutput) override;
  Bool getCheckOutput() const override;

  /** Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
  Graph draw(const UnsignedInteger inputMarginal,
             const UnsignedInteger outputMarginal,
             const Point & centralPoint,
             const Scalar xMin,
             const Scalar xMax,
             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE) const override;

  /** Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
  Graph draw(const UnsignedInteger firstInputMarginal,
             const UnsignedInteger secondInputMarginal,
             const UnsignedInteger outputMarginal,
             const Point & centralPoint,
             const Point & xMin,
             const Point & xMax,
             const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE) const override;

  /** Draw the output of the function with respect to its input when the input and output dimensions are 1 */
  Graph draw(const Scalar xMin,
             const Scalar xMax,
             const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE) const override;

  /** Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
  Graph draw(const Point & xMin,
             const Point & xMax,
             const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
             const GraphImplementation::LogScale scale = GraphImplementation::NONE) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

  /** The proxied instance */
  Evaluation evaluation_;

}; /* class EvaluationProxy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EVALUATIONPROXY_HXX */
