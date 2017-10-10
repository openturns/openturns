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

#include "openturns/InverseBoxCoxHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(InverseBoxCoxHessian)

static const Factory<InverseBoxCoxHessian> Factory_InverseBoxCoxHessian;

/* Default constructor */
InverseBoxCoxHessian::InverseBoxCoxHessian()
  : HessianImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
InverseBoxCoxHessian::InverseBoxCoxHessian(const InverseBoxCoxEvaluation & evaluation)
  : HessianImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Clone constructor */
InverseBoxCoxHessian * InverseBoxCoxHessian::clone() const
{
  return new InverseBoxCoxHessian(*this);
}

/* Comparison operator */
Bool InverseBoxCoxHessian::operator ==(const InverseBoxCoxHessian & other) const
{
  if (this == &other) return true;
  return (evaluation_ == other.evaluation_);
}

/* String converter */
String InverseBoxCoxHessian::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseBoxCoxHessian::GetClassName()
      << " name=" << getName()
      << " evaluation=" << evaluation_;
  return oss;
}

/* String converter __str__ */
String InverseBoxCoxHessian::__str__(const String & offset) const
{
  OSS oss (false);
  oss << "InverseBoxCoxHessian(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
}

/* Accessor for the evaluation */
InverseBoxCoxEvaluation InverseBoxCoxHessian::getEvaluation() const
{
  return evaluation_;
}

/* Hessian evaluation method */
SymmetricTensor InverseBoxCoxHessian::hessian(const Point & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  SymmetricTensor result(1, dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the hessian is used in a stochastic context, in the InverseBoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const Scalar x = inP[index] + getShift()[index];
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox hessian function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const Scalar lambda_i = getLambda()[index];
    if (std::abs(lambda_i * x * x) < 1e-8) result(0, 0, index) = exp(x) * (1.0 - lambda_i * (1.0 + x * (2.0 + 0.5 * x)));
    else result(0, 0, index) = (1.0 - lambda_i) * pow(x, 1.0 / lambda_i - 2.0);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger InverseBoxCoxHessian::getInputDimension() const
{
  return evaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseBoxCoxHessian::getOutputDimension() const
{
  return evaluation_.getOutputDimension();
}

/* Accessor for the lambda */
Point InverseBoxCoxHessian::getLambda() const
{
  return evaluation_.getLambda();
}

/* Accessor for the shift */
Point InverseBoxCoxHessian::getShift() const
{
  return evaluation_.getShift();
}

/* Method save() stores the object through the StorageManager */
void InverseBoxCoxHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseBoxCoxHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
