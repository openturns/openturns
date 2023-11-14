//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math hessians
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ComposedHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComposedHessian)

static const Factory<ComposedHessian> Factory_ComposedHessian;

/* Default constructor */
ComposedHessian::ComposedHessian(const Gradient & leftGradient,
                                 const Hessian & leftHessian,
                                 const Evaluation & rightFunction,
                                 const Gradient & rightGradient,
                                 const Hessian & rightHessian)
  : HessianImplementation(),
    leftGradient_(leftGradient),
    leftHessian_(leftHessian),
    rightFunction_(rightFunction),
    rightGradient_(rightGradient),
    rightHessian_(rightHessian)
{
  // We check that the dimensions of the input parameters are compatible
  // First, check the compatibility between the right function, the right gradient and the right hessian
  if ((rightFunction.getInputDimension() != rightGradient.getInputDimension()) || (rightFunction.getInputDimension() != rightHessian.getInputDimension()) || (rightFunction.getOutputDimension() != rightGradient.getOutputDimension()) || (rightFunction.getOutputDimension() != rightHessian.getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the right function, the right gradient and the right hessian have incompatible input or output dimensions.";
  // Second, check the compatibility between the left gradient and the left hessian
  if ((leftGradient.getInputDimension() != leftHessian.getInputDimension()) || (leftGradient.getOutputDimension() != leftHessian.getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the left gradient and the left hessian have incompatible input or output dimensions.";
  // Third, check the compatibility between the right output dimension and the left input dimension
  if (leftGradient.getInputDimension() != rightFunction.getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the left gradient and the right function have incompatible input or output dimensions.";
}

/* Virtual constructor */
ComposedHessian * ComposedHessian::clone() const
{
  return new ComposedHessian(*this);
}

/* Comparison operator */
Bool ComposedHessian::operator ==(const ComposedHessian & ) const
{
  return true;
}

/* String converter */
String ComposedHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << ComposedHessian::GetClassName()
      << " name=" << getName()
      << " leftGradient=" << leftGradient_.getImplementation()->__repr__()
      << " leftHessian=" << leftHessian_.getImplementation()->__repr__()
      << " rightFunction=" << rightFunction_.getImplementation()->__repr__()
      << " rightGradient=" << rightGradient_.getImplementation()->__repr__()
      << " rightHessian=" << rightHessian_.getImplementation()->__repr__();
  return oss;
}

/* Method hessian() returns the symmetric tensor of the second gradient of the function at point */
SymmetricTensor ComposedHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  callsNumber_.increment();
  // Hessian of the composed function fog, g:R^n->R^p, f:R^p->R^q so fog:R^n->R^q
  const Point rightIn(rightFunction_.operator()(inP));
  const Matrix rightGradientIn(rightGradient_.gradient(inP));
  const SymmetricTensor rightHessianIn(rightHessian_.hessian(inP));
  const Matrix leftGradientIn(leftGradient_.gradient(rightIn));
  const SymmetricTensor leftHessianIn(leftHessian_.hessian(rightIn));
  // Get the several dimensions
  const UnsignedInteger outputDimension = getOutputDimension();
  const UnsignedInteger innerDimension = rightIn.getDimension();
  SymmetricTensor result(inputDimension, outputDimension);
  /* We unroll the formula:
   *  D2(h)(x) = D2(f)(g(x))D(g)(x)D(g)(x)+D(f)(g(x))D2(g)(x)
   *  after transposition to convert jacobian and hessian into
   *  the associated gradients
   */
  for (UnsignedInteger i = 0; i < inputDimension; ++i)
  {
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      for (UnsignedInteger k = 0; k < outputDimension; ++k)
      {
        result(i, j, k) = 0.0;
        for (UnsignedInteger m = 0; m < innerDimension; ++m)
        {
          result(i, j, k) += leftGradientIn(m, k) * rightHessianIn(i, j, m);
          for (UnsignedInteger n = 0; n < rightIn.getDimension(); ++n)
          {
            result(i, j, k) += leftHessianIn(m, n, k) * rightGradientIn(i, m) * rightGradientIn(j, n);
          } // n
        } // m
      } // k
    } // j
  } // i
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger ComposedHessian::getInputDimension() const
{
  return rightHessian_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ComposedHessian::getOutputDimension() const
{
  return leftHessian_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ComposedHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "leftGradient_", leftGradient_ );
  adv.saveAttribute( "leftHessian_", leftHessian_ );
  adv.saveAttribute( "rightFunction_", rightFunction_ );
  adv.saveAttribute( "rightGradient_", rightGradient_ );
  adv.saveAttribute( "rightHessian_", rightHessian_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  adv.loadAttribute( "leftGradient_", leftGradient_ );
  adv.loadAttribute( "leftHessian_", leftHessian_ );
  adv.loadAttribute( "rightFunction_", rightFunction_ );
  adv.loadAttribute( "rightGradient_", rightGradient_ );
  adv.loadAttribute( "rightHessian_", rightHessian_ );
}

END_NAMESPACE_OPENTURNS
