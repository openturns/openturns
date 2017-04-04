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
#include "openturns/ProductHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductHessian);

static const Factory<ProductHessian> Factory_ProductHessian;

/* Default constructor */
ProductHessian::ProductHessian(const EvaluationPointer & p_leftEvaluation,
                               const GradientPointer & p_leftGradient,
                               const HessianPointer & p_leftHessian,
                               const EvaluationPointer & p_rightEvaluation,
                               const GradientPointer & p_rightGradient,
                               const HessianPointer & p_rightHessian)
  : HessianImplementation(),
    p_leftEvaluation_(p_leftEvaluation),
    p_leftGradient_(p_leftGradient),
    p_leftHessian_(p_leftHessian),
    p_rightEvaluation_(p_rightEvaluation),
    p_rightGradient_(p_rightGradient),
    p_rightHessian_(p_rightHessian)
{
  // Check the compatibility of the evaluations
  if (p_leftEvaluation_->getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the left function must have an output dimension equal to 1.";
  if (p_leftEvaluation_->getInputDimension() != p_rightEvaluation_->getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the two functions must have the same input dimension.";

  // Check the compatibility of the gradients
  if ((p_leftGradient_->getInputDimension()  != p_rightGradient_->getInputDimension()) ||
      (p_leftGradient_->getInputDimension()  != p_leftEvaluation_->getInputDimension()) ||
      (p_leftGradient_->getOutputDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the gradients have incompatible dimensions.";
  // Check the compatibility of the hessians
  if ((p_leftHessian_->getInputDimension()  != p_rightHessian_->getInputDimension()) ||
      (p_leftHessian_->getInputDimension()  != p_leftEvaluation_->getInputDimension()) ||
      (p_leftHessian_->getOutputDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the hessians have incompatible dimensions.";

}

/* Virtual constructor */
ProductHessian * ProductHessian::clone() const
{
  return new ProductHessian(*this);
}

/* Comparison operator */
Bool ProductHessian::operator ==(const ProductHessian & other) const
{
  return true;
}

/* String converter */
String ProductHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductHessian::GetClassName()
      << " name=" << getName()
      << " leftEvaluation=" << p_leftEvaluation_->__repr__()
      << " leftGradient=" << p_leftGradient_->__repr__()
      << " leftHessian=" << p_leftHessian_->__repr__()
      << " rightEvaluation=" << p_rightEvaluation_->__repr__()
      << " rightGradient=" << p_rightGradient_->__repr__()
      << " rightHessian=" << p_rightHessian_->__repr__();
  return oss;
}

/* Method hessian() returns the symmetric tensor of the second gradient of the function at point
 * h = f . g with f from R^n to R, g from R^n to R^p and then h from R^n to R^p
 * J(f) \in M_{1, n}, J(g) \in M_{p, n}, J(h) \in M_{p, n}
 * grad(f) \in M_{n, 1}, grad(g) \in M_{n, p}, grad(h) \in M_{n, p}
 * grad(h)_{i,j} = d(fg)_j / dx_i = f . dg_j / dx_i + df / dx_i . g_j
 * grad(h) = f . grad(g) + grad(f) . g^t
 * H(f) \in T_{1, n, n}, H(g) \in T_{p, n, n}, H(h) \in T_{p, n, n}
 * hessian(h)_{i,j,k} = d^2(fg)_k / (dx_i . dx_j) = f . d^2g_k / (dx_i . dx_j) + df / dx_j . dg_k / dx_i + d^2f / (dx_i . dx_j) + df / dx_i . dg_k / dx_j
 */
SymmetricTensor ProductHessian::hessian(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  const Point leftValue(p_leftEvaluation_->operator()(inP));
  const Point rightValue(p_rightEvaluation_->operator()(inP));
  const Matrix leftGradient(p_leftGradient_->gradient(inP));
  const Matrix rightGradient(p_rightGradient_->gradient(inP));
  const SymmetricTensor leftHessian(p_leftHessian_->hessian(inP));
  const SymmetricTensor rightHessian(p_rightHessian_->hessian(inP));
  const UnsignedInteger sheetDimension = getOutputDimension();
  SymmetricTensor result(inputDimension, sheetDimension);
  for (UnsignedInteger k = 0; k < sheetDimension; ++k)
  {
    const SymmetricMatrix leftHessianSheet(leftHessian.getSheet(0));
    const SymmetricMatrix rightHessianSheet(rightHessian.getSheet(k));
    const Scalar rightValueScalar = rightValue[k];
    const Scalar leftValueScalar = leftValue[0];
    const SymmetricMatrix term1(leftHessianSheet * rightValueScalar);
    const SymmetricMatrix term2(rightHessianSheet * leftValueScalar);
    const Matrix term3(leftGradient * rightGradient.transpose().getRow(k));
    const Matrix term4(rightGradient.getColumn(k) * leftGradient.transpose());
    const SymmetricMatrix sheet((term1 + term2 + term3 + term4).getImplementation());
    for (UnsignedInteger i = 0; i < inputDimension; ++i)
      for (UnsignedInteger j = i; j < inputDimension; ++j)
        result(i, j, k) = sheet(i, j);
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger ProductHessian::getInputDimension() const
{
  return p_rightHessian_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductHessian::getOutputDimension() const
{
  return p_rightHessian_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "leftGradient_", *p_leftGradient_ );
  adv.saveAttribute( "leftHessian_", *p_leftHessian_ );
  adv.saveAttribute( "rightEvaluation_", *p_rightEvaluation_ );
  adv.saveAttribute( "rightGradient_", *p_rightGradient_ );
  adv.saveAttribute( "rightHessian_", *p_rightHessian_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductHessian::load(Advocate & adv)
{
  TypedInterfaceObject<EvaluationImplementation> evaluationValue;
  TypedInterfaceObject<GradientImplementation> gradientValue;
  TypedInterfaceObject<HessianImplementation> hessianValue;
  HessianImplementation::load(adv);
  adv.loadAttribute( "leftGradient_", gradientValue );
  p_leftGradient_ = gradientValue.getImplementation();
  adv.loadAttribute( "leftHessian_", hessianValue );
  p_leftHessian_ = hessianValue.getImplementation();
  adv.loadAttribute( "rightEvaluation_", evaluationValue );
  p_rightEvaluation_ = evaluationValue.getImplementation();
  adv.loadAttribute( "rightGradient_", gradientValue );
  p_rightGradient_ = gradientValue.getImplementation();
  adv.loadAttribute( "rightHessian_", hessianValue );
  p_rightHessian_ = hessianValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
