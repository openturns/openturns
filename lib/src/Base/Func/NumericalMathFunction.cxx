//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "NumericalMathFunction.hxx"
#include "ComposedNumericalMathFunction.hxx"
#include "AggregatedNumericalMathEvaluationImplementation.hxx"
#include "AggregatedNumericalMathGradientImplementation.hxx"
#include "AggregatedNumericalMathHessianImplementation.hxx"
#include "IndicatorNumericalMathEvaluationImplementation.hxx"
#include "DualLinearCombinationEvaluationImplementation.hxx"
#include "DualLinearCombinationGradientImplementation.hxx"
#include "DualLinearCombinationHessianImplementation.hxx"
#include "LinearCombinationEvaluationImplementation.hxx"
#include "LinearCombinationGradientImplementation.hxx"
#include "LinearCombinationHessianImplementation.hxx"
#include "NoNumericalMathGradientImplementation.hxx"
#include "NoNumericalMathHessianImplementation.hxx"
#include "ParametricEvaluationImplementation.hxx"
#include "ParametricGradientImplementation.hxx"
#include "ParametricHessianImplementation.hxx"
#include "Log.hxx"
#include "Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NumericalMathFunction);


/* Default constructor */
NumericalMathFunction::NumericalMathFunction()
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation())
{
  // Nothing to do
}

/* Constructor from NumericalMathFunctionImplementation */
NumericalMathFunction::NumericalMathFunction(const NumericalMathFunctionImplementation & implementation)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
NumericalMathFunction::NumericalMathFunction(const Implementation & p_implementation)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
NumericalMathFunction::NumericalMathFunction(NumericalMathFunctionImplementation * p_implementation)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(p_implementation)
{
  // Nothing to do
}


/* Constructor from implementation pointer */
NumericalMathFunction::NumericalMathFunction(const NumericalMathEvaluationImplementation & evaluation)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation(evaluation.clone()))
{
  // Nothing to do
}


/* Composition constructor */
NumericalMathFunction::NumericalMathFunction(const NumericalMathFunction & left,
    const NumericalMathFunction & right)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new ComposedNumericalMathFunction(left.getImplementation(), right.getImplementation()))
{
  // Nothing to do
}

/* Analytical formula constructor */
NumericalMathFunction::NumericalMathFunction(const Description & inputVariablesNames,
    const Description & outputVariablesNames,
    const Description & formulas)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation(inputVariablesNames, outputVariablesNames, formulas))
{
  // Nothing to do
}

/* Analytical formula constructor */
NumericalMathFunction::NumericalMathFunction(const Description & inputVariablesNames,
    const Description & formulas)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>()
{
  const UnsignedInteger size(formulas.getSize());
  Description outputVariablesNames(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    outputVariablesNames[i] = String(OSS() << "y" << i);
  *this = NumericalMathFunction(inputVariablesNames, outputVariablesNames, formulas);
}

/* Indicator function constructor */
NumericalMathFunction::NumericalMathFunction(const NumericalMathFunction & function,
    const ComparisonOperator & comparisonOperator,
    const NumericalScalar threshold)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation(new IndicatorNumericalMathEvaluationImplementation(function.getEvaluation(), comparisonOperator, threshold), new NoNumericalMathGradientImplementation(), new NoNumericalMathHessianImplementation()))
{
  // Nothing to do
}

/* Aggregated function constructor: the output is the aggregation of the several functions */
NumericalMathFunction::NumericalMathFunction(const NumericalMathFunctionCollection & functionCollection)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation())
{
  const AggregatedNumericalMathEvaluationImplementation evaluation(functionCollection);
  setEvaluation(evaluation.clone());
  setGradient(new AggregatedNumericalMathGradientImplementation(evaluation));
  setHessian(new AggregatedNumericalMathHessianImplementation(evaluation));
}

/* Linear combination function constructor */
NumericalMathFunction::NumericalMathFunction(const NumericalMathFunctionCollection & functionCollection,
    const NumericalPoint & coefficients)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation())
{
  const LinearCombinationEvaluationImplementation evaluation(functionCollection, coefficients);
  setEvaluation(evaluation.clone());
  setGradient(new LinearCombinationGradientImplementation(evaluation));
  setHessian(new LinearCombinationHessianImplementation(evaluation));
}

/* Dual linear combination function constructor */
NumericalMathFunction::NumericalMathFunction(const NumericalMathFunctionCollection & functionCollection,
    const NumericalSample & coefficients)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation())
{
  const DualLinearCombinationEvaluationImplementation evaluation(functionCollection, coefficients);
  setEvaluation(evaluation.clone());
  setGradient(new DualLinearCombinationGradientImplementation(evaluation));
  setHessian(new DualLinearCombinationHessianImplementation(evaluation));
}

/* Simplified analytical formula constructor */
NumericalMathFunction::NumericalMathFunction(const String & inputVariableName,
    const String & formula,
    const String & outputVariableName)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation(Description(1, inputVariableName), Description(1, outputVariableName), Description(1, formula)))
{
  // Nothing to do
}

/* Constructor from implementations */
NumericalMathFunction::NumericalMathFunction(const EvaluationImplementation & evaluationImplementation,
    const GradientImplementation & gradientImplementation,
    const HessianImplementation  & hessianImplementation)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation(evaluationImplementation, gradientImplementation, hessianImplementation))
{
  // Nothing to do
}

/* Constructor from samples */
NumericalMathFunction::NumericalMathFunction(const NumericalSample & inputSample,
    const NumericalSample & outputSample)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation( inputSample, outputSample ))
{
  // Nothing to do
}

/* Constructor by splitting the input of a function between variables and parameters */
NumericalMathFunction::NumericalMathFunction(const NumericalMathFunction & function,
    const Indices & set,
    const Bool parametersSet)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation())
{
  const Pointer<ParametricEvaluationImplementation> p_evaluation = new ParametricEvaluationImplementation(function, set, NumericalPoint(function.getInputDimension()), parametersSet);
  setEvaluation(p_evaluation);
  setGradient(new ParametricGradientImplementation(p_evaluation));
  setHessian(new ParametricHessianImplementation(p_evaluation));
}


NumericalMathFunction::NumericalMathFunction(const NumericalMathFunction & function,
    const Indices & set,
    const NumericalPoint & referencePoint,
    const Bool parametersSet)
  : TypedInterfaceObject<NumericalMathFunctionImplementation>(new NumericalMathFunctionImplementation())
{
  const Pointer<ParametricEvaluationImplementation> p_evaluation = new ParametricEvaluationImplementation(function, set, referencePoint, parametersSet);
  setEvaluation(p_evaluation);
  setGradient(new ParametricGradientImplementation(p_evaluation));
  setHessian(new ParametricHessianImplementation(p_evaluation));
}

/* Comparison operator */
Bool NumericalMathFunction::operator ==(const NumericalMathFunction & other) const
{
  if (this == &other) return true;
  return *getImplementation() == *other.getImplementation();
}

/* String converter */
String NumericalMathFunction::__repr__() const
{
  return OSS(true) << "class=" << NumericalMathFunction::GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String NumericalMathFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Description Accessor */
void NumericalMathFunction::setDescription(const Description & description)
{
  copyOnWrite();
  getImplementation()->setDescription(description);
}

/* Description Accessor */
Description NumericalMathFunction::getDescription() const
{
  return getImplementation()->getDescription();
}

/* Input description Accessor */
Description NumericalMathFunction::getInputDescription() const
{
  return getImplementation()->getInputDescription();
}

/* Output description Accessor */
Description NumericalMathFunction::getOutputDescription() const
{
  return getImplementation()->getOutputDescription();
}

/* Enable or disable the internal cache */
void NumericalMathFunction::enableCache() const
{
  getImplementation()->enableCache();
}

void NumericalMathFunction::disableCache() const
{
  getImplementation()->disableCache();
}

Bool NumericalMathFunction::isCacheEnabled() const
{
  return getImplementation()->isCacheEnabled();
}

UnsignedInteger NumericalMathFunction::getCacheHits() const
{
  return getImplementation()->getCacheHits();
}

void NumericalMathFunction::addCacheContent(const NumericalSample & inSample, const NumericalSample & outSample)
{
  getImplementation()->addCacheContent(inSample, outSample);
}

NumericalSample NumericalMathFunction::getCacheInput() const
{
  return getImplementation()->getCacheInput();
}

NumericalSample NumericalMathFunction::getCacheOutput() const
{
  return getImplementation()->getCacheOutput();
}

void NumericalMathFunction::clearCache() const
{
  return getImplementation()->clearCache();
}

/* Enable or disable the input/output history */
void NumericalMathFunction::enableHistory() const
{
  return getImplementation()->enableHistory();
}

void NumericalMathFunction::disableHistory() const
{
  return getImplementation()->disableHistory();
}

Bool NumericalMathFunction::isHistoryEnabled() const
{
  return getImplementation()->isHistoryEnabled();
}

void NumericalMathFunction::clearHistory() const
{
  return getImplementation()->clearHistory();
}

HistoryStrategy NumericalMathFunction::getHistoryInput() const
{
  return getImplementation()->getHistoryInput();
}

HistoryStrategy NumericalMathFunction::getHistoryOutput() const
{
  return getImplementation()->getHistoryOutput();
}

/* Multiplication operator between two functions with the same input dimension and 1D output dimension */
ProductNumericalMathFunction NumericalMathFunction::operator * (const NumericalMathFunction & right) const
{
  return ProductNumericalMathFunction(getImplementation(), right.getImplementation());
}

/* Addition operator between two functions with the same input dimension and output dimension */
NumericalMathFunction NumericalMathFunction::operator + (const NumericalMathFunction & right) const
{
  const NumericalPoint coefficients(2, 1.0);
  NumericalMathFunctionCollection collection(2);
  collection[0] = *this;
  collection[1] = right;
  const LinearCombinationEvaluationImplementation evaluation(collection, coefficients);
  return NumericalMathFunction(evaluation.clone(), LinearCombinationGradientImplementation(evaluation).clone(), LinearCombinationHessianImplementation(evaluation).clone());
}

/* Soustraction operator between two functions with the same input dimension and output dimension */
NumericalMathFunction NumericalMathFunction::operator - (const NumericalMathFunction & right) const
{
  NumericalPoint coefficients(2, 1.0);
  coefficients[1] = -1.0;
  NumericalMathFunctionCollection collection(2);
  collection[0] = *this;
  collection[1] = right;
  const LinearCombinationEvaluationImplementation evaluation(collection, coefficients);
  return NumericalMathFunction(evaluation.clone(), LinearCombinationGradientImplementation(evaluation).clone(), LinearCombinationHessianImplementation(evaluation).clone());
}

/* Function implementation accessors */
void NumericalMathFunction::setEvaluation(const EvaluationImplementation & functionImplementation)
{
  copyOnWrite();
  getImplementation()->setEvaluation(functionImplementation);
}

const NumericalMathFunction::EvaluationImplementation & NumericalMathFunction::getEvaluation() const
{
  return getImplementation()->getEvaluation();
}

/* Gradient implementation accessors */
void NumericalMathFunction::setGradient(const NumericalMathGradientImplementation & gradientImplementation)
{
  copyOnWrite();
  getImplementation()->setGradient(GradientImplementation(gradientImplementation.clone()));
}

void NumericalMathFunction::setGradient(const GradientImplementation & gradientImplementation)
{
  copyOnWrite();
  getImplementation()->setGradient(gradientImplementation);
}

const NumericalMathFunction::GradientImplementation & NumericalMathFunction::getGradient() const
{
  return getImplementation()->getGradient();
}

/* Hessian implementation accessors */
void NumericalMathFunction::setHessian(const NumericalMathHessianImplementation & hessianImplementation)
{
  copyOnWrite();
  getImplementation()->setHessian(HessianImplementation(hessianImplementation.clone()));
}

void NumericalMathFunction::setHessian(const HessianImplementation & hessianImplementation)
{
  copyOnWrite();
  getImplementation()->setHessian(hessianImplementation);
}

const NumericalMathFunction::HessianImplementation & NumericalMathFunction::getHessian() const
{
  return getImplementation()->getHessian();
}

/* Initial Function implementation accessors */
const NumericalMathFunction::EvaluationImplementation & NumericalMathFunction::getInitialEvaluationImplementation() const
{
  return getImplementation()->getInitialEvaluationImplementation();
}

/* Initial gradient implementation accessors */
const NumericalMathFunction::GradientImplementation & NumericalMathFunction::getInitialGradientImplementation() const
{
  return getImplementation()->getInitialGradientImplementation();
}

/* Initial hessian implementation accessors */
const NumericalMathFunction::HessianImplementation & NumericalMathFunction::getInitialHessianImplementation() const
{
  return getImplementation()->getInitialHessianImplementation();
}

/* Flag for default gradient accessors */
Bool NumericalMathFunction::getUseDefaultGradientImplementation() const
{
  return getImplementation()->getUseDefaultGradientImplementation();
}

void NumericalMathFunction::setUseDefaultGradientImplementation(const Bool gradientFlag)
{
  copyOnWrite();
  getImplementation()->setUseDefaultGradientImplementation(gradientFlag);
}

/* Flag for default hessian accessors */
Bool NumericalMathFunction::getUseDefaultHessianImplementation() const
{
  return getImplementation()->getUseDefaultHessianImplementation();
}

void NumericalMathFunction::setUseDefaultHessianImplementation(const Bool hessianFlag)
{
  copyOnWrite();
  getImplementation()->setUseDefaultHessianImplementation(hessianFlag);
}


/* Gradient according to the marginal parameters */
Matrix NumericalMathFunction::parameterGradient(const NumericalPoint & inP) const
{
  return getImplementation()->parameterGradient(inP);
}

Matrix NumericalMathFunction::parameterGradient(const NumericalPoint & inP,
    const NumericalPoint & parameters)
{
  copyOnWrite();
  return getImplementation()->parameterGradient(inP, parameters);
}

/* Parameters value and description accessor */
NumericalPointWithDescription NumericalMathFunction::getParameter() const
{
  return getImplementation()->getParameter();
}

void NumericalMathFunction::setParameter(const NumericalPointWithDescription & parameters)
{
  copyOnWrite();
  getImplementation()->setParameter(parameters);
}


/* Operator () */
NumericalPoint NumericalMathFunction::operator() (const NumericalPoint & inP) const
{
  return getImplementation()->operator()(inP);
}

NumericalPoint NumericalMathFunction::operator() (const NumericalPoint & inP,
    const NumericalPoint & parameters)
{
  copyOnWrite();
  return getImplementation()->operator()(inP, parameters);
}

/* Operator () */
NumericalSample NumericalMathFunction::operator() (const NumericalSample & inSample) const
{
  return getImplementation()->operator()(inSample);
}

/* Operator () */
Field NumericalMathFunction::operator() (const Field & inField) const
{
  return getImplementation()->operator()(inField);
}

/* Method gradient() returns the Jacobian transposed matrix of the function at point */
Matrix NumericalMathFunction::gradient(const NumericalPoint & inP) const
{
  return getImplementation()->gradient(inP);
}

Matrix NumericalMathFunction::gradient(const NumericalPoint & inP,
                                       const NumericalPoint & parameters)
{
  copyOnWrite();
  return getImplementation()->gradient(inP, parameters);
}

/* Method hessian() returns the symmetric tensor of the function at point */
SymmetricTensor NumericalMathFunction::hessian(const NumericalPoint & inP) const
{
  return getImplementation()->hessian(inP);
}

SymmetricTensor NumericalMathFunction::hessian(const NumericalPoint & inP,
    const NumericalPoint & parameters)
{
  copyOnWrite();
  return getImplementation()->hessian(inP, parameters);
}


/* Accessor for parameter dimension */
UnsignedInteger NumericalMathFunction::getParameterDimension() const
{
  return getImplementation()->getParameterDimension();
}

/* Accessor for input point dimension */
UnsignedInteger NumericalMathFunction::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger NumericalMathFunction::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();

}

/* Get the i-th marginal function */
NumericalMathFunction NumericalMathFunction::getMarginal(const UnsignedInteger i) const
{
  return *(getImplementation()->getMarginal(i));
}

/* Get the function corresponding to indices components */
NumericalMathFunction NumericalMathFunction::getMarginal(const Indices & indices) const
{
  return *(getImplementation()->getMarginal(indices));
}

/* Number of calls to the evaluation */
UnsignedInteger NumericalMathFunction::getCallsNumber() const
{
  return getEvaluationCallsNumber();
}

UnsignedInteger NumericalMathFunction::getEvaluationCallsNumber() const
{
  return getImplementation()->getEvaluationCallsNumber();
}

/* Number of calls to the gradient */
UnsignedInteger NumericalMathFunction::getGradientCallsNumber() const
{
  return getImplementation()->getGradientCallsNumber();
}

/* Number of calls to the hessian */
UnsignedInteger NumericalMathFunction::getHessianCallsNumber() const
{
  return getImplementation()->getHessianCallsNumber();
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph NumericalMathFunction::draw(const UnsignedInteger inputMarginal,
                                  const UnsignedInteger outputMarginal,
                                  const NumericalPoint & centralPoint,
                                  const NumericalScalar xMin,
                                  const NumericalScalar xMax,
                                  const UnsignedInteger pointNumber,
                                  const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph NumericalMathFunction::draw(const UnsignedInteger firstInputMarginal,
                                  const UnsignedInteger secondInputMarginal,
                                  const UnsignedInteger outputMarginal,
                                  const NumericalPoint & centralPoint,
                                  const NumericalPoint & xMin,
                                  const NumericalPoint & xMax,
                                  const Indices & pointNumber,
                                  const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph NumericalMathFunction::draw(const NumericalScalar xMin,
                                  const NumericalScalar xMax,
                                  const UnsignedInteger pointNumber,
                                  const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph NumericalMathFunction::draw(const NumericalPoint & xMin,
                                  const NumericalPoint & xMax,
                                  const Indices & pointNumber,
                                  const GraphImplementation::LogScale scale) const
{
  return getImplementation()->draw(xMin, xMax, pointNumber, scale);
}

/* Static methods for documentation of analytical fnctions */
Description NumericalMathFunction::GetValidConstants()
{
  return NumericalMathFunctionImplementation::GetValidConstants();
}

Description NumericalMathFunction::GetValidFunctions()
{
  return NumericalMathFunctionImplementation::GetValidFunctions();
}

Description NumericalMathFunction::GetValidOperators()
{
  return NumericalMathFunctionImplementation::GetValidOperators();
}



END_NAMESPACE_OPENTURNS
