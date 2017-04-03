//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the gradient of composed functions
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
#include "openturns/ComposedGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComposedGradient);

static const Factory<ComposedGradient> Factory_ComposedGradient;

/* Default constructor */
ComposedGradient::ComposedGradient(const GradientPointer & p_leftGradient,
                                   const EvaluationPointer & p_rightFunction,
                                   const GradientPointer & p_rightGradient)
  : GradientImplementation(),
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
ComposedGradient * ComposedGradient::clone() const
{
  return new ComposedGradient(*this);
}

/* Comparison operator */
Bool ComposedGradient::operator ==(const ComposedGradient & other) const
{
  return true;
}

/* String converter */
String ComposedGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << ComposedGradient::GetClassName()
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
Matrix ComposedGradient::gradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  return  p_rightGradient_->gradient(inP) * p_leftGradient_->gradient(p_rightFunction_->operator()(inP));
}

/* Accessor for input point dimension */
UnsignedInteger ComposedGradient::getInputDimension() const
{
  return p_rightGradient_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ComposedGradient::getOutputDimension() const
{
  return p_leftGradient_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ComposedGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "leftGradient_", *p_leftGradient_ );
  adv.saveAttribute( "rightFunction_", *p_rightFunction_ );
  adv.saveAttribute( "rightGradient_", *p_rightGradient_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedGradient::load(Advocate & adv)
{
  TypedInterfaceObject<EvaluationImplementation> evaluationValue;
  TypedInterfaceObject<GradientImplementation> gradientValue;
  GradientImplementation::load(adv);
  adv.loadAttribute( "leftGradient_", gradientValue );
  p_leftGradient_ = gradientValue.getImplementation();
  adv.loadAttribute( "rightFunction_", evaluationValue );
  p_rightFunction_ = evaluationValue.getImplementation();
  adv.loadAttribute( "rightGradient_", gradientValue );
  p_rightGradient_ = gradientValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
