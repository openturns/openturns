//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
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
#include "openturns/ProductUniVariateFunctionHessian.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductUniVariateFunctionHessian)

static const Factory<ProductUniVariateFunctionHessian> Factory_ProductUniVariateFunctionHessian;


/* Default constructor */
ProductUniVariateFunctionHessian::ProductUniVariateFunctionHessian()
  : HessianImplementation()
{
  // Nothing to do
}


/* Constructor */
ProductUniVariateFunctionHessian::ProductUniVariateFunctionHessian(const ProductUniVariateFunctionEvaluation & evaluation)
  : HessianImplementation()
  , p_evaluation_(evaluation.clone())
{
  // Nothing to do
}


/* Constructor */
ProductUniVariateFunctionHessian::ProductUniVariateFunctionHessian(const Pointer<ProductUniVariateFunctionEvaluation> & p_evaluation)
  : HessianImplementation()
  , p_evaluation_(p_evaluation)
{
  // Nothing to do
}


/* Virtual constructor */
ProductUniVariateFunctionHessian * ProductUniVariateFunctionHessian::clone() const
{
  return new ProductUniVariateFunctionHessian(*this);
}


/* String converter */
String ProductUniVariateFunctionHessian::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Compute the hessian of a product of univariate polynomials */
SymmetricTensor ProductUniVariateFunctionHessian::hessian (const Point & inP) const
{
  const UnsignedInteger inDimension = inP.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  Scalar productEvaluation = 1.0;
  Point evaluations(inDimension);
  Point derivatives(inDimension);
  Point secondDerivatives(inDimension);
  for (UnsignedInteger i = 0; i < inDimension; ++i)
  {
    const Scalar x = inP[i];
    const Scalar y = p_evaluation_->functions_[i](x);
    const Scalar dy = p_evaluation_->functions_[i].gradient(x);
    const Scalar d2y = p_evaluation_->functions_[i].hessian(x);
    evaluations[i] = y;
    derivatives[i] = dy;
    secondDerivatives[i] = d2y;
    productEvaluation *= y;
  }
  SymmetricTensor hess(inDimension, 1);
  // Usual case: productEvaluation <> 0
  if (productEvaluation != 0.0)
  {
    for (UnsignedInteger i = 0; i < inDimension; ++i)
    {
      const Scalar dyi = derivatives[i] * (productEvaluation / evaluations[i]);
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        hess(i, j, 0) = derivatives[j] * (dyi / evaluations[j]);
      }
      hess(i, i, 0) = secondDerivatives[i] * (productEvaluation / evaluations[i]);
    }
  }
  // Must compute the hessian in a more expensive way
  else
  {
    for (UnsignedInteger i = 0; i < inDimension; ++i)
    {
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        hess(i, j, 0) = derivatives[i] * derivatives[j];
        for (UnsignedInteger k = 0; k < j; ++k) hess(i, j, 0) *= evaluations[k];
        for (UnsignedInteger k = j + 1; k < i; ++k) hess(i, j, 0) *= evaluations[k];
        for (UnsignedInteger k = i + 1; k < inDimension; ++k) hess(i, j, 0) *= evaluations[k];
      }
      hess(i, i, 0) = secondDerivatives[i];
      for (UnsignedInteger k = 0; k < i; ++k) hess(i, i, 0) *= evaluations[k];
      for (UnsignedInteger k = i + 1; k < inDimension; ++k) hess(i, i, 0) *= evaluations[k];
    }
  }
  return hess;
}

/* Accessor for input point dimension */
UnsignedInteger ProductUniVariateFunctionHessian::getInputDimension() const
{
  return p_evaluation_->functions_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductUniVariateFunctionHessian::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductUniVariateFunctionHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute("evaluation_", *p_evaluation_);
}

/* Method load() reloads the object from the StorageManager */
void ProductUniVariateFunctionHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  TypedInterfaceObject<ProductUniVariateFunctionEvaluation> evaluation;
  adv.loadAttribute("evaluation_", evaluation);
  p_evaluation_ = evaluation.getImplementation();
}


END_NAMESPACE_OPENTURNS
