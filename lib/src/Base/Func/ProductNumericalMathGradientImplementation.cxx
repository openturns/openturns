//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the gradient of product functions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ProductNumericalMathGradientImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductNumericalMathGradientImplementation);

static const Factory<ProductNumericalMathGradientImplementation> Factory_ProductNumericalMathGradientImplementation;

/* Default constructor */
ProductNumericalMathGradientImplementation::ProductNumericalMathGradientImplementation(const EvaluationImplementation & p_leftEvaluation,
    const GradientImplementation & p_leftGradient,
    const EvaluationImplementation & p_rightEvaluation,
    const GradientImplementation & p_rightGradient)
  : NumericalMathGradientImplementation(),
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
ProductNumericalMathGradientImplementation * ProductNumericalMathGradientImplementation::clone() const
{
  return new ProductNumericalMathGradientImplementation(*this);
}

/* Comparison operator */
Bool ProductNumericalMathGradientImplementation::operator ==(const ProductNumericalMathGradientImplementation & other) const
{
  return true;
}

/* String converter */
String ProductNumericalMathGradientImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductNumericalMathGradientImplementation::GetClassName()
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
Matrix ProductNumericalMathGradientImplementation::gradient(const NumericalPoint & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
  ++callsNumber_;
  const NumericalScalar leftValue = p_leftEvaluation_->operator()(inP)[0];
  const NumericalPoint rightValue(p_rightEvaluation_->operator()(inP));
  const Matrix leftGradient(p_leftGradient_->gradient(inP));
  const Matrix rightGradient(p_rightGradient_->gradient(inP));
  return leftValue * rightGradient + leftGradient * Matrix(1, getOutputDimension(), rightValue);
}

/* Accessor for input point dimension */
UnsignedInteger ProductNumericalMathGradientImplementation::getInputDimension() const
{
  return p_rightEvaluation_->getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductNumericalMathGradientImplementation::getOutputDimension() const
{
  return p_rightEvaluation_->getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductNumericalMathGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute( "leftEvaluation_", *p_leftEvaluation_ );
  adv.saveAttribute( "leftGradient_", *p_leftGradient_ );
  adv.saveAttribute( "rightEvaluation_", *p_rightEvaluation_ );
  adv.saveAttribute( "rightGradient_", *p_rightGradient_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductNumericalMathGradientImplementation::load(Advocate & adv)
{
  TypedInterfaceObject<NumericalMathEvaluationImplementation> evaluationValue;
  TypedInterfaceObject<NumericalMathGradientImplementation> gradientValue;
  NumericalMathGradientImplementation::load(adv);
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
