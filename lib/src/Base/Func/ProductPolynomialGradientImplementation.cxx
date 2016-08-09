//                                               -*- C++ -*-
/**
 *  @brief This is a nD polynomial build as a product of n 1D polynomial
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
#include "openturns/ProductPolynomialGradientImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductPolynomialGradientImplementation);

static const Factory<ProductPolynomialGradientImplementation> Factory_ProductPolynomialGradientImplementation;


/* Default constructor */
ProductPolynomialGradientImplementation::ProductPolynomialGradientImplementation()
  : NumericalMathGradientImplementation()
  , polynomials_()
{
  // Nothing to do
}


/* Constructor */
ProductPolynomialGradientImplementation::ProductPolynomialGradientImplementation(const PolynomialCollection & coll)
  : NumericalMathGradientImplementation()
  , polynomials_(coll)
{
  // Nothing to do
}


/* Virtual constructor */
ProductPolynomialGradientImplementation * ProductPolynomialGradientImplementation::clone() const
{
  return new ProductPolynomialGradientImplementation(*this);
}


/* String converter */
String ProductPolynomialGradientImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Compute the gradient of a product of univariate polynomials */
Matrix ProductPolynomialGradientImplementation::gradient (const NumericalPoint & inP) const
{
  const UnsignedInteger inDimension = inP.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  NumericalScalar productEvaluation = 1.0;
  NumericalPoint evaluations(inDimension);
  NumericalPoint derivatives(inDimension);
  for (UnsignedInteger i = 0; i < inDimension; ++i)
  {
    const NumericalScalar x = inP[i];
    const NumericalScalar y = polynomials_[i](x);
    const NumericalScalar dy = polynomials_[i].gradient(x);
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
UnsignedInteger ProductPolynomialGradientImplementation::getInputDimension() const
{
  return polynomials_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductPolynomialGradientImplementation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductPolynomialGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute( "polynomials_", polynomials_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductPolynomialGradientImplementation::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);
  adv.loadAttribute( "polynomials_", polynomials_ );
}


END_NAMESPACE_OPENTURNS
