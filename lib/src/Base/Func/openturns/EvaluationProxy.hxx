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
  virtual EvaluationProxy * clone() const;

  /** Comparison operator */
  Bool operator ==(const EvaluationProxy & other) const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;


  /** Description Accessor, i.e. the names of the input and output parameters */
  virtual void setDescription(const Description & description);
  Description getDescription() const;

  /** Input description Accessor, i.e. the names of the input parameters */
  Description getInputDescription() const;
  void setInputDescription(const Description & inputDescription);

  /** Output description Accessor, i.e. the names of the Output parameters */
  Description getOutputDescription() const;
  void setOutputDescription(const Description & outputDescription);

  /* Here is the interface that all derived class must implement */

  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;

  /** Operator () */
  virtual Point operator() (const Point & inP) const;

  /** Operator () on a sample, not pure virtual because a generic implementation is given */
  virtual Sample operator() (const Sample & inSample) const;

  /** Operator () on a time series, not pure virtual because a generic implementation is given */
  virtual Field operator() (const Field & inField) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Accessor for parameters dimension */
  virtual UnsignedInteger getParameterDimension() const;

  /** Get the i-th marginal evaluation */
  virtual Evaluation getMarginal(const UnsignedInteger i) const;

  /** Get the evaluation corresponding to indices components */
  virtual Evaluation getMarginal(const Indices & indices) const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

  /** Get the number of calls to operator() */
  UnsignedInteger getCallsNumber() const;

  /** Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
  virtual Graph draw(const UnsignedInteger inputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
  virtual Graph draw(const UnsignedInteger firstInputMarginal,
                     const UnsignedInteger secondInputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input and output dimensions are 1 */
  virtual Graph draw(const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
  virtual Graph draw(const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

  /** The proxied instance */
  Evaluation evaluation_;

}; /* class EvaluationProxy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EVALUATIONPROXY_HXX */
