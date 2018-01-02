//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the gradient of product functions
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ProductGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductGradient)

static const Factory<ProductGradient> Factory_ProductGradient;

/* Default constructor */
ProductGradient::ProductGradient(const EvaluationPointer & p_leftEvaluation,
                                 const GradientPointer & p_leftGradient,
                                 const EvaluationPointer & p_rightEvaluation,
                                 const GradientPointer & p_rightGradient)
  : GradientImplementation(),
    p_leftEvaluation_(p_leftEvaluation),
    p_leftGradient_(p_leftGradient),
    p_rightEvaluation_(p_rightEvaluation),
    p_rightGradient_(p_rightGradient)
{
  // Check the compatibility of the evaluations
  if (p_leftEvaluation_->getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the left function must have an output dimension equal to 1.";
  if (p_leftEvaluation_->getInputDimension() != p_rightEvaluation_->getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the two functions must have the same input dimension.";

  // Check the compatibility of the gradients
  if ((p_leftGradient_->getInputDimension()  != p_rightGradient_->getInputDimension()) ||
      (p_leftGradient_->getInputDimension()  != p_leftEvaluation_->getInputDimension()) ||
      (p_leftGradient_->getOutputDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the gradients have incompatible dimensions.";
}

/* Virtual constructor */
ProductGradient * ProductGradient::clone() const
{
  return new ProductGradient(*this);
}

/* Comparison operator */
Bool ProductGradient::operator ==(const ProductGradient & other) const
{
  return true;
}

/* String converter */
String ProductGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductGradient::GetClassName()
      << " name=" << getName()
      << " leftEvaluation=" << p_leftEvaluation_->__repr__()
      << " leftGradient=" << p_leftGradient_->__repr__()
      << " rightEvaluation=" << p_rightEvaluation_->__repr__()
      << " rightGradient=" << p_rightGradient_->__repr__();
  return oss;
}

/* Method gradient() returns the Jacobian transposed matrix of the function at point
 * h = f . g with f from R^n to R, g from R^n to R^p and then h from R^n to R^p
 * J(f) \in M_{1, n}, J(g) \in M_{p, n}, J(h) \in M_{p, n}
 * grad(f) \in M_{n, 1}, grad(g) \in M_{n, p}, grad(h) \in M_{n, p}
 * grad(h)_{i,j} = d(fg)_j / dx_i = f . dg_j / dx_i + df / dx_i . g_j
 * grad(h) = f . grad(g) + grad(f) . g^t
 */
Matrix ProductGradient::gradient(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  const Scalar leftValue = p_leftEvaluation_->operator()(inP)[0];
  const Point rightValue(p_rightEvaluation_->operator()(inP));
  const Matrix leftGradient(p_leftGradient_->gradient(inP));
  const Matrix rightGradient(p_rightGradient_->gradient(inP));
  return leftValue * rightGradient + leftGradient * Matrix(1, getOutputDimension(), rightValue);
}

/* Accessor for input point dimension */
UnsignedInteger ProductGradient::getInputDimension() const
{
  return p_rightEvaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductGradient::getOutputDimension() const
{
  return p_rightEvaluation_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "leftEvaluation_", *p_leftEvaluation_ );
  adv.saveAttribute( "leftGradient_", *p_leftGradient_ );
  adv.saveAttribute( "rightEvaluation_", *p_rightEvaluation_ );
  adv.saveAttribute( "rightGradient_", *p_rightGradient_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductGradient::load(Advocate & adv)
{
  TypedInterfaceObject<EvaluationImplementation> evaluationValue;
  TypedInterfaceObject<GradientImplementation> gradientValue;
  GradientImplementation::load(adv);
  adv.loadAttribute( "leftEvaluation_", evaluationValue );
  p_leftEvaluation_ = evaluationValue.getImplementation();
  adv.loadAttribute( "leftGradient_", gradientValue );
  p_leftGradient_ = gradientValue.getImplementation();
  adv.loadAttribute( "rightEvaluation_", evaluationValue );
  p_rightEvaluation_ = evaluationValue.getImplementation();
  adv.loadAttribute( "rightGradient_", gradientValue );
  p_rightGradient_ = gradientValue.getImplementation();
}

END_NAMESPACE_OPENTURNS
