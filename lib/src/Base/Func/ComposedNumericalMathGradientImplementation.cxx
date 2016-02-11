//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all distributions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ComposedNumericalMathGradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComposedNumericalMathGradientImplementation);

static const Factory<ComposedNumericalMathGradientImplementation> RegisteredFactory;

/* Default constructor */
ComposedNumericalMathGradientImplementation::ComposedNumericalMathGradientImplementation(const GradientImplementation & p_leftGradient,
    const EvaluationImplementation & p_rightFunction,
    const GradientImplementation & p_rightGradient)
  : NumericalMathGradientImplementation(),
    p_leftGradient_(p_leftGradient),
    p_rightFunction_(p_rightFunction),
    p_rightGradient_(p_rightGradient)
{
  // We check that the dimensions of the input parameters are compatible
  // First, check the compatibility between the right function and the right gradient
  if ((p_rightFunction->getInputDimension() != p_rightGradient->getInputDimension()) || (p_rightFunction->getOutputDimension() != p_rightGradient->getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the right function and the right gradient have incompatible input or output dimensions.";
  // Second, check the left gradient and the right function
  if (p_leftGradient->getInputDimension() != p_rightFunction->getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the left gradient and the right function have incompatible input or output dimensions.";
}

/* Virtual constructor */
ComposedNumericalMathGradientImplementation * ComposedNumericalMathGradientImplementation::clone() const
{
  return new ComposedNumericalMathGradientImplementation(*this);
}

/* Comparison operator */
Bool ComposedNumericalMathGradientImplementation::operator ==(const ComposedNumericalMathGradientImplementation & other) const
{
  return true;
}

/* String converter */
String ComposedNumericalMathGradientImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << ComposedNumericalMathGradientImplementation::GetClassName()
      << " name=" << getName()
      << " leftGradient=" << p_leftGradient_->__repr__()
      << " rightFunction=" << p_rightFunction_->__repr__()
      << " rightGradient=" << p_rightGradient_->__repr__();
  return oss;
}

/* Method gradient() returns the Jacobian transposed matrix of the function at point
 * f goes from Rn to Rp and g from Rq to Rp, so h = f o g goes from Rq to Rn. Its
 * jacobian goes from Rq to L(Rq, Rn), thus its gradient goes from Rq to L(Rn, Rq):
 * it is an n by q matrix                                                            */
Matrix ComposedNumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension(getInputDimension());
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  return  p_rightGradient_->gradient(inP) * p_leftGradient_->gradient(p_rightFunction_->operator()(inP));
}

/* Accessor for input point dimension */
UnsignedInteger ComposedNumericalMathGradientImplementation::getInputDimension() const
{
  return p_rightGradient_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ComposedNumericalMathGradientImplementation::getOutputDimension() const
{
  return p_leftGradient_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ComposedNumericalMathGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute( "leftGradient_", *p_leftGradient_ );
  adv.saveAttribute( "rightFunction_", *p_rightFunction_ );
  adv.saveAttribute( "rightGradient_", *p_rightGradient_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedNumericalMathGradientImplementation::load(Advocate & adv)
{
  TypedInterfaceObject<NumericalMathEvaluationImplementation> evaluationValue;
  TypedInterfaceObject<NumericalMathGradientImplementation> gradientValue;
  NumericalMathGradientImplementation::load(adv);
  adv.loadAttribute( "leftGradient_", gradientValue );
  p_leftGradient_ = gradientValue.getImplementation();
  adv.loadAttribute( "rightFunction_", evaluationValue );
  p_rightFunction_ = evaluationValue.getImplementation();
  adv.loadAttribute( "rightGradient_", gradientValue );
  p_rightGradient_ = gradientValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
