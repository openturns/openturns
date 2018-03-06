//                                               -*- C++ -*-
/**
 * @brief Proxy class for EvaluationImplementation
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
#include "openturns/EvaluationProxy.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EvaluationProxy)

static const Factory<EvaluationProxy> Factory_EvaluationProxy;


/* Default constructor */
EvaluationProxy::EvaluationProxy()
  : EvaluationImplementation()
  , p_evaluationImplementation_(0)
{
  // Nothing to do
}

/* Parameter constructor */
EvaluationProxy::EvaluationProxy(const Implementation & p_evaluation)
  : EvaluationImplementation()
  , p_evaluationImplementation_(p_evaluation)
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
  return p_evaluationImplementation_ == other.p_evaluationImplementation_;
}

/* String converter */
String EvaluationProxy::__repr__() const
{
  return p_evaluationImplementation_->__repr__();
}

/* String converter */
String EvaluationProxy::__str__(const String & offset) const
{
  return p_evaluationImplementation_->__str__(offset);
}

/* Description Accessor */
void EvaluationProxy::setDescription(const Description & description)
{
  p_evaluationImplementation_->setDescription(description);
}


/* Description Accessor */
Description EvaluationProxy::getDescription() const
{
  return p_evaluationImplementation_->getDescription();
}

/* Input description Accessor */
void EvaluationProxy::setInputDescription(const Description & inputDescription)
{
  p_evaluationImplementation_->setInputDescription(inputDescription);
}

Description EvaluationProxy::getInputDescription() const
{
  return p_evaluationImplementation_->getInputDescription();
}

/* Output description Accessor */
void EvaluationProxy::setOutputDescription(const Description & outputDescription)
{
  p_evaluationImplementation_->setOutputDescription(outputDescription);
}

Description EvaluationProxy::getOutputDescription() const
{
  return p_evaluationImplementation_->getOutputDescription();
}

/* Test for actual implementation */
Bool EvaluationProxy::isActualImplementation() const
{
  return p_evaluationImplementation_->isActualImplementation();
}

/* Operator () */
Sample EvaluationProxy::operator() (const Sample & inSample) const
{
  return p_evaluationImplementation_->operator()(inSample);
}


/* Operator () */
Field EvaluationProxy::operator() (const Field & inField) const
{
  return p_evaluationImplementation_->operator()(inField);
}


/* Enable or disable the internal cache */
void EvaluationProxy::enableCache() const
{
  p_evaluationImplementation_->enableCache();
}

void EvaluationProxy::disableCache() const
{
  p_evaluationImplementation_->disableCache();
}

Bool EvaluationProxy::isCacheEnabled() const
{
  return p_evaluationImplementation_->isCacheEnabled();
}

UnsignedInteger EvaluationProxy::getCacheHits() const
{
  return p_evaluationImplementation_->getCacheHits();
}

void EvaluationProxy::addCacheContent(const Sample& inSample, const Sample& outSample)
{
  p_evaluationImplementation_->addCacheContent(inSample, outSample);
}

Sample EvaluationProxy::getCacheInput() const
{
  return p_evaluationImplementation_->getCacheInput();
}

Sample EvaluationProxy::getCacheOutput() const
{
  return p_evaluationImplementation_->getCacheOutput();
}

void EvaluationProxy::clearCache() const
{
  p_evaluationImplementation_->clearCache();
}

/* Gradient according to the marginal parameters */
Matrix EvaluationProxy::parameterGradient(const Point & inP) const
{
  return p_evaluationImplementation_->parameterGradient(inP);
}

/* Parameters value accessor */
Point EvaluationProxy::getParameter() const
{
  return p_evaluationImplementation_->getParameter();
}

void EvaluationProxy::setParameter(const Point & parameter)
{
  p_evaluationImplementation_->setParameter(parameter);
}

void EvaluationProxy::setParameterDescription(const Description & description)
{
  p_evaluationImplementation_->setParameterDescription(description);
}

/* Parameters description accessor */
Description EvaluationProxy::getParameterDescription() const
{
  return p_evaluationImplementation_->getParameterDescription();
}


/* Operator () */
Point EvaluationProxy::operator() (const Point & inP) const
{
  return p_evaluationImplementation_->operator()(inP);
}

Point EvaluationProxy::operator() (const Point & inP,
    const Point & parameter)
{
  return p_evaluationImplementation_->operator()(inP, parameter);
}

Sample EvaluationProxy::operator() (const Point & inP,
    const Sample & parameters)
{
  return p_evaluationImplementation_->operator()(inP, parameters);
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationProxy::getInputDimension() const
{
  return p_evaluationImplementation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger EvaluationProxy::getOutputDimension() const
{
  return p_evaluationImplementation_->getOutputDimension();
}

/* Accessor for input point dimension */
UnsignedInteger EvaluationProxy::getParameterDimension() const
{
  return p_evaluationImplementation_->getParameterDimension();
}

/* Get the i-th marginal function */
EvaluationProxy::Implementation EvaluationProxy::getMarginal(const UnsignedInteger i) const
{
  return p_evaluationImplementation_->getMarginal(i);
}

/* Get the function corresponding to indices components */
EvaluationProxy::Implementation EvaluationProxy::getMarginal(const Indices & indices) const
{
  return p_evaluationImplementation_->getMarginal(indices);
}

/* Get the number of calls to operator() */
UnsignedInteger EvaluationProxy::getCallsNumber() const
{
  return p_evaluationImplementation_->getCallsNumber();
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
  return p_evaluationImplementation_->draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
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
  return p_evaluationImplementation_->draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph EvaluationProxy::draw(const Scalar xMin,
                            const Scalar xMax,
                            const UnsignedInteger pointNumber,
                            const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph EvaluationProxy::draw(const Point & xMin,
                            const Point & xMax,
                            const Indices & pointNumber,
                            const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(xMin, xMax, pointNumber, scale);
}


/* Method save() stores the object through the StorageManager */
void EvaluationProxy::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "evaluationImplementation_", *p_evaluationImplementation_ );
}

/* Method load() reloads the object from the StorageManager */
void EvaluationProxy::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  TypedInterfaceObject<EvaluationImplementation> evaluationValue;
  adv.loadAttribute( "evaluationImplementation_", evaluationValue );
  p_evaluationImplementation_ = evaluationValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
