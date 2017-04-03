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
#include "openturns/Function.hxx"
#include "openturns/FunctionImplementation.hxx"
#include "openturns/NoEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/SymbolicGradient.hxx"
#include "openturns/SymbolicHessian.hxx"
#endif
#include "openturns/DatabaseEvaluation.hxx"
#include "openturns/ProductFunction.hxx"
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

CLASSNAMEINIT(FunctionImplementation);

static const Factory<FunctionImplementation> Factory_FunctionImplementation;

/* Default constructor */
FunctionImplementation::FunctionImplementation()
  : PersistentObject()
  , p_evaluationImplementation_(new NoEvaluation)
  , p_gradientImplementation_(new NoGradient)
  , p_hessianImplementation_(new NoHessian)
  , useDefaultGradientImplementation_(false)
  , useDefaultHessianImplementation_(false)
{
  // Nothing to do
}

/* Analytical formula constructor */
FunctionImplementation::FunctionImplementation(const Description & inputVariablesNames,
    const Description & outputVariablesNames,
    const Description & formulas)
  : PersistentObject()
  , p_evaluationImplementation_(new NoEvaluation)
  , p_gradientImplementation_(new NoGradient)
  , p_hessianImplementation_(new NoHessian)
  , useDefaultGradientImplementation_(true)
  , useDefaultHessianImplementation_(true)
{
#ifdef OPENTURNS_HAVE_MUPARSER
  // Try to build an analytical gradient
  SymbolicEvaluation evaluation(inputVariablesNames, outputVariablesNames, formulas);
  p_evaluationImplementation_ = evaluation.clone();
  try
  {
    p_gradientImplementation_ = new SymbolicGradient(evaluation);
    useDefaultGradientImplementation_ = false;
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical gradient, using finite differences instead.");
    p_gradientImplementation_ = new CenteredFiniteDifferenceGradient(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), p_evaluationImplementation_);
  }
  try
  {
    p_hessianImplementation_ = new SymbolicHessian(evaluation);
    useDefaultHessianImplementation_ = false;
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical hessian, using finite differences instead.");
    p_hessianImplementation_ = new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), p_evaluationImplementation_);
  }
#else
  throw NotYetImplementedException(HERE) << "In FunctionImplementation::FunctionImplementation(const Description & inputVariablesNames, const Description & outputVariablesNames, const Description & formulas): Analytical function requires muParser";
#endif
}


/* Constructor from a wrapper file */
FunctionImplementation::FunctionImplementation(const Sample & inputSample,
    const Sample & outputSample)
  : PersistentObject()
  , p_gradientImplementation_(new NoGradient)
  , p_hessianImplementation_(new NoHessian)
  , useDefaultGradientImplementation_(false)
  , useDefaultHessianImplementation_(false)
{
  p_evaluationImplementation_ = new DatabaseEvaluation( inputSample, outputSample );
}


/* Single function implementation constructor */
FunctionImplementation::FunctionImplementation(const EvaluationPointer & evaluationImplementation)
  : PersistentObject()
  , p_evaluationImplementation_(evaluationImplementation)
  , p_gradientImplementation_(new CenteredFiniteDifferenceGradient(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), p_evaluationImplementation_))
  , p_hessianImplementation_(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), p_evaluationImplementation_))
  , useDefaultGradientImplementation_(true)
  , useDefaultHessianImplementation_(true)
{
  // Nothing to do
}

/* Constructor from implementations */
FunctionImplementation::FunctionImplementation(const EvaluationPointer & evaluationImplementation,
    const GradientPointer & gradientImplementation,
    const HessianPointer  & hessianImplementation)
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
FunctionImplementation * FunctionImplementation::clone() const
{
  return new FunctionImplementation(*this);
}

/* Comparison operator */
Bool FunctionImplementation::operator ==(const FunctionImplementation & other) const
{
  if (this == &other) return true;
  return (*p_evaluationImplementation_ == *other.p_evaluationImplementation_) && (*p_gradientImplementation_ == *other.p_gradientImplementation_) && (*p_hessianImplementation_ == *other.p_hessianImplementation_);
}

/* String converter */
String FunctionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FunctionImplementation::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription()
      << " evaluationImplementation=" << p_evaluationImplementation_->__repr__()
      << " gradientImplementation=" << p_gradientImplementation_->__repr__()
      << " hessianImplementation=" << p_hessianImplementation_->__repr__();
  return oss;
}

/* String converter */
String FunctionImplementation::__str__(const String & offset) const
{
  return p_evaluationImplementation_->__str__(offset);
}

/* Description Accessor */
void FunctionImplementation::setDescription(const Description & description)
{
  p_evaluationImplementation_->setDescription(description);
}

/* Description Accessor */
Description FunctionImplementation::getDescription() const
{
  return p_evaluationImplementation_->getDescription();
}

/* Input description Accessor */
Description FunctionImplementation::getInputDescription() const
{
  return p_evaluationImplementation_->getInputDescription();
}

/* Output description Accessor */
Description FunctionImplementation::getOutputDescription() const
{
  return p_evaluationImplementation_->getOutputDescription();
}

/* Enable or disable the internal cache */
void FunctionImplementation::enableCache() const
{
  p_evaluationImplementation_->enableCache();
}

void FunctionImplementation::disableCache() const
{
  p_evaluationImplementation_->disableCache();
}

Bool FunctionImplementation::isCacheEnabled() const
{
  return p_evaluationImplementation_->isCacheEnabled();
}

UnsignedInteger FunctionImplementation::getCacheHits() const
{
  return p_evaluationImplementation_->getCacheHits();
}

void FunctionImplementation::addCacheContent(const Sample& inSample,
    const Sample& outSample)
{
  p_evaluationImplementation_->addCacheContent(inSample, outSample);
}

Sample FunctionImplementation::getCacheInput() const
{
  return p_evaluationImplementation_->getCacheInput();
}

Sample FunctionImplementation::getCacheOutput() const
{
  return p_evaluationImplementation_->getCacheOutput();
}

void FunctionImplementation::clearCache() const
{
  p_evaluationImplementation_->clearCache();
}

/* Enable or disable the input/output history */
void FunctionImplementation::enableHistory() const
{
  p_evaluationImplementation_->enableHistory();
}

void FunctionImplementation::disableHistory() const
{
  p_evaluationImplementation_->disableHistory();
}

Bool FunctionImplementation::isHistoryEnabled() const
{
  return p_evaluationImplementation_->isHistoryEnabled();
}

void FunctionImplementation::clearHistory() const
{
  p_evaluationImplementation_->clearHistory();
}

HistoryStrategy FunctionImplementation::getHistoryInput() const
{
  return p_evaluationImplementation_->getHistoryInput();
}

HistoryStrategy FunctionImplementation::getHistoryOutput() const
{
  return p_evaluationImplementation_->getHistoryOutput();
}

Sample FunctionImplementation::getInputPointHistory() const
{
  return p_evaluationImplementation_->getInputPointHistory();
}

Sample FunctionImplementation::getInputParameterHistory() const
{
  return p_evaluationImplementation_->getInputParameterHistory();
}

/* Multiplication operator between two functions with the same input dimension and 1D output dimension */
FunctionImplementation FunctionImplementation::operator * (const FunctionImplementation & right) const
{
  return ProductFunction(clone(), right.clone());
}

/* Multiplication operator between two functions with the same input dimension and 1D output dimension */
FunctionImplementation FunctionImplementation::operator * (const Implementation & p_right) const
{
  return ProductFunction(clone(), p_right);
}

/* Function implementation accessors */
void FunctionImplementation::setEvaluation(const EvaluationPointer & evaluation)
{
  p_evaluationImplementation_ = evaluation;
}

const FunctionImplementation::EvaluationPointer & FunctionImplementation::getEvaluation() const
{
  return p_evaluationImplementation_;
}

/* Gradient implementation accessors */
void FunctionImplementation::setGradient(const GradientPointer & gradientImplementation)
{
  p_gradientImplementation_ = gradientImplementation;
  useDefaultGradientImplementation_ = false;
}

const FunctionImplementation::GradientPointer & FunctionImplementation::getGradient() const
{
  return p_gradientImplementation_;
}

/* Hessian implementation accessors */
void FunctionImplementation::setHessian(const HessianPointer & hessianImplementation)
{
  p_hessianImplementation_ = hessianImplementation;
  useDefaultHessianImplementation_ = false;
}

const FunctionImplementation::HessianPointer & FunctionImplementation::getHessian() const
{
  return p_hessianImplementation_;
}

/* Flag for default gradient accessors */
Bool FunctionImplementation::getUseDefaultGradientImplementation() const
{
  return useDefaultGradientImplementation_;
}

void FunctionImplementation::setUseDefaultGradientImplementation(const Bool gradientFlag)
{
  useDefaultGradientImplementation_ = gradientFlag;
}

/* Flag for default hessian accessors */
Bool FunctionImplementation::getUseDefaultHessianImplementation() const
{
  return useDefaultHessianImplementation_;
}

void FunctionImplementation::setUseDefaultHessianImplementation(const Bool hessianFlag)
{
  useDefaultHessianImplementation_ = hessianFlag;
}


/* Gradient according to the marginal parameters */
Matrix FunctionImplementation::parameterGradient(const Point & inP) const
{
  return p_evaluationImplementation_->parameterGradient(inP);
}

/* Gradient according to the marginal parameters */
Matrix FunctionImplementation::parameterGradient(const Point & inP,
    const Point & parameter)
{
  setParameter(parameter);
  return p_evaluationImplementation_->parameterGradient(inP);
}

/* Parameters value accessor */
Point FunctionImplementation::getParameter() const
{
  return p_evaluationImplementation_->getParameter();
}

void FunctionImplementation::setParameter(const Point & parameter)
{
  p_evaluationImplementation_->setParameter(parameter);
  p_gradientImplementation_->setParameter(parameter);
  p_hessianImplementation_->setParameter(parameter);
}

/* Parameters description accessor */
Description FunctionImplementation::getParameterDescription() const
{
  return p_evaluationImplementation_->getParameterDescription();
}

void FunctionImplementation::setParameterDescription(const Description & description)
{
  p_evaluationImplementation_->setParameterDescription(description);
}

/* Operator () */
Point FunctionImplementation::operator() (const Point & inP) const
{
  return p_evaluationImplementation_->operator()(inP);
}

Point FunctionImplementation::operator() (const Point & inP,
    const Point & parameter)
{
  setParameter(parameter);
  return p_evaluationImplementation_->operator()(inP);
}

Sample FunctionImplementation::operator() (const Point & inP,
    const Sample & parameters)
{
  return p_evaluationImplementation_->operator()(inP, parameters);
}

/* Operator () */
Sample FunctionImplementation::operator() (const Sample & inSample) const
{
  return p_evaluationImplementation_->operator()(inSample);
}

/* Operator () */
Field FunctionImplementation::operator() (const Field & inField) const
{
  return p_evaluationImplementation_->operator()(inField);
}

/* Method gradient() returns the Jacobian transposed matrix of the function at point */
Matrix FunctionImplementation::gradient(const Point & inP) const
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
      const CenteredFiniteDifferenceGradient gradientFD(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), p_evaluationImplementation_);
      return gradientFD.gradient(inP);
    }
    catch (...)
    {
      throw InternalException(HERE) << "Error: cannot compute gradient at point=" << inP;
    }
  } // Usual gradient failed
}

Matrix FunctionImplementation::gradient(const Point & inP,
    const Point & parameters)
{
  if (useDefaultGradientImplementation_) LOGWARN(OSS() << "You are using a default implementation for the gradient. Be careful, your computation can be severely wrong!");
  setParameter(parameters);
  return p_gradientImplementation_->gradient(inP);
}

/* Method hessian() returns the symetric tensor of the function at point */
SymmetricTensor FunctionImplementation::hessian(const Point & inP) const
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
      const CenteredFiniteDifferenceHessian hessianFD(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), p_evaluationImplementation_);
      return hessianFD.hessian(inP);
    }
    catch (...)
    {
      throw InternalException(HERE) << "Error: cannot compute hessian at point=" << inP;
    }
  } // Usual gradient failed
}

SymmetricTensor FunctionImplementation::hessian(const Point & inP,
    const Point & parameters)
{
  if (useDefaultHessianImplementation_) LOGWARN(OSS() << "You are using a default implementation for the hessian. Be careful, your computation can be severely wrong!");
  setParameter(parameters);
  return p_hessianImplementation_->hessian(inP);
}

/* Accessor for parameter dimension */
UnsignedInteger FunctionImplementation::getParameterDimension() const
{
  return p_evaluationImplementation_->getParameterDimension();
}

/* Accessor for input point dimension */
UnsignedInteger FunctionImplementation::getInputDimension() const
{
  return p_evaluationImplementation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger FunctionImplementation::getOutputDimension() const
{
  return p_evaluationImplementation_->getOutputDimension();
}

/* Get the i-th marginal function */
FunctionImplementation::Implementation FunctionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
FunctionImplementation::Implementation FunctionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new FunctionImplementation(p_evaluationImplementation_->getMarginal(indices), p_gradientImplementation_->getMarginal(indices), p_hessianImplementation_->getMarginal(indices));
}

/* Number of calls to the evaluation */
UnsignedInteger FunctionImplementation::getEvaluationCallsNumber() const
{
  return p_evaluationImplementation_->getCallsNumber();
}

/* Number of calls to the gradient */
UnsignedInteger FunctionImplementation::getGradientCallsNumber() const
{
  return p_gradientImplementation_->getCallsNumber();
}

/* Number of calls to the hessian */
UnsignedInteger FunctionImplementation::getHessianCallsNumber() const
{
  return p_hessianImplementation_->getCallsNumber();
}

/* Static methods for documentation of analytical fonctions */
Description FunctionImplementation::GetValidConstants()
{
  Log::Warn(OSS() << "Function:GetValidConstants is deprecated");
  return SymbolicFunction::GetValidConstants();
}

Description FunctionImplementation::GetValidFunctions()
{
  Log::Warn(OSS() << "Function:GetValidFunctions is deprecated");
  return SymbolicFunction::GetValidFunctions();
}

Description FunctionImplementation::GetValidOperators()
{
  Log::Warn(OSS() << "Function:GetValidOperators is deprecated");
  return SymbolicFunction::GetValidOperators();
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph FunctionImplementation::draw(const UnsignedInteger inputMarginal,
    const UnsignedInteger outputMarginal,
    const Point & centralPoint,
    const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph FunctionImplementation::draw(const UnsignedInteger firstInputMarginal,
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
Graph FunctionImplementation::draw(const NumericalScalar xMin,
    const NumericalScalar xMax,
    const UnsignedInteger pointNumber,
    const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph FunctionImplementation::draw(const Point & xMin,
    const Point & xMax,
    const Indices & pointNumber,
    const GraphImplementation::LogScale scale) const
{
  return p_evaluationImplementation_->draw(xMin, xMax, pointNumber, scale);
}

/* Method save() stores the object through the StorageManager */
void FunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluationImplementation_", *p_evaluationImplementation_ );
  adv.saveAttribute( "gradientImplementation_", *p_gradientImplementation_ );
  adv.saveAttribute( "hessianImplementation_", *p_hessianImplementation_ );
  adv.saveAttribute( "useDefaultGradientImplementation_", useDefaultGradientImplementation_ );
  adv.saveAttribute( "useDefaultHessianImplementation_", useDefaultHessianImplementation_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionImplementation::load(Advocate & adv)
{
  TypedInterfaceObject<EvaluationImplementation> evaluationValue;
  TypedInterfaceObject<GradientImplementation> gradientValue;
  TypedInterfaceObject<HessianImplementation> hessianValue;
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
