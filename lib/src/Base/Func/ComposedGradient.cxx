//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the gradient of composed functions
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ComposedGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComposedGradient)

static const Factory<ComposedGradient> Factory_ComposedGradient;

/* Default constructor */
ComposedGradient::ComposedGradient(const Gradient & leftGradient,
                                   const Evaluation & rightFunction,
                                   const Gradient & rightGradient)
  : GradientImplementation(),
    leftGradient_(leftGradient),
    rightFunction_(rightFunction),
    rightGradient_(rightGradient)
{
  // We check that the dimensions of the input parameters are compatible
  // First, check the compatibility between the right function and the right gradient
  if ((rightFunction.getInputDimension() != rightGradient.getInputDimension()) || (rightFunction.getOutputDimension() != rightGradient.getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the right function and the right gradient have incompatible input or output dimensions.";
  // Second, check the left gradient and the right function
  if (leftGradient.getInputDimension() != rightFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the left gradient and the right function have incompatible input or output dimensions.";
}

/* Virtual constructor */
ComposedGradient * ComposedGradient::clone() const
{
  return new ComposedGradient(*this);
}

/* Comparison operator */
Bool ComposedGradient::operator ==(const ComposedGradient & ) const
{
  return true;
}

/* String converter */
String ComposedGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << ComposedGradient::GetClassName()
      << " name=" << getName()
      << " leftGradient=" << leftGradient_.getImplementation()->__repr__()
      << " rightFunction=" << rightFunction_.getImplementation()->__repr__()
      << " rightGradient=" << rightGradient_.getImplementation()->__repr__();
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
  callsNumber_.increment();
  return  rightGradient_.gradient(inP) * leftGradient_.gradient(rightFunction_.operator()(inP));
}

/* Accessor for input point dimension */
UnsignedInteger ComposedGradient::getInputDimension() const
{
  return rightGradient_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ComposedGradient::getOutputDimension() const
{
  return leftGradient_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ComposedGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "leftGradient_", leftGradient_ );
  adv.saveAttribute( "rightFunction_", rightFunction_ );
  adv.saveAttribute( "rightGradient_", rightGradient_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "leftGradient_", leftGradient_ );
  adv.loadAttribute( "rightFunction_", rightFunction_ );
  adv.loadAttribute( "rightGradient_", rightGradient_ );
}

END_NAMESPACE_OPENTURNS
