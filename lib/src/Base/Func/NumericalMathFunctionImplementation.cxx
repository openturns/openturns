//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all function implementations
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/NumericalMathFunctionImplementation.hxx"
#include "openturns/NoNumericalMathEvaluationImplementation.hxx"
#include "openturns/NoNumericalMathGradientImplementation.hxx"
#include "openturns/NoNumericalMathHessianImplementation.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/AnalyticalNumericalMathEvaluationImplementation.hxx"
#include "openturns/AnalyticalNumericalMathGradientImplementation.hxx"
#include "openturns/AnalyticalNumericalMathHessianImplementation.hxx"
#endif
#include "openturns/DatabaseNumericalMathEvaluationImplementation.hxx"
#include "openturns/ProductNumericalMathFunction.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/CenteredFiniteDifferenceHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Os.hxx"
#include "openturns/SymbolicFunction.hxx"

#undef GetClassName

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NumericalMathFunctionImplementation);

static const Factory<NumericalMathFunctionImplementation> Factory_NumericalMathFunctionImplementation;

/* Default constructor */
NumericalMathFunctionImplementation::NumericalMathFunctionImplementation()
  : PersistentObject()
  , p_evaluationImplementation_(new NoNumericalMathEvaluationImplementation)
  , p_gradientImplementation_(new NoNumericalMathGradientImplementation)
  , p_hessianImplementation_(new NoNumericalMathHessianImplementation)
  , useDefaultGradientImplementation_(false)
  , useDefaultHessianImplementation_(false)
{
  // Nothing to do
}

/* Analytical formula constructor */
NumericalMathFunctionImplementation::NumericalMathFunctionImplementation(const Description & inputVariablesNames,
    const Description & outputVariablesNames,
    const Description & formulas)
  : PersistentObject()
  , p_evaluationImplementation_(new NoNumericalMathEvaluationImplementation)
  , p_gradientImplementation_(new NoNumericalMathGradientImplementation)
  , p_hessianImplementation_(new NoNumericalMathHessianImplementation)
  , useDefaultGradientImplementation_(true)
  , useDefaultHessianImplementation_(true)
{
#ifdef OPENTURNS_HAVE_MUPARSER
  // Try to build an analytical gradient
  AnalyticalNumericalMathEvaluationImplementation evaluation(inputVariablesNames, outputVariablesNames, formulas);
  p_evaluationImplementation_ = evaluation.clone();
  try
  {
    p_gradientImplementation_ = new AnalyticalNumericalMathGradientImplementation(evaluation);
    useDefaultGradientImplementation_ = false;
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical gradient, using finite differences instead.");
    p_gradientImplementation_ = new CenteredFiniteDifferenceGradient(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), p_evaluationImplementation_);
  }
  try
  {
    p_hessianImplementation_ = new AnalyticalNumericalMathHessianImplementation(evaluation);
    useDefaultHessianImplementation_ = false;
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical hessian, using finite differences instead.");
    p_hessianImplementation_ = new CenteredFiniteDifferenceHessian(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), p_evaluationImplementation_);
  }
#else
  throw NotYetImplementedException(HERE) << "In NumericalMathFunctionImplementation::NumericalMathFunctionImplementation(const Description & inputVariablesNames, const Description & outputVariablesNames, const Description & formulas): Analytical function requires muParser";
#endif
}


/* Constructor from a wrapper file */
NumericalMathFunctionImplementation::NumericalMathFunctionImplementation(const NumericalSample & inputSample,
    const NumericalSample & outputSample)
  : PersistentObject()
  , p_gradientImplementation_(new NoNumericalMathGradientImplementation)
  , p_hessianImplementation_(new NoNumericalMathHessianImplementation)
  , useDefaultGradientImplementation_(false)
  , useDefaultHessianImplementation_(false)
{
  p_evaluationImplementation_ = new DatabaseNumericalMathEvaluationImplementation( inputSample, outputSample );
}


/* Single function implementation constructor */
NumericalMathFunctionImplementation::NumericalMathFunctionImplementation(const EvaluationImplementation & evaluationImplementation)
  : PersistentObject()
  , p_evaluationImplementation_(evaluationImplementation)
  , p_gradientImplementation_(new CenteredFiniteDifferenceGradient(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), p_evaluationImplementation_))
  , p_hessianImplementation_(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), p_evaluationImplementation_))
  , useDefaultGradientImplementation_(true)
  , useDefaultHessianImplementation_(true)
{
  // Nothing to do
}

/* Constructor from implementations */
NumericalMathFunctionImplementation::NumericalMathFunctionImplementation(const EvaluationImplementation & evaluationImplementation,
    const GradientImplementation & gradientImplementation,
    const HessianImplementation  & hessianImplementation)
  : PersistentObject()
  , p_evaluationImplementation_(evaluationImplementation)
  , p_gradientImplementation_(gradientImplementation)
  , p_hessianImplementation_(hessianImplementation)
  , useDefaultGradientImplementation_(false)
  , useDefaultHessianImplementation_(false)
{
  // Nothing to do
}


/* Virtual constructor */
NumericalMathFunctionImplementation * NumericalMathFunctionImplementation::clone() const
{
  return new NumericalMathFunctionImplementation(*this);
}

/* Comparison operator */
Bool NumericalMathFunctionImplementation::operator ==(const NumericalMathFunctionImplementation & other) const
{
  if (this == &other) return true;
  return (*p_evaluationImplementation_ == *other.p_evaluationImplementation_) && (*p_gradientImplementation_ == *other.p_gradientImplementation_) && (*p_hessianImplementation_ == *other.p_hessianImplementation_);
}

/* String converter */
String NumericalMathFunctionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NumericalMathFunctionImplementation::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription()
      << " evaluationImplementation=" << p_evaluationImplementation_->__repr__()
      << " gradientImplementation=" << p_gradientImplementation_->__repr__()
      << " hessianImplementation=" << p_hessianImplementation_->__repr__();
  return oss;
}

/* String converter */
String NumericalMathFunctionImplementation::__str__(const String & offset) const
{
  return p_evaluationImplementation_->__str__(offset);
}

/* Description Accessor */
void NumericalMathFunctionImplementation::setDescription(const Description & description)
{
  p_evaluationImplementation_->setDescription(description);
}

/* Description Accessor */
Description NumericalMathFunctionImplementation::getDescription() const
{
  return p_evaluationImplementation_->getDescription();
}

/* Input description Accessor */
Description NumericalMathFunctionImplementation::getInputDescription() const
{
  return p_evaluationImplementation_->getInputDescription();
}

/* Output description Accessor */
Description NumericalMathFunctionImplementation::getOutputDescription() const
{
  return p_evaluationImplementation_->getOutputDescription();
}

/* Enable or disable the internal cache */
void NumericalMathFunctionImplementation::enableCache() const
{
  p_evaluationImplementation_->enableCache();
}

void NumericalMathFunctionImplementation::disableCache() const
{
  p_evaluationImplementation_->disableCache();
}

Bool NumericalMathFunctionImplementation::isCacheEnabled() const
{
  return p_evaluationImplementation_->isCacheEnabled();
}

UnsignedInteger NumericalMathFunctionImplementation::getCacheHits() const
{
  return p_evaluationImplementation_->getCacheHits();
}

void NumericalMathFunctionImplementation::addCacheContent(const NumericalSample& inSample,
    const NumericalSample& outSample)
{
  p_evaluationImplementation_->addCacheContent(inSample, outSample);
}

NumericalSample NumericalMathFunctionImplementation::getCacheInput() const
{
  return p_evaluationImplementation_->getCacheInput();
}

NumericalSample NumericalMathFunctionImplementation::getCacheOutput() const
{
  return p_evaluationImplementation_->getCacheOutput();
}

void NumericalMathFunctionImplementation::clearCache() const
{
  p_evaluationImplementation_->clearCache();
}

/* Enable or disable the input/output history */
void NumericalMathFunctionImplementation::enableHistory() const
{
  p_evaluationImplementation_->enableHistory();
}

void NumericalMathFunctionImplementation::disableHistory() const
{
  p_evaluationImplementation_->disableHistory();
}

Bool NumericalMathFunctionImplementation::isHistoryEnabled() const
{
  return p_evaluationImplementation_->isHistoryEnabled();
}

void NumericalMathFunctionImplementation::clearHistory() const
{
  p_evaluationImplementation_->clearHistory();
}

HistoryStrategy NumericalMathFunctionImplementation::getHistoryInput() const
{
  return p_evaluationImplementation_->getHistoryInput();
}

HistoryStrategy NumericalMathFunctionImplementation::getHistoryOutput() const
{
  return p_evaluationImplementation_->getHistoryOutput();
}

NumericalSample NumericalMathFunctionImplementation::getInputPointHistory() const
{
  return p_evaluationImplementation_->getInputPointHistory();
}

NumericalSample NumericalMathFunctionImplementation::getInputParameterHistory() const
{
  return p_evaluationImplementation_->getInputParameterHistory();
}

/* Multiplication operator between two functions with the same input dimension and 1D output dimension */
NumericalMathFunctionImplementation NumericalMathFunctionImplementation::operator * (const NumericalMathFunctionImplementation & right) const
{
  return ProductNumericalMathFunction(clone(), right.clone());
}

/* Multiplication operator between two functions with the same input dimension and 1D output dimension */
NumericalMathFunctionImplementation NumericalMathFunctionImplementation::operator * (const Implementation & p_right) const
{
  return ProductNumericalMathFunction(clone(), p_right);
}

/* Function implementation accessors */
void NumericalMathFunctionImplementation::setEvaluation(const EvaluationImplementation & evaluation)
{
  p_evaluationImplementation_ = evaluation;
}

const NumericalMathFunctionImplementation::EvaluationImplementation & NumericalMathFunctionImplementation::getEvaluation() const
{
  return p_evaluationImplementation_;
}

/* Gradient implementation accessors */
void NumericalMathFunctionImplementation::setGradient(const GradientImplementation & gradientImplementation)
{
  p_gradientImplementation_ = gradientImplementation;
  useDefaultGradientImplementation_ = false;
}

const NumericalMathFunctionImplementation::GradientImplementation & NumericalMathFunctionImplementation::getGradient() const
{
  return p_gradientImplementation_;
}

/* Hessian implementation accessors */
void NumericalMathFunctionImplementation::setHessian(const HessianImplementation & hessianImplementation)
{
  p_hessianImplementation_ = hessianImplementation;
  useDefaultHessianImplementation_ = false;
}

const NumericalMathFunctionImplementation::HessianImplementation & NumericalMathFunctionImplementation::getHessian() const
{
  return p_hessianImplementation_;
}

/* Flag for default gradient accessors */
Bool NumericalMathFunctionImplementation::getUseDefaultGradientImplementation() const
{
  return useDefaultGradientImplementation_;
}

void NumericalMathFunctionImplementation::setUseDefaultGradientImplementation(const Bool gradientFlag)
{
  useDefaultGradientImplementation_ = gradientFlag;
}

/* Flag for default hessian accessors */
Bool NumericalMathFunctionImplementation::getUseDefaultHessianImplementation() const
{
  return useDefaultHessianImplementation_;
}

void NumericalMathFunctionImplementation::setUseDefaultHessianImplementation(const Bool hessianFlag)
{
  useDefaultHessianImplementation_ = hessianFlag;
}


/* Gradient according to the marginal parameters */
Matrix NumericalMathFunctionImplementation::parameterGradient(const NumericalPoint & inP) const
{
  return p_evaluationImplementation_->parameterGradient(inP);
}

/* Gradient according to the marginal parameters */
Matrix NumericalMathFunctionImplementation::parameterGradient(const NumericalPoint & inP,
    const NumericalPoint & parameter)
{
  setParameter(parameter);
  return p_evaluationImplementation_->parameterGradient(inP);
}

/* Parameters value accessor */
NumericalPoint NumericalMathFunctionImplementation::getParameter() const
{
  return p_evaluationImplementation_->getParameter();
}

void NumericalMathFunctionImplementation::setParameter(const NumericalPoint & parameter)
{
  p_evaluationImplementation_->setParameter(parameter);
  p_gradientImplementation_->setParameter(parameter);
  p_hessianImplementation_->setParameter(parameter);
}

/* Parameters description accessor */
Description NumericalMathFunctionImplementation::getParameterDescription() const
{
  return p_evaluationImplementation_->getParameterDescription();
}

void NumericalMathFunctionImplementation::setParameterDescription(const Description & description)
{
  p_evaluationImplementation_->setParameterDescription(description);
}

/* Operator () */
NumericalPoint NumericalMathFunctionImplementation::operator() (const NumericalPoint & inP) const
{
  return p_evaluationImplementation_->operator()(inP);
}

NumericalPoint NumericalMathFunctionImplementation::operator() (const NumericalPoint & inP,
    const NumericalPoint & parameter)
{
  setParameter(parameter);
  return p_evaluationImplementation_->operator()(inP);
}

NumericalSample NumericalMathFunctionImplementation::operator() (const NumericalPoint & inP,
    const NumericalSample & parameters)
{
  return p_evaluationImplementation_->operator()(inP, parameters);
}

/* Operator () */
NumericalSample NumericalMathFunctionImplementation::operator() (const NumericalSample & inSample) const
{
  return p_evaluationImplementation_->operator()(inSample);
}

/* Operator () */
Field NumericalMathFunctionImplementation::operator() (const Field & inField) const
{
  return p_evaluationImplementation_->operator()(inField);
}

/* Method gradient() returns the Jacobian transposed matrix of the function at point */
Matrix NumericalMathFunctionImplementation::gradient(const NumericalPoint & inP) const
{
  if (useDefaultGradientImplementation_) LOGWARN(OSS() << "You are using a default implementation for the gradient. Be careful, your computation can be severely wrong!");
  // Here we must catch the exceptions raised by functions with no gradient
  try
  {
    return p_gradientImplementation_->gradient(inP);
  }
  catch (...)
  {
    // Fallback on non-centered finite difference gradient
    try
    {
      LOGWARN(OSS() << "Switch to finite difference to compute the gradient at point=" << inP);
      const CenteredFiniteDifferenceGradient gradientFD(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), p_evaluationImplementation_);
      return gradientFD.gradient(inP);
    }
    catch (...)
    {
      throw InternalException(HERE) << "Error: cannot compute gradient at point=" << inP;
    }
  } // Usual gradient failed
}

Matrix NumericalMathFunctionImplementation::gradient(const NumericalPoint & inP,
    const NumericalPoint & parameters)
{
  if (useDefaultGradientImplementation_) LOGWARN(OSS() << "You are using a default implementation for the gradient. Be careful, your computation can be severely wrong!");
  setParameter(parameters);
  return p_gradientImplementation_->gradient(inP);
}

/* Method hessian() returns the symetric tensor of the function at point */
SymmetricTensor NumericalMathFunctionImplementation::hessian(const NumericalPoint & inP) const
{
  if (useDefaultHessianImplementation_) LOGWARN(OSS() << "You are using a default implementation for the hessian. Be careful, your computation can be severely wrong!");
  // Here we must catch the exceptions raised by functions with no gradient
  try
  {
    return p_hessianImplementation_->hessian(inP);
  }
  catch (...)
  {
    // Fallback on non-centered finite difference gradient
    try
    {
      LOGWARN(OSS() << "Switch to finite difference to compute the hessian at point=" << inP);
      const CenteredFiniteDifferenceHessian hessianFD(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), p_evaluationImplementation_);
      return hessianFD.hessian(inP);
    }
    catch (...)
    {
      throw InternalException(HERE) << "Error: cannot compute hessian at point=" << inP;
    }
  } // Usual gradient failed
}

SymmetricTensor NumericalMathFunctionImplementation::hessian(const NumericalPoint & inP,
    const NumericalPoint & parameters)
{
  if (useDefaultHessianImplementation_) LOGWARN(OSS() << "You are using a default implementation for the hessian. Be careful, your computation can be severely wrong!");
  setParameter(parameters);
  return p_hessianImplementation_->hessian(inP);
}

/* Accessor for parameter dimension */
UnsignedInteger NumericalMathFunctionImplementation::getParameterDimension() const
{
  return p_evaluationImplementation_->getParameterDimension();
}

/* Accessor for input point dimension */
UnsignedInteger NumericalMathFunctionImplementation::getInputDimension() const
{
  return p_evaluationImplementation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger NumericalMathFunctionImplementation::getOutputDimension() const
{
  return p_evaluationImplementation_->getOutputDimension();
}

/* Get the i-th marginal function */
NumericalMathFunctionImplementation::Implementation NumericalMathFunctionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
NumericalMathFunctionImplementation::Implementation NumericalMathFunctionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new NumericalMathFunctionImplementation(p_evaluationImplementation_->getMarginal(indices), p_gradientImplementation_->getMarginal(indices), p_hessianImplementation_->getMarginal(indices));
}

/* Number of calls to the evaluation */
UnsignedInteger NumericalMathFunctionImplementation::getEvaluationCallsNumber() const
{
  return p_evaluationImplementation_->getCallsNumber();
}

/* Number of calls to the gradient */
UnsignedInteger NumericalMathFunctionImplementation::getGradientCallsNumber() const
{
  return p_gradientImplementation_->getCallsNumber();
}

/* Number of calls to the hessian */
UnsignedInteger NumericalMathFunctionImplementation::getHessianCallsNumber() const
{
  return p_hessianImplementation_->getCallsNumber();
}

/* Static methods for documentation of analytical fonctions */
Description NumericalMathFunctionImplementation::GetValidConstants()
{
  Log::Warn(OSS() << "NumericalMathFunction:GetValidConstants is deprecated");
  return SymbolicFunction::GetValidConstants();
}

Description NumericalMathFunctionImplementation::GetValidFunctions()
{
  Log::Warn(OSS() << "NumericalMathFunction:GetValidFunctions is deprecated");
  return SymbolicFunction::GetValidFunctions();
}

Description NumericalMathFunctionImplementation::GetValidOperators()
{
  Log::Warn(OSS() << "NumericalMathFunction:GetValidOperators is deprecated");
  return SymbolicFunction::GetValidOperators();
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph NumericalMathFunctionImplementation::draw(const UnsignedInteger inputMarginal,
    const UnsignedInteger outputMarginal,
    const NumericalPoint & centralPoint,
    const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph NumericalMathFunctionImplementation::draw(const UnsignedInteger firstInputMarginal,
    const UnsignedInteger secondInputMarginal,
    const UnsignedInteger outputMarginal,
    const NumericalPoint & centralPoint,
    const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph NumericalMathFunctionImplementation::draw(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph NumericalMathFunctionImplementation::draw(const NumericalPoint & xMin,
    const NumericalPoint & xMax,
    const Indices & pointNumber,
    const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(xMin, xMax, pointNumber, scale);
}

/* Method save() stores the object through the StorageManager */
void NumericalMathFunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluationImplementation_", *p_evaluationImplementation_ );
  adv.saveAttribute( "gradientImplementation_", *p_gradientImplementation_ );
  adv.saveAttribute( "hessianImplementation_", *p_hessianImplementation_ );
  adv.saveAttribute( "useDefaultGradientImplementation_", useDefaultGradientImplementation_ );
  adv.saveAttribute( "useDefaultHessianImplementation_", useDefaultHessianImplementation_ );
}

/* Method load() reloads the object from the StorageManager */
void NumericalMathFunctionImplementation::load(Advocate & adv)
{
  TypedInterfaceObject<NumericalMathEvaluationImplementation> evaluationValue;
  TypedInterfaceObject<NumericalMathGradientImplementation> gradientValue;
  TypedInterfaceObject<NumericalMathHessianImplementation> hessianValue;
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluationImplementation_", evaluationValue );
  p_evaluationImplementation_ = evaluationValue.getImplementation();
  adv.loadAttribute( "gradientImplementation_", gradientValue );
  p_gradientImplementation_ = gradientValue.getImplementation();
  adv.loadAttribute( "hessianImplementation_", hessianValue );
  p_hessianImplementation_ = hessianValue.getImplementation();
  adv.loadAttribute( "useDefaultGradientImplementation_", useDefaultGradientImplementation_ );
  adv.loadAttribute( "useDefaultHessianImplementation_", useDefaultHessianImplementation_ );
}




END_NAMESPACE_OPENTURNS
