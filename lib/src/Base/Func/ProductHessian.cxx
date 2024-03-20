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
#include "openturns/ProductHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductHessian)

static const Factory<ProductHessian> Factory_ProductHessian;

/* Default constructor */
ProductHessian::ProductHessian(const Evaluation & leftEvaluation,
                               const Gradient & leftGradient,
                               const Hessian & leftHessian,
                               const Evaluation & rightEvaluation,
                               const Gradient & rightGradient,
                               const Hessian & rightHessian)
  : HessianImplementation(),
    leftEvaluation_(leftEvaluation),
    leftGradient_(leftGradient),
    leftHessian_(leftHessian),
    rightEvaluation_(rightEvaluation),
    rightGradient_(rightGradient),
    rightHessian_(rightHessian)
{
  // Check the compatibility of the evaluations
  if (leftEvaluation_.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the left function must have an output dimension equal to 1, not " << leftEvaluation_.getOutputDimension();
  if (leftEvaluation_.getInputDimension() != rightEvaluation_.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the two functions must have the same input dimension, but left=" << leftEvaluation_.getInputDimension() << " and right=" << rightEvaluation_.getInputDimension();

  // Check the compatibility of the gradients
  if ((leftGradient_.getInputDimension()  != rightGradient_.getInputDimension()) ||
      (leftGradient_.getInputDimension()  != leftEvaluation_.getInputDimension()) ||
      (leftGradient_.getOutputDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the gradients have incompatible dimensions.";
  // Check the compatibility of the hessians
  if ((leftHessian_.getInputDimension()  != rightHessian_.getInputDimension()) ||
      (leftHessian_.getInputDimension()  != leftEvaluation_.getInputDimension()) ||
      (leftHessian_.getOutputDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the hessians have incompatible dimensions.";

}

/* Virtual constructor */
ProductHessian * ProductHessian::clone() const
{
  return new ProductHessian(*this);
}

/* Comparison operator */
Bool ProductHessian::operator ==(const ProductHessian & other) const
{
  if (this == &other) return true;
  return leftEvaluation_ == other.leftEvaluation_ && leftGradient_ == other.leftGradient_ && leftHessian_ == other.leftHessian_ &&
         rightEvaluation_ == other.rightEvaluation_ && rightGradient_ == other.rightGradient_ && rightHessian_ == other.rightHessian_;
}

/* String converter */
String ProductHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductHessian::GetClassName()
      << " name=" << getName()
      << " leftEvaluation=" << leftEvaluation_.getImplementation()->__repr__()
      << " leftGradient=" << leftGradient_.getImplementation()->__repr__()
      << " leftHessian=" << leftHessian_.getImplementation()->__repr__()
      << " rightEvaluation=" << rightEvaluation_.getImplementation()->__repr__()
      << " rightGradient=" << rightGradient_.getImplementation()->__repr__()
      << " rightHessian=" << rightHessian_.getImplementation()->__repr__();
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
  callsNumber_.increment();
  const Point leftValue(leftEvaluation_.operator()(inP));
  const Point rightValue(rightEvaluation_.operator()(inP));
  const Matrix leftGradient(leftGradient_.gradient(inP));
  const Matrix rightGradient(rightGradient_.gradient(inP));
  const SymmetricTensor leftHessian(leftHessian_.hessian(inP));
  const SymmetricTensor rightHessian(rightHessian_.hessian(inP));
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
  return rightHessian_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductHessian::getOutputDimension() const
{
  return rightHessian_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "leftEvaluation_", leftEvaluation_ );
  adv.saveAttribute( "leftGradient_", leftGradient_ );
  adv.saveAttribute( "leftHessian_", leftHessian_ );
  adv.saveAttribute( "rightEvaluation_", rightEvaluation_ );
  adv.saveAttribute( "rightGradient_", rightGradient_ );
  adv.saveAttribute( "rightHessian_", rightHessian_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductHessian::load(Advocate & adv)
{
  TypedInterfaceObject<HessianImplementation> hessianValue;
  HessianImplementation::load(adv);
  adv.loadAttribute( "leftEvaluation_", leftEvaluation_ );
  adv.loadAttribute( "leftGradient_", leftGradient_ );
  adv.loadAttribute( "leftHessian_", leftHessian_ );
  adv.loadAttribute( "rightEvaluation_", rightEvaluation_ );
  adv.loadAttribute( "rightGradient_", rightGradient_ );
  adv.loadAttribute( "rightHessian_", rightHessian_ );
}

END_NAMESPACE_OPENTURNS
