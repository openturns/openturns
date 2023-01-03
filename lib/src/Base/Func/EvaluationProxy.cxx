//                                               -*- C++ -*-
/**
 * @brief Proxy class for EvaluationImplementation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/EvaluationProxy.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/NoEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EvaluationProxy)

static const Factory<EvaluationProxy> Factory_EvaluationProxy;


/* Default constructor */
EvaluationProxy::EvaluationProxy()
  : EvaluationImplementation()
  , evaluation_(new NoEvaluation())
{
  // Nothing to do
}

/* Parameter constructor */
EvaluationProxy::EvaluationProxy(const Evaluation & evaluation)
  : EvaluationImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
EvaluationProxy * EvaluationProxy::clone() const
{
  return new EvaluationProxy(*this);
}


/* Comparison operator */
Bool EvaluationProxy::operator ==(const EvaluationProxy & other) const
{
  return evaluation_ == other.evaluation_;
}

/* String converter */
String EvaluationProxy::__repr__() const
{
  return evaluation_.__repr__();
}

/* String converter */
String EvaluationProxy::__str__(const String & offset) const
{
  return evaluation_.__str__(offset);
}

/* Description Accessor */
void EvaluationProxy::setDescription(const Description & description)
{
  evaluation_.setDescription(description);
}


/* Description Accessor */
Description EvaluationProxy::getDescription() const
{
  return evaluation_.getDescription();
}

/* Input description Accessor */
void EvaluationProxy::setInputDescription(const Description & inputDescription)
{
  evaluation_.setInputDescription(inputDescription);
}

Description EvaluationProxy::getInputDescription() const
{
  return evaluation_.getInputDescription();
}

/* Output description Accessor */
void EvaluationProxy::setOutputDescription(const Description & outputDescription)
{
  evaluation_.setOutputDescription(outputDescription);
}

Description EvaluationProxy::getOutputDescription() const
{
  return evaluation_.getOutputDescription();
}

/* Test for actual implementation */
Bool EvaluationProxy::isActualImplementation() const
{
  return evaluation_.getImplementation()->isActualImplementation();
}

/* Operator () */
Sample EvaluationProxy::operator() (const Sample & inSample) const
{
  return evaluation_.operator()(inSample);
}


/* Operator () */
Field EvaluationProxy::operator() (const Field & inField) const
{
  return evaluation_.operator()(inField);
}


/* Gradient according to the marginal parameters */
Matrix EvaluationProxy::parameterGradient(const Point & inP) const
{
  return evaluation_.parameterGradient(inP);
}

/* Parameters value accessor */
Point EvaluationProxy::getParameter() const
{
  return evaluation_.getParameter();
}

void EvaluationProxy::setParameter(const Point & parameter)
{
  evaluation_.setParameter(parameter);
}

void EvaluationProxy::setParameterDescription(const Description & description)
{
  evaluation_.setParameterDescription(description);
}

/* Parameters description accessor */
Description EvaluationProxy::getParameterDescription() const
{
  return evaluation_.getParameterDescription();
}


/* Operator () */
Point EvaluationProxy::operator() (const Point & inP) const
{
  return evaluation_.operator()(inP);
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationProxy::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger EvaluationProxy::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationProxy::getParameterDimension() const
{
  return evaluation_.getParameterDimension();
}

/* Get the i-th marginal function */
Evaluation EvaluationProxy::getMarginal(const UnsignedInteger i) const
{
  return evaluation_.getMarginal(i);
}

/* Get the function corresponding to indices components */
Evaluation EvaluationProxy::getMarginal(const Indices & indices) const
{
  return evaluation_.getMarginal(indices);
}

/* Get the number of calls to operator() */
UnsignedInteger EvaluationProxy::getCallsNumber() const
{
  return evaluation_.getCallsNumber();
}

/* Linearity accessors */
Bool EvaluationProxy::isLinear() const
{
  return evaluation_.isLinear();
}

Bool EvaluationProxy::isLinearlyDependent(const UnsignedInteger index) const
{
  return evaluation_.isLinearlyDependent(index);
}

/* Is it safe to call in parallel? */
Bool EvaluationProxy::isParallel() const
{
  return evaluation_.getImplementation()->isParallel();
}

/* Invalid values check accessor */
void EvaluationProxy::setCheckOutput(const Bool checkOutput)
{
  evaluation_.setCheckOutput(checkOutput);
}

Bool EvaluationProxy::getCheckOutput() const
{
  return evaluation_.getCheckOutput();
}


/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph EvaluationProxy::draw(const UnsignedInteger inputMarginal,
                            const UnsignedInteger outputMarginal,
                            const Point & centralPoint,
                            const Scalar xMin,
                            const Scalar xMax,
                            const UnsignedInteger pointNumber,
                            const GraphImplementation::LogScale scale) const
{
  return evaluation_.draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
Graph EvaluationProxy::draw(const UnsignedInteger firstInputMarginal,
                            const UnsignedInteger secondInputMarginal,
                            const UnsignedInteger outputMarginal,
                            const Point & centralPoint,
                            const Point & xMin,
                            const Point & xMax,
                            const Indices & pointNumber,
                            const GraphImplementation::LogScale scale) const
{
  return evaluation_.draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph EvaluationProxy::draw(const Scalar xMin,
                            const Scalar xMax,
                            const UnsignedInteger pointNumber,
                            const GraphImplementation::LogScale scale) const
{
  return evaluation_.draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph EvaluationProxy::draw(const Point & xMin,
                            const Point & xMax,
                            const Indices & pointNumber,
                            const GraphImplementation::LogScale scale) const
{
  return evaluation_.draw(xMin, xMax, pointNumber, scale);
}


/* Method save() stores the object through the StorageManager */
void EvaluationProxy::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void EvaluationProxy::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
