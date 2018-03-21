//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all function implementations
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
#include "openturns/FunctionImplementation.hxx"
#include "openturns/NoEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
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

CLASSNAMEINIT(FunctionImplementation)

static const Factory<FunctionImplementation> Factory_FunctionImplementation;

/* Default constructor */
FunctionImplementation::FunctionImplementation()
  : PersistentObject()
  , evaluation_(new NoEvaluation)
  , gradient_(new NoGradient)
  , hessian_(new NoHessian)
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
  , evaluation_(new NoEvaluation)
  , gradient_(new NoGradient)
  , hessian_(new NoHessian)
  , useDefaultGradientImplementation_(true)
  , useDefaultHessianImplementation_(true)
{
#ifdef OPENTURNS_HAVE_ANALYTICAL_PARSER
  // Try to build an analytical gradient
  Pointer<SymbolicEvaluation> symbolicEvaluation(new SymbolicEvaluation(inputVariablesNames, outputVariablesNames, formulas));
  evaluation_ = symbolicEvaluation.get();
  try
  {
    gradient_ = new SymbolicGradient(symbolicEvaluation);
    useDefaultGradientImplementation_ = false;
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical gradient, using finite differences instead.");
    gradient_ = new CenteredFiniteDifferenceGradient(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), evaluation_);
  }
  try
  {
    hessian_ = new SymbolicHessian(symbolicEvaluation);
    useDefaultHessianImplementation_ = false;
  }
  catch(...)
  {
    LOGWARN("Cannot compute an analytical hessian, using finite differences instead.");
    hessian_ = new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), evaluation_);
  }
#else
  throw NotYetImplementedException(HERE) << "In FunctionImplementation::FunctionImplementation(const Description & inputVariablesNames, const Description & outputVariablesNames, const Description & formulas): Analytical function requires muParser or ExprTk";
#endif
}


/* Constructor from a wrapper file */
FunctionImplementation::FunctionImplementation(const Sample & inputSample,
    const Sample & outputSample)
  : PersistentObject()
  , gradient_(new NoGradient)
  , hessian_(new NoHessian)
  , useDefaultGradientImplementation_(false)
  , useDefaultHessianImplementation_(false)
{
  evaluation_ = new DatabaseEvaluation( inputSample, outputSample );
}


/* Single function implementation constructor */
FunctionImplementation::FunctionImplementation(const Evaluation & evaluation)
  : PersistentObject()
  , evaluation_(evaluation)
  , gradient_(new CenteredFiniteDifferenceGradient(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), evaluation_))
  , hessian_(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), evaluation_))
  , useDefaultGradientImplementation_(true)
  , useDefaultHessianImplementation_(true)
{
  // Nothing to do
}

/* Constructor from implementations */
FunctionImplementation::FunctionImplementation(const Evaluation & evaluation,
    const Gradient & gradient,
    const Hessian  & hessian)
  : PersistentObject()
  , evaluation_(evaluation)
  , gradient_(gradient)
  , hessian_(hessian)
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
  return (evaluation_ == other.evaluation_) && (gradient_ == other.gradient_) && (hessian_ == other.hessian_);
}

/* String converter */
String FunctionImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << FunctionImplementation::GetClassName()
      << " name=" << getName()
      << " description=" << getDescription()
      << " evaluationImplementation=" << evaluation_.getImplementation()->__repr__()
      << " gradientImplementation=" << gradient_.getImplementation()->__repr__()
      << " hessianImplementation=" << hessian_.getImplementation()->__repr__();
  return oss;
}

/* String converter */
String FunctionImplementation::__str__(const String & offset) const
{
  return evaluation_.__str__(offset);
}

/* Description Accessor */
void FunctionImplementation::setDescription(const Description & description)
{
  evaluation_.setDescription(description);
}

/* Description Accessor */
Description FunctionImplementation::getDescription() const
{
  return evaluation_.getDescription();
}

/* Input description Accessor */
Description FunctionImplementation::getInputDescription() const
{
  return evaluation_.getInputDescription();
}

/* Output description Accessor */
Description FunctionImplementation::getOutputDescription() const
{
  return evaluation_.getOutputDescription();
}

/* Enable or disable the internal cache */
void FunctionImplementation::enableCache() const
{
  evaluation_.enableCache();
}

void FunctionImplementation::disableCache() const
{
  evaluation_.disableCache();
}

Bool FunctionImplementation::isCacheEnabled() const
{
  return evaluation_.isCacheEnabled();
}

UnsignedInteger FunctionImplementation::getCacheHits() const
{
  return evaluation_.getCacheHits();
}

void FunctionImplementation::addCacheContent(const Sample& inSample,
    const Sample& outSample)
{
  evaluation_.addCacheContent(inSample, outSample);
}

Sample FunctionImplementation::getCacheInput() const
{
  return evaluation_.getCacheInput();
}

Sample FunctionImplementation::getCacheOutput() const
{
  return evaluation_.getCacheOutput();
}

void FunctionImplementation::clearCache() const
{
  evaluation_.clearCache();
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
void FunctionImplementation::setEvaluation(const Evaluation & evaluation)
{
  evaluation_ = evaluation;
}

Evaluation FunctionImplementation::getEvaluation() const
{
  return evaluation_;
}

/* Gradient implementation accessors */
void FunctionImplementation::setGradient(const Gradient & gradient)
{
  gradient_ = gradient;
  useDefaultGradientImplementation_ = false;
}

Gradient FunctionImplementation::getGradient() const
{
  return gradient_;
}

/* Hessian implementation accessors */
void FunctionImplementation::setHessian(const Hessian & hessian)
{
  hessian_ = hessian;
  useDefaultHessianImplementation_ = false;
}

Hessian FunctionImplementation::getHessian() const
{
  return hessian_;
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
  return evaluation_.parameterGradient(inP);
}

/* Gradient according to the marginal parameters */
Matrix FunctionImplementation::parameterGradient(const Point & inP,
    const Point & parameter)
{
  LOGWARN("FunctionImplementation::parameterGradient(inP,parameter) is deprecated, use setParameter(parameter) and parameterGradient(inP)");
  setParameter(parameter);
  return evaluation_.parameterGradient(inP);
}

/* Parameters value accessor */
Point FunctionImplementation::getParameter() const
{
  return evaluation_.getParameter();
}

void FunctionImplementation::setParameter(const Point & parameter)
{
  evaluation_.setParameter(parameter);
  gradient_.setParameter(parameter);
  hessian_.setParameter(parameter);
}

/* Parameters description accessor */
Description FunctionImplementation::getParameterDescription() const
{
  return evaluation_.getParameterDescription();
}

void FunctionImplementation::setParameterDescription(const Description & description)
{
  evaluation_.setParameterDescription(description);
}

/* Operator () */
Point FunctionImplementation::operator() (const Point & inP) const
{
  return evaluation_.operator()(inP);
}

Point FunctionImplementation::operator() (const Point & inP,
    const Point & parameter)
{
  LOGWARN("FunctionImplementation::operator()(inP,parameter) is deprecated, use setParameter(parameter) and operator()(inP)");
  setParameter(parameter);
  return evaluation_.operator()(inP);
}

Sample FunctionImplementation::operator() (const Point & inP,
    const Sample & parameters)
{
  // Deprecated, but already issues a LOGWARN
  return evaluation_.operator()(inP, parameters);
}

/* Operator () */
Sample FunctionImplementation::operator() (const Sample & inSample) const
{
  return evaluation_.operator()(inSample);
}

/* Operator () */
Field FunctionImplementation::operator() (const Field & inField) const
{
  return evaluation_.operator()(inField);
}

/* Method gradient() returns the Jacobian transposed matrix of the function at point */
Matrix FunctionImplementation::gradient(const Point & inP) const
{
  if (useDefaultGradientImplementation_) LOGWARN(OSS() << "You are using a default implementation for the gradient. Be careful, your computation can be severely wrong!");
  // Here we must catch the exceptions raised by functions with no gradient
  try
  {
    return gradient_.gradient(inP);
  }
  catch (...)
  {
    // Fallback on non-centered finite difference gradient
    try
    {
      LOGWARN(OSS() << "Switch to finite difference to compute the gradient at point=" << inP);
      const CenteredFiniteDifferenceGradient gradientFD(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), evaluation_);
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
  LOGWARN("FunctionImplementation::gradient()(inP,parameters) is deprecated, use setParameter(parameters) and gradient(inP)");
  setParameter(parameters);
  return gradient_.gradient(inP);
}

/* Method hessian() returns the symetric tensor of the function at point */
SymmetricTensor FunctionImplementation::hessian(const Point & inP) const
{
  if (useDefaultHessianImplementation_) LOGWARN(OSS() << "You are using a default implementation for the hessian. Be careful, your computation can be severely wrong!");
  // Here we must catch the exceptions raised by functions with no gradient
  try
  {
    return hessian_.hessian(inP);
  }
  catch (...)
  {
    // Fallback on non-centered finite difference gradient
    try
    {
      LOGWARN(OSS() << "Switch to finite difference to compute the hessian at point=" << inP);
      const CenteredFiniteDifferenceHessian hessianFD(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), evaluation_);
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
  LOGWARN("FunctionImplementation::hessian()(inP,parameters) is deprecated, use setParameter(parameters) and hessian(inP)");
  setParameter(parameters);
  return hessian_.hessian(inP);
}

/* Accessor for parameter dimension */
UnsignedInteger FunctionImplementation::getParameterDimension() const
{
  return evaluation_.getParameterDimension();
}

/* Accessor for input point dimension */
UnsignedInteger FunctionImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger FunctionImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Get the i-th marginal function */
Function FunctionImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the index of a marginal function must be in the range [0, outputDimension-1]";
  return getMarginal(Indices(1, i));
}

/* Get the function corresponding to indices components */
Function FunctionImplementation::getMarginal(const Indices & indices) const
{
  if (!indices.check(getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the indices of a marginal function must be in the range [0, outputDimension-1] and must be different";
  return new FunctionImplementation(evaluation_.getMarginal(indices), gradient_.getMarginal(indices), hessian_.getMarginal(indices));
}

/* Number of calls to the evaluation */
UnsignedInteger FunctionImplementation::getEvaluationCallsNumber() const
{
  return getCallsNumber();
}

UnsignedInteger FunctionImplementation::getCallsNumber() const
{
  return evaluation_.getCallsNumber();
}

/* Number of calls to the gradient */
UnsignedInteger FunctionImplementation::getGradientCallsNumber() const
{
  return gradient_.getCallsNumber();
}

/* Number of calls to the hessian */
UnsignedInteger FunctionImplementation::getHessianCallsNumber() const
{
  return hessian_.getCallsNumber();
}

/* Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
Graph FunctionImplementation::draw(const UnsignedInteger inputMarginal,
                                   const UnsignedInteger outputMarginal,
                                   const Point & centralPoint,
                                   const Scalar xMin,
                                   const Scalar xMax,
                                   const UnsignedInteger pointNumber,
                                   const GraphImplementation::LogScale scale) const
{
  return evaluation_.draw(inputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
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
  return evaluation_.draw(firstInputMarginal, secondInputMarginal, outputMarginal, centralPoint, xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input and output dimensions are 1 */
Graph FunctionImplementation::draw(const Scalar xMin,
                                   const Scalar xMax,
                                   const UnsignedInteger pointNumber,
                                   const GraphImplementation::LogScale scale) const
{
  return evaluation_.draw(xMin, xMax, pointNumber, scale);
}

/* Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
Graph FunctionImplementation::draw(const Point & xMin,
                                   const Point & xMax,
                                   const Indices & pointNumber,
                                   const GraphImplementation::LogScale scale) const
{
  return evaluation_.draw(xMin, xMax, pointNumber, scale);
}

/* Method save() stores the object through the StorageManager */
void FunctionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
  adv.saveAttribute( "gradient_", gradient_ );
  adv.saveAttribute( "hessian_", hessian_ );
  adv.saveAttribute( "useDefaultGradientImplementation_", useDefaultGradientImplementation_ );
  adv.saveAttribute( "useDefaultHessianImplementation_", useDefaultHessianImplementation_ );
}

/* Method load() reloads the object from the StorageManager */
void FunctionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
  adv.loadAttribute( "gradient_", gradient_ );
  adv.loadAttribute( "hessian_", hessian_ );
  adv.loadAttribute( "useDefaultGradientImplementation_", useDefaultGradientImplementation_ );
  adv.loadAttribute( "useDefaultHessianImplementation_", useDefaultHessianImplementation_ );
}




END_NAMESPACE_OPENTURNS
