//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ProductUniVariateFunctionGradient.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductUniVariateFunctionGradient)

static const Factory<ProductUniVariateFunctionGradient> Factory_ProductUniVariateFunctionGradient;


/* Default constructor */
ProductUniVariateFunctionGradient::ProductUniVariateFunctionGradient()
  : GradientImplementation()
{
  // Nothing to do
}


/* Constructor */
ProductUniVariateFunctionGradient::ProductUniVariateFunctionGradient(const Pointer<ProductUniVariateFunctionEvaluation> & p_evaluation)
  : GradientImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}


/* Virtual constructor */
ProductUniVariateFunctionGradient * ProductUniVariateFunctionGradient::clone() const
{
  return new ProductUniVariateFunctionGradient(*this);
}


/* String converter */
String ProductUniVariateFunctionGradient::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Compute the gradient of a product of univariate polynomials */
Matrix ProductUniVariateFunctionGradient::gradient (const Point & inP) const
{
  const UnsignedInteger inDimension = inP.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  Scalar productEvaluation = 1.0;
  Point evaluations(inDimension);
  Point derivatives(inDimension);
  for (UnsignedInteger i = 0; i < inDimension; ++i)
  {
    const Scalar x = inP[i];
    const Scalar y = p_evaluation_->functions_[i](x);
    const Scalar dy = p_evaluation_->functions_[i].gradient(x);
    evaluations[i] = y;
    derivatives[i] = dy;
    productEvaluation *= y;
  }
  Matrix grad(inDimension, 1);
  // Usual case: productEvaluation <> 0
  if (productEvaluation != 0.0)
  {
    for (UnsignedInteger i = 0; i < inDimension; ++i)
    {
      grad(i, 0) = derivatives[i] * (productEvaluation / evaluations[i]);
    }
  }
  // Must compute the gradient in a more expensive way
  else
  {
    for (UnsignedInteger i = 0; i < inDimension; ++i)
    {
      grad(i, 0) = derivatives[i];
      for (UnsignedInteger j = 0; j < i; ++j) grad(i, 0) *= evaluations[j];
      for (UnsignedInteger j = i + 1; j < inDimension; ++j) grad(i, 0) *= evaluations[j];
    }
  }
  return grad;
}


/* Accessor for input point dimension */
UnsignedInteger ProductUniVariateFunctionGradient::getInputDimension() const
{
  return p_evaluation_->functions_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductUniVariateFunctionGradient::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductUniVariateFunctionGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute("evaluation_", *p_evaluation_);
}

/* Method load() reloads the object from the StorageManager */
void ProductUniVariateFunctionGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  TypedInterfaceObject<ProductUniVariateFunctionEvaluation> evaluation;
  adv.loadAttribute("evaluation_", evaluation);
  p_evaluation_ = evaluation.getImplementation();
}


END_NAMESPACE_OPENTURNS
