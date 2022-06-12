//                                               -*- C++ -*-
/**
 *  @brief Class for a Box cox implementation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "openturns/InverseBoxCoxHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

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
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
InverseBoxCoxHessian::InverseBoxCoxHessian(const Pointer<InverseBoxCoxEvaluation> & p_evaluation)
  : HessianImplementation()
  , p_evaluation_(p_evaluation)
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
  return (*p_evaluation_ == *other.p_evaluation_);
}

/* String converter */
String InverseBoxCoxHessian::__repr__() const
{
  OSS oss(true);
  oss << "class=" << InverseBoxCoxHessian::GetClassName()
      << " name=" << getName()
      << " evaluation=" << *p_evaluation_;
  return oss;
}

/* String converter __str__ */
String InverseBoxCoxHessian::__str__(const String & ) const
{
  OSS oss (false);
  oss << "InverseBoxCoxHessian(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
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
    if (!(x > 0.0))
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
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger InverseBoxCoxHessian::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* Accessor for the lambda */
Point InverseBoxCoxHessian::getLambda() const
{
  return p_evaluation_->getLambda();
}

/* Accessor for the shift */
Point InverseBoxCoxHessian::getShift() const
{
  return p_evaluation_->getShift();
}

/* Method save() stores the object through the StorageManager */
void InverseBoxCoxHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void InverseBoxCoxHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  TypedInterfaceObject<InverseBoxCoxEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}

END_NAMESPACE_OPENTURNS
