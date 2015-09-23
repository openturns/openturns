//                                               -*- C++ -*-
/**
 * @brief Class for a Box cox implementation
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "InverseBoxCoxHessianImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseBoxCoxHessianImplementation);

static const Factory<InverseBoxCoxHessianImplementation> RegisteredFactory;

/* Default constructor */
InverseBoxCoxHessianImplementation::InverseBoxCoxHessianImplementation()
  : NumericalMathHessianImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseBoxCoxHessianImplementation::InverseBoxCoxHessianImplementation(const InverseBoxCoxEvaluationImplementation & evaluation)
  : NumericalMathHessianImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Clone constructor */
InverseBoxCoxHessianImplementation * InverseBoxCoxHessianImplementation::clone() const
{
  return new InverseBoxCoxHessianImplementation(*this);
}

/* Comparison operator */
Bool InverseBoxCoxHessianImplementation::operator ==(const InverseBoxCoxHessianImplementation & other) const
{
  if (this == &other) return true;
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String InverseBoxCoxHessianImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseBoxCoxHessianImplementation::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter __str__ */
String InverseBoxCoxHessianImplementation::__str__(const String & offset) const
{
  OSS oss (false);
  oss << "InverseBoxCoxHessian(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
}

/* Accessor for the evaluation */
InverseBoxCoxEvaluationImplementation InverseBoxCoxHessianImplementation::getEvaluation() const
{
  return evaluation_;
}

/* Hessian evaluation method */
SymmetricTensor InverseBoxCoxHessianImplementation::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension(getInputDimension());
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  SymmetricTensor result(1, dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the hessian is used in a stochastic context, in the InverseBoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const NumericalScalar x(inP[index] + getShift()[index]);
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox hessian function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const NumericalScalar lambda_i(getLambda()[index]);
    if (std::abs(lambda_i * x * x) < 1e-8) result(0, 0, index) = exp(x) * (1.0 - lambda_i * (1.0 + x * (2.0 + 0.5 * x)));
    else result(0, 0, index) = (1.0 - lambda_i) * pow(x, 1.0 / lambda_i - 2.0);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseBoxCoxHessianImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseBoxCoxHessianImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for the lambda */
NumericalPoint InverseBoxCoxHessianImplementation::getLambda() const
{
  return evaluation_.getLambda();
}

/* Accessor for the shift */
NumericalPoint InverseBoxCoxHessianImplementation::getShift() const
{
  return evaluation_.getShift();
}

/* Method save() stores the object through the StorageManager */
void InverseBoxCoxHessianImplementation::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseBoxCoxHessianImplementation::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
