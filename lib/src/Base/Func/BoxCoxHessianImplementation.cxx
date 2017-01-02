//                                               -*- C++ -*-
/**
 * @brief Class for a Box cox implementation
 *
 * Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/BoxCoxHessianImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BoxCoxHessianImplementation);

static const Factory<BoxCoxHessianImplementation> Factory_BoxCoxHessianImplementation;

/* Default constructor */
BoxCoxHessianImplementation::BoxCoxHessianImplementation()
  : NumericalMathHessianImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
BoxCoxHessianImplementation::BoxCoxHessianImplementation(const BoxCoxEvaluationImplementation & evaluation)
  : NumericalMathHessianImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Clone constructor */
BoxCoxHessianImplementation * BoxCoxHessianImplementation::clone() const
{
  return new BoxCoxHessianImplementation(*this);
}

/* Comparison operator */
Bool BoxCoxHessianImplementation::operator ==(const BoxCoxHessianImplementation & other) const
{
  if (this == &other) return true;
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String BoxCoxHessianImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BoxCoxHessianImplementation::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter __str__ */
String BoxCoxHessianImplementation::__str__(const String & offset) const
{
  OSS oss (false);
  oss << "BoxCoxHessian(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
}

/* Accessor for the evaluation */
BoxCoxEvaluationImplementation BoxCoxHessianImplementation::getEvaluation() const
{
  return evaluation_;
}

/* Hessian evaluation method */
SymmetricTensor BoxCoxHessianImplementation::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  SymmetricTensor result(1, dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the hessian is used in a stochastic context, in the BoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const NumericalScalar x = inP[index] + getShift()[index];
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox hessian function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const NumericalScalar lambda_i = getLambda()[index];
    const NumericalScalar logX = log(x);
    if (std::abs(lambda_i * logX) < 1e-8) result(0, 0, index) = -(1.0 + lambda_i * (logX - 1.0)) / (x * x);
    else result(0, 0, index) = (lambda_i - 1.0) * exp((lambda_i - 2.0) * logX);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger BoxCoxHessianImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger BoxCoxHessianImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for the lambda */
NumericalPoint BoxCoxHessianImplementation::getLambda() const
{
  return evaluation_.getLambda();
}

/* Accessor for the shift */
NumericalPoint BoxCoxHessianImplementation::getShift() const
{
  return evaluation_.getShift();
}

/* Method save() stores the object through the StorageManager */
void BoxCoxHessianImplementation::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void BoxCoxHessianImplementation::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
