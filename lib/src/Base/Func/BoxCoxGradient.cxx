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

#include "openturns/BoxCoxGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BoxCoxGradient);

static const Factory<BoxCoxGradient> Factory_BoxCoxGradient;

/* Default constructor */
BoxCoxGradient::BoxCoxGradient()
  : NumericalMathGradientImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
BoxCoxGradient::BoxCoxGradient(const BoxCoxEvaluation & evaluation)
  : NumericalMathGradientImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Clone constructor */
BoxCoxGradient * BoxCoxGradient::clone() const
{
  return new BoxCoxGradient(*this);
}

/* Comparison operator */
Bool BoxCoxGradient::operator ==(const BoxCoxGradient & other) const
{
  if (this == &other) return true;
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String BoxCoxGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BoxCoxGradient::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter __str__ */
String BoxCoxGradient::__str__(const String & offset) const
{
  OSS oss (false);
  oss << "BoxCoxGradient(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
}

/* Accessor for the evaluation */
BoxCoxEvaluation BoxCoxGradient::getEvaluation() const
{
  return evaluation_;
}

/* Gradient evaluation method */
Matrix BoxCoxGradient::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  Matrix result(1, dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the gradient is used in a stochastic context, in the BoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const NumericalScalar x = inP[index] + getShift()[index];
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox gradient function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const NumericalScalar lambda_i = getLambda()[index];
    const NumericalScalar logX = log(x);
    if (std::abs(lambda_i * logX) < 1e-8) result(0, index) = (1.0 + lambda_i * logX) / x;
    else result(0, index) = exp((lambda_i - 1.0) * logX);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger BoxCoxGradient::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger BoxCoxGradient::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for the lambda */
NumericalPoint BoxCoxGradient::getLambda() const
{
  return evaluation_.getLambda();
}

/* Accessor for the shift */
NumericalPoint BoxCoxGradient::getShift() const
{
  return evaluation_.getShift();
}

/* Method save() stores the object through the StorageManager */
void BoxCoxGradient::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void BoxCoxGradient::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
