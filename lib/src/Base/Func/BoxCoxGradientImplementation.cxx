//                                               -*- C++ -*-
/**
 * @brief Class for a Box cox implementation
 *
 * Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#include "openturns/BoxCoxGradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BoxCoxGradientImplementation);

static const Factory<BoxCoxGradientImplementation> Factory_BoxCoxGradientImplementation;

/* Default constructor */
BoxCoxGradientImplementation::BoxCoxGradientImplementation()
  : NumericalMathGradientImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
BoxCoxGradientImplementation::BoxCoxGradientImplementation(const BoxCoxEvaluationImplementation & evaluation)
  : NumericalMathGradientImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Clone constructor */
BoxCoxGradientImplementation * BoxCoxGradientImplementation::clone() const
{
  return new BoxCoxGradientImplementation(*this);
}

/* Comparison operator */
Bool BoxCoxGradientImplementation::operator ==(const BoxCoxGradientImplementation & other) const
{
  if (this == &other) return true;
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String BoxCoxGradientImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BoxCoxGradientImplementation::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter __str__ */
String BoxCoxGradientImplementation::__str__(const String & offset) const
{
  OSS oss (false);
  oss << "BoxCoxGradient(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
}

/* Accessor for the evaluation */
BoxCoxEvaluationImplementation BoxCoxGradientImplementation::getEvaluation() const
{
  return evaluation_;
}

/* Gradient evaluation method */
Matrix BoxCoxGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension(getInputDimension());
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  Matrix result(1, dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the gradient is used in a stochastic context, in the BoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const NumericalScalar x(inP[index] + getShift()[index]);
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox gradient function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const NumericalScalar lambda_i(getLambda()[index]);
    const NumericalScalar logX(log(x));
    if (std::abs(lambda_i * logX) < 1e-8) result(0, index) = (1.0 + lambda_i * logX) / x;
    else result(0, index) = exp((lambda_i - 1.0) * logX);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger BoxCoxGradientImplementation::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger BoxCoxGradientImplementation::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for the lambda */
NumericalPoint BoxCoxGradientImplementation::getLambda() const
{
  return evaluation_.getLambda();
}

/* Accessor for the shift */
NumericalPoint BoxCoxGradientImplementation::getShift() const
{
  return evaluation_.getShift();
}

/* Method save() stores the object through the StorageManager */
void BoxCoxGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void BoxCoxGradientImplementation::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
