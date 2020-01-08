//                                               -*- C++ -*-
/**
 *  @brief Class for a Box cox implementation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/BoxCoxGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BoxCoxGradient)

static const Factory<BoxCoxGradient> Factory_BoxCoxGradient;

/* Default constructor */
BoxCoxGradient::BoxCoxGradient()
  : GradientImplementation()
  , p_evaluation_()
{
  // Nothing to do
}

/* Parameter constructor */
BoxCoxGradient::BoxCoxGradient(const BoxCoxEvaluation & evaluation)
  : GradientImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Parameters constructor */
BoxCoxGradient::BoxCoxGradient(const Pointer<BoxCoxEvaluation> & p_evaluation)
  : GradientImplementation()
  , p_evaluation_(p_evaluation)
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
  return (*p_evaluation_ == *other.p_evaluation_);
}

/* String converter */
String BoxCoxGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BoxCoxGradient::GetClassName()
      << " name=" << getName()
      << " evaluation=" << *p_evaluation_;
  return oss;
}

/* String converter __str__ */
String BoxCoxGradient::__str__(const String & ) const
{
  OSS oss (false);
  oss << "BoxCoxGradient(lambda=" << getLambda()
      << ", shift=" << getShift()
      << ")";
  return oss;
}

/* Gradient evaluation method */
Matrix BoxCoxGradient::gradient(const Point & inP) const
{
  const UnsignedInteger dimension = getInputDimension();
  if (inP.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << dimension << ", got " << inP.getDimension();
  Matrix result(1, dimension);

  // There is no check of positive variables
  // This last one must be done by user or, as the gradient is used in a stochastic context, in the BoxCoxTransform class
  for (UnsignedInteger index = 0; index < dimension; ++index)
  {
    const Scalar x = inP[index] + getShift()[index];
    if (x <= 0.0)
      throw InvalidArgumentException(HERE) << "Can not apply the Box Cox gradient function to a negative shifted value x=" << x;

    // Applying the Box-Cox function
    const Scalar lambda_i = getLambda()[index];
    const Scalar logX = log(x);
    if (std::abs(lambda_i * logX) < 1e-8) result(0, index) = (1.0 + lambda_i * logX) / x;
    else result(0, index) = exp((lambda_i - 1.0) * logX);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger BoxCoxGradient::getInputDimension() const
{
  return p_evaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger BoxCoxGradient::getOutputDimension() const
{
  return p_evaluation_->getOutputDimension();
}

/* Accessor for the lambda */
Point BoxCoxGradient::getLambda() const
{
  return p_evaluation_->getLambda();
}

/* Accessor for the shift */
Point BoxCoxGradient::getShift() const
{
  return p_evaluation_->getShift();
}

/* Method save() stores the object through the StorageManager */
void BoxCoxGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void BoxCoxGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  TypedInterfaceObject<BoxCoxEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}

END_NAMESPACE_OPENTURNS
