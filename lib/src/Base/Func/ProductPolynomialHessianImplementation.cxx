//                                               -*- C++ -*-
/**
 *  @brief This is a nD polynomial build as a product of n 1D polynomial
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ProductPolynomialHessianImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProductPolynomialHessianImplementation);

static const Factory<ProductPolynomialHessianImplementation> RegisteredFactory;


/* Default constructor */
ProductPolynomialHessianImplementation::ProductPolynomialHessianImplementation()
  : NumericalMathHessianImplementation()
  , polynomials_()
{
  // Nothing to do
}


/* Constructor */
ProductPolynomialHessianImplementation::ProductPolynomialHessianImplementation(const PolynomialCollection & coll)
  : NumericalMathHessianImplementation()
  , polynomials_(coll)
{
  // Nothing to do
}


/* Virtual constructor */
ProductPolynomialHessianImplementation * ProductPolynomialHessianImplementation::clone() const
{
  return new ProductPolynomialHessianImplementation(*this);
}


/* String converter */
String ProductPolynomialHessianImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Compute the hessian of a product of univariate polynomials */
SymmetricTensor ProductPolynomialHessianImplementation::hessian (const NumericalPoint & inP) const
{
  const UnsignedInteger inDimension(inP.getDimension());
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  NumericalScalar productEvaluation(1.0);
  NumericalPoint evaluations(inDimension);
  NumericalPoint derivatives(inDimension);
  NumericalPoint secondDerivatives(inDimension);
  for (UnsignedInteger i = 0; i < inDimension; ++i)
  {
    const NumericalScalar x(inP[i]);
    const NumericalScalar y(polynomials_[i](x));
    const NumericalScalar dy(polynomials_[i].derivative(x));
    const NumericalScalar d2y(polynomials_[i].derivate().derivative(x));
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
      const NumericalScalar dyi(derivatives[i] * (productEvaluation / evaluations[i]));
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
UnsignedInteger ProductPolynomialHessianImplementation::getInputDimension() const
{
  return polynomials_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductPolynomialHessianImplementation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductPolynomialHessianImplementation::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save(adv);
  adv.saveAttribute( "polynomials_", polynomials_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductPolynomialHessianImplementation::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load(adv);
  adv.loadAttribute( "polynomials_", polynomials_ );
}


END_NAMESPACE_OPENTURNS
