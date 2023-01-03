//                                               -*- C++ -*-
/**
 *  @brief This is a nD polynomial build as a product of n 1D polynomial
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ProductPolynomialGradient.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductPolynomialGradient)

static const Factory<ProductPolynomialGradient> Factory_ProductPolynomialGradient;


/* Default constructor */
ProductPolynomialGradient::ProductPolynomialGradient()
  : GradientImplementation()
  , polynomials_()
{
  // Nothing to do
}


/* Constructor */
ProductPolynomialGradient::ProductPolynomialGradient(const PolynomialCollection & coll)
  : GradientImplementation()
  , polynomials_(coll)
{
  // Nothing to do
}


/* Virtual constructor */
ProductPolynomialGradient * ProductPolynomialGradient::clone() const
{
  return new ProductPolynomialGradient(*this);
}


/* String converter */
String ProductPolynomialGradient::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Compute the gradient of a product of univariate polynomials */
Matrix ProductPolynomialGradient::gradient (const Point & inP) const
{
  const UnsignedInteger inDimension = inP.getDimension();
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  Scalar productEvaluation = 1.0;
  Point evaluations(inDimension);
  Point derivatives(inDimension);
  for (UnsignedInteger i = 0; i < inDimension; ++i)
  {
    const Scalar x = inP[i];
    const Scalar y = polynomials_[i](x);
    const Scalar dy = polynomials_[i].gradient(x);
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
UnsignedInteger ProductPolynomialGradient::getInputDimension() const
{
  return polynomials_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductPolynomialGradient::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductPolynomialGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "polynomials_", polynomials_ );
}

/* Method load() reloads the object from the StorageManager */
void ProductPolynomialGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "polynomials_", polynomials_ );
}


END_NAMESPACE_OPENTURNS
