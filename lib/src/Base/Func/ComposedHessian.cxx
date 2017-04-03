//                                               -*- C++ -*-
/**
 *  @brief The class that implements the composition between numerical math hessians
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
#include "openturns/ComposedHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ComposedHessian);

static const Factory<ComposedHessian> Factory_ComposedHessian;

/* Default constructor */
ComposedHessian::ComposedHessian(const GradientPointer & p_leftGradient,
    const HessianPointer & p_leftHessian,
    const EvaluationPointer & p_rightFunction,
    const GradientPointer & p_rightGradient,
    const HessianPointer & p_rightHessian)
  : HessianImplementation(),
    p_leftGradient_(p_leftGradient),
    p_leftHessian_(p_leftHessian),
    p_rightFunction_(p_rightFunction),
    p_rightGradient_(p_rightGradient),
    p_rightHessian_(p_rightHessian)
{
  // We check that the dimensions of the input parameters are compatible
  // First, check the compatibility between the right function, the right gradient and the right hessian
  if ((p_rightFunction->getInputDimension() != p_rightGradient->getInputDimension()) || (p_rightFunction->getInputDimension() != p_rightHessian->getInputDimension()) || (p_rightFunction->getOutputDimension() != p_rightGradient->getOutputDimension()) || (p_rightFunction->getOutputDimension() != p_rightHessian->getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the right function, the right gradient and the right hessian have incompatible input or output dimensions.";
  // Second, check the compatibility between the left gradient and the left hessian
  if ((p_leftGradient->getInputDimension() != p_leftHessian->getInputDimension()) || (p_leftGradient->getOutputDimension() != p_leftHessian->getOutputDimension())) throw InvalidArgumentException(HERE) << "Error: the left gradient and the left hessian have incompatible input or output dimensions.";
  // Third, check the compatibility between the right output dimension and the left input dimension
  if (p_leftGradient->getInputDimension() != p_rightFunction->getOutputDimension()) throw InvalidArgumentException(HERE) << "Error: the left gradient and the right function have incompatible input or output dimensions.";
}

/* Virtual constructor */
ComposedHessian * ComposedHessian::clone() const
{
  return new ComposedHessian(*this);
}

/* Comparison operator */
Bool ComposedHessian::operator ==(const ComposedHessian & other) const
{
  return true;
}

/* String converter */
String ComposedHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << ComposedHessian::GetClassName()
      << " name=" << getName()
      << " leftGradient=" << p_leftGradient_->__repr__()
      << " leftHessian=" << p_leftHessian_->__repr__()
      << " rightFunction=" << p_rightFunction_->__repr__()
      << " rightGradient=" << p_rightGradient_->__repr__()
      << " rightHessian=" << p_rightHessian_->__repr__();
  return oss;
}

/* Method hessian() returns the symmetric tensor of the second gradient of the function at point */
SymmetricTensor ComposedHessian::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  // Hessian of the composed function fog, g:R^n->R^p, f:R^p->R^q so fog:R^n->R^q
  const NumericalPoint p_rightIn(p_rightFunction_->operator()(inP));
  const Matrix p_rightGradientIn(p_rightGradient_->gradient(inP));
  const SymmetricTensor p_rightHessianIn(p_rightHessian_->hessian(inP));
  const Matrix p_leftGradientIn(p_leftGradient_->gradient(p_rightIn));
  const SymmetricTensor p_leftHessianIn(p_leftHessian_->hessian(p_rightIn));
  // Get the several dimensions
  const UnsignedInteger outputDimension = getOutputDimension();
  const UnsignedInteger innerDimension = p_rightIn.getDimension();
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
          result(i, j, k) += p_leftGradientIn(m, k) * p_rightHessianIn(i, j, m);
          for (UnsignedInteger n = 0; n < p_rightIn.getDimension(); ++n)
          {
            result(i, j, k) += p_leftHessianIn(m, n, k) * p_rightGradientIn(i, m) * p_rightGradientIn(j, n);
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
  return p_rightHessian_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ComposedHessian::getOutputDimension() const
{
  return p_leftHessian_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ComposedHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "leftGradient_", *p_leftGradient_ );
  adv.saveAttribute( "leftHessian_", *p_leftHessian_ );
  adv.saveAttribute( "rightFunction_", *p_rightFunction_ );
  adv.saveAttribute( "rightGradient_", *p_rightGradient_ );
  adv.saveAttribute( "rightHessian_", *p_rightHessian_ );
}

/* Method load() reloads the object from the StorageManager */
void ComposedHessian::load(Advocate & adv)
{
  TypedInterfaceObject<EvaluationImplementation> evaluationValue;
  TypedInterfaceObject<GradientImplementation> gradientValue;
  TypedInterfaceObject<HessianImplementation> hessianValue;
  HessianImplementation::load(adv);
  adv.loadAttribute( "leftGradient_", gradientValue );
  p_leftGradient_ = gradientValue.getImplementation();
  adv.loadAttribute( "leftHessian_", hessianValue );
  p_leftHessian_ = hessianValue.getImplementation();
  adv.loadAttribute( "rightFunction_", evaluationValue );
  p_rightFunction_ = evaluationValue.getImplementation();
  adv.loadAttribute( "rightGradient_", gradientValue );
  p_rightGradient_ = gradientValue.getImplementation();
  adv.loadAttribute( "rightHessian_", hessianValue );
  p_rightHessian_ = hessianValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
