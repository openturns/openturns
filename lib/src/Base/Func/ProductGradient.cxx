//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for the gradient of product functions
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ProductGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductGradient)

static const Factory<ProductGradient> Factory_ProductGradient;

/* Default constructor */
ProductGradient::ProductGradient(const Evaluation & leftEvaluation,
                                 const Gradient & leftGradient,
                                 const Evaluation & rightEvaluation,
                                 const Gradient & rightGradient)
  : GradientImplementation(),
    leftEvaluation_(leftEvaluation),
    leftGradient_(leftGradient),
    rightEvaluation_(rightEvaluation),
    rightGradient_(rightGradient)
{
  // Check the compatibility of the evaluations
  if (leftEvaluation_.getOutputDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the left function must have an output dimension equal to 1.";
  if (leftEvaluation_.getInputDimension() != rightEvaluation_.getInputDimension()) throw InvalidArgumentException(HERE) << "Error: the two functions must have the same input dimension.";

  // Check the compatibility of the gradients
  if ((leftGradient_.getInputDimension()  != rightGradient_.getInputDimension()) ||
      (leftGradient_.getInputDimension()  != leftEvaluation_.getInputDimension()) ||
      (leftGradient_.getOutputDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: the gradients have incompatible dimensions.";
}

/* Virtual constructor */
ProductGradient * ProductGradient::clone() const
{
  return new ProductGradient(*this);
}

/* Comparison operator */
Bool ProductGradient::operator ==(const ProductGradient & ) const
{
  return true;
}

/* String converter */
String ProductGradient::__repr__() const
{
  OSS oss;
  oss << "class=" << ProductGradient::GetClassName()
      << " name=" << getName()
      << " leftEvaluation=" << leftEvaluation_.getImplementation()->__repr__()
      << " leftGradient=" << leftGradient_.getImplementation()->__repr__()
      << " rightEvaluation=" << rightEvaluation_.getImplementation()->__repr__()
      << " rightGradient=" << rightGradient_.getImplementation()->__repr__();
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
  callsNumber_.increment();
  const Scalar leftValue = leftEvaluation_.operator()(inP)[0];
  const Point rightValue(rightEvaluation_.operator()(inP));
  const Matrix leftGradient(leftGradient_.gradient(inP));
  const Matrix rightGradient(rightGradient_.gradient(inP));
  return leftValue * rightGradient + leftGradient * Matrix(1, getOutputDimension(), rightValue);
}

/* Accessor for input point dimension */
UnsignedInteger ProductGradient::getInputDimension() const
{
  return rightEvaluation_.getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ProductGradient::getOutputDimension() const
{
  return rightEvaluation_.getOutputDimension();
}

/* Method save() stores the object through the StorageManager */
void ProductGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "leftEvaluation_", leftEvaluation_ );
  adv.saveAttribute( "leftGradient_", leftGradient_ );
  adv.saveAttribute( "rightEvaluation_", rightEvaluation_ );
  adv.saveAttribute( "rightGradient_", rightGradient_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "leftEvaluation_", leftEvaluation_ );
  adv.loadAttribute( "leftGradient_", leftGradient_ );
  adv.loadAttribute( "rightEvaluation_", rightEvaluation_ );
  adv.loadAttribute( "rightGradient_", rightGradient_ );
}

END_NAMESPACE_OPENTURNS
