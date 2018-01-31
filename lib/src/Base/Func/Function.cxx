//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all functions
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
#include "openturns/Function.hxx"
#include "openturns/LinearCombinationEvaluation.hxx"
#include "openturns/LinearCombinationGradient.hxx"
#include "openturns/LinearCombinationHessian.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Function)


/* Default constructor */
Function::Function()
  : TypedInterfaceObject<FunctionImplementation>(new FunctionImplementation())
{
  // Nothing to do
}

/* Constructor from FunctionImplementation */
Function::Function(const FunctionImplementation & implementation)
  : TypedInterfaceObject<FunctionImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
Function::Function(const Implementation & p_implementation)
  : TypedInterfaceObject<FunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
Function::Function(FunctionImplementation * p_implementation)
  : TypedInterfaceObject<FunctionImplementation>(p_implementation)
{
  // Nothing to do
}


/* Constructor from implementation pointer */
Function::Function(const EvaluationImplementation & evaluation)
  : TypedInterfaceObject<FunctionImplementation>(new FunctionImplementation(evaluation.clone()))
{
  // Nothing to do
}


/* Analytical formula constructor */
Function::Function(const Description & inputVariablesNames,
                   const Description & outputVariablesNames,
                   const Description & formulas)
  : TypedInterfaceObject<FunctionImplementation>(new FunctionImplementation(inputVariablesNames, outputVariablesNames, formulas))
{
  LOGWARN(OSS() << "Function(Description, Description, Description) is deprecated in favor of SymbolicFunction.");
}

/* Constructor from evaluation */
Function::Function(const EvaluationPointer & evaluationImplementation)
  : TypedInterfaceObject<FunctionImplementation>(new FunctionImplementation(evaluationImplementation))
{
  // Nothing to do
}


/* Constructor from implementations */
Function::Function(const EvaluationPointer & evaluationImplementation,
                   const GradientPointer & gradientImplementation,
                   const HessianPointer  & hessianImplementation)
  : TypedInterfaceObject<FunctionImplementation>(new FunctionImplementation(evaluationImplementation, gradientImplementation, hessianImplementation))
{
  // Nothing to do
}

/* Constructor from field */
Function::Function(const Field & field)
  : TypedInterfaceObject<FunctionImplementation>(new FunctionImplementation(new P1LagrangeEvaluation( field )))
{
  LOGWARN(OSS() << "Function(Field) is deprecated.");
}

/* Comparison operator */
Bool Function::operator ==(const Function & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String Function::__repr__() const
{
  return OSS(true) << "class=" << Function::GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String Function::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Description Accessor */
void Function::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}

/* Description Accessor */
Description Function::getDescription() const
{
  return getImplementation()->getDescription();
}

/* Input description Accessor */
Description Function::getInputDescription() const
{
  return getImplementation()->getInputDescription();
}

/* Output description Accessor */
Description Function::getOutputDescription() const
{
  return getImplementation()->getOutputDescription();
}

/* Enable or disable the internal cache */
void Function::enableCache() const
{
  getImplementation()->enableCache();
}

void Function::disableCache() const
{
  getImplementation()->disableCache();
}

Bool Function::isCacheEnabled() const
{
  return getImplementation()->isCacheEnabled();
}

UnsignedInteger Function::getCacheHits() const
{
  return getImplementation()->getCacheHits();
}

void Function::addCacheContent(const Sample & inSample, const Sample & outSample)
{
  getImplementation()->addCacheContent(inSample, outSample);
}

Sample Function::getCacheInput() const
{
  return getImplementation()->getCacheInput();
}

Sample Function::getCacheOutput() const
{
  return getImplementation()->getCacheOutput();
}

void Function::clearCache() const
{
  return getImplementation()->clearCache();
}

/* Enable or disable the input/output history */
void Function::enableHistory() const
{
  return getImplementation()->enableHistory();
}

void Function::disableHistory() const
{
  return getImplementation()->disableHistory();
}

Bool Function::isHistoryEnabled() const
{
  return getImplementation()->isHistoryEnabled();
}

void Function::clearHistory() const
{
  return getImplementation()->clearHistory();
}

HistoryStrategy Function::getHistoryInput() const
{
  return getImplementation()->getHistoryInput();
}

HistoryStrategy Function::getHistoryOutput() const
{
  return getImplementation()->getHistoryOutput();
}

Sample Function::getInputPointHistory() const
{
  return getImplementation()->getInputPointHistory();
}

Sample Function::getInputParameterHistory() const
{
  return getImplementation()->getInputParameterHistory();
}

/* Multiplication operator between two functions with the same input dimension and 1D output dimension */
ProductFunction Function::operator * (const Function & right) const
{
  return ProductFunction(getImplementation(), right.getImplementation());
}

/* Addition operator between two functions with the same input dimension and output dimension */
Function Function::operator + (const Function & right) const
{
  const Point coefficients(2, 1.0);
  FunctionCollection collection(2);
  collection[0] = *this;
  collection[1] = right;
  const LinearCombinationEvaluation evaluation(collection, coefficients);
  return Function(evaluation.clone(), LinearCombinationGradient(evaluation).clone(), LinearCombinationHessian(evaluation).clone());
}

/* Soustraction operator between two functions with the same input dimension and output dimension */
Function Function::operator - (const Function & right) const
{
  Point coefficients(2, 1.0);
  coefficients[1] = -1.0;
  FunctionCollection collection(2);
  collection[0] = *this;
  collection[1] = right;
  const LinearCombinationEvaluation evaluation(collection, coefficients);
  return Function(evaluation.clone(), LinearCombinationGradient(evaluation).clone(), LinearCombinationHessian(evaluation).clone());
}

/* Function implementation accessors */
void Function::setEvaluation(const EvaluationPointer & functionImplementation)
{
  copyOnWrite();
  getImplementation()->setEvaluation(functionImplementation);
}

const Function::EvaluationPointer & Function::getEvaluation() const
{
  return getImplementation()->getEvaluation();
}

/* Gradient implementation accessors */
void Function::setGradient(const GradientImplementation & gradientImplementation)
{
  copyOnWrite();
  getImplementation()->setGradient(GradientPointer(gradientImplementation.clone()));
}

void Function::setGradient(const GradientPointer & gradientImplementation)
{
  copyOnWrite();
  getImplementation()->setGradient(gradientImplementation);
}

const Function::GradientPointer & Function::getGradient() const
{
  return getImplementation()->getGradient();
}

/* Hessian implementation accessors */
void Function::setHessian(const HessianImplementation & hessianImplementation)
{
  copyOnWrite();
  getImplementation()->setHessian(HessianPointer(hessianImplementation.clone()));
}

void Function::setHessian(const HessianPointer & hessianImplementation)
{
  copyOnWrite();
  getImplementation()->setHessian(hessianImplementation);
}

const Function::HessianPointer & Function::getHessian() const
{
  return getImplementation()->getHessian();
}

/* Flag for default gradient accessors */
Bool Function::getUseDefaultGradientImplementation() const
{
  return getImplementation()->getUseDefaultGradientImplementation();
}

void Function::setUseDefaultGradientImplementation(const Bool gradientFlag)
{
  copyOnWrite();
  getImplementation()->setUseDefaultGradientImplementation(gradientFlag);
}

/* Flag for default hessian accessors */
Bool Function::getUseDefaultHessianImplementation() const
{
  return getImplementation()->getUseDefaultHessianImplementation();
}

void Function::setUseDefaultHessianImplementation(const Bool hessianFlag)
{
  copyOnWrite();
  getImplementation()->setUseDefaultHessianImplementation(hessianFlag);
}


/* Gradient according to the marginal parameters */
Matrix Function::parameterGradient(const Point & inP) const
{
  return getImplementation()->parameterGradient(inP);
}

/* Parameters value accessor */
Point Function::getParameter() const
{
  return getImplementation()->getParameter();
}

void Function::setParameter(const Point & parameter)
{
  copyOnWrite();
  getImplementation()->setParameter(parameter);
}

/* Parameters description accessor */
Description Function::getParameterDescription() const
{
  return getImplementation()->getParameterDescription();
}

void Function::setParameterDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setParameterDescription(description);
}

/* Operator () */
Point Function::operator() (const Point & inP) const
{
  return getImplementation()->operator()(inP);
}

/* Operator () */
Sample Function::operator() (const Sample & inSample) const
{
  return getImplementation()->operator()(inSample);
}

/* Operator () */
Field Function::operator() (const Field & inField) const
{
  return getImplementation()->operator()(inField);
}

/* Method gradient() returns the Jacobian transposed matrix of the function at point */
Matrix Function::gradient(const Point & inP) const
{
  return getImplementation()->gradient(inP);
}

/* Method hessian() returns the symmetric tensor of the function at point */
SymmetricTensor Function::hessian(const Point & inP) const
{
  return getImplementation()->hessian(inP);
}

/* Accessor for parameter dimension */
UnsignedInteger Function::getParameterDimension() const
{
  return getImplementation()->getParameterDimension();
}

/* Accessor for input point dimension */
UnsignedInteger Function::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger Function::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();

}

/* Get the i-th marginal function */
Function Function::getMarginal(const UnsignedInteger i) const
{
  return *(getImplementation()->getMarginal(i));
}

/* Get the function corresponding to indices components */
Function Function::getMarginal(const Indices & indices) const
{
  return *(getImplementation()->getMarginal(indices));
}

/* Number of calls to the evaluation */
UnsignedInteger Function::getCallsNumber() const
{
  return getEvaluationCallsNumber();
}

UnsignedInteger Function::getEvaluationCallsNumber() const
{
  return getImplementation()->getEvaluationCallsNumber();
}

/* Number of calls to the gradient */
UnsignedInteger Function::getGradientCallsNumber() const
{
  return getImplementation()->getGradientCallsNumber();
}

/* Number of calls to the hessian */
UnsignedInteger Function::getHessianCallsNumber() const
{
  return getImplementation()->getHessianCallsNumber();
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph Function::draw(const UnsignedInteger inputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber,
                     const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph Function::draw(const UnsignedInteger firstInputMarginal,
                     const UnsignedInteger secondInputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber,
                     const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph Function::draw(const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber,
                     const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph Function::draw(const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber,
                     const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber, scale);
}


END_NAMESPACE_OPENTURNS
