//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
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
#include "openturns/ProductUniVariateFunctionGradientImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProductUniVariateFunctionGradientImplementation);

static const Factory<ProductUniVariateFunctionGradientImplementation> RegisteredFactory;


/* Default constructor */
ProductUniVariateFunctionGradientImplementation::ProductUniVariateFunctionGradientImplementation()
  : NumericalMathGradientImplementation()
  , functions_()
{
  // Nothing to do
}


/* Constructor */
ProductUniVariateFunctionGradientImplementation::ProductUniVariateFunctionGradientImplementation(const UniVariateFunctionCollection & coll)
  : NumericalMathGradientImplementation()
  , functions_(coll)
{
  // Nothing to do
}


/* Virtual constructor */
ProductUniVariateFunctionGradientImplementation * ProductUniVariateFunctionGradientImplementation::clone() const
{
  return new ProductUniVariateFunctionGradientImplementation(*this);
}


/* String converter */
String ProductUniVariateFunctionGradientImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}


/* Compute the gradient of a product of univariate polynomials */
Matrix ProductUniVariateFunctionGradientImplementation::gradient (const NumericalPoint & inP) const
{
  const UnsignedInteger inDimension(inP.getDimension());
  if (inDimension != getInputDimension()) throw InvalidArgumentException(HERE) << "Error: trying to evaluate a ProductPolynomialFunction with an argument of invalid dimension";
  NumericalScalar productEvaluation(1.0);
  NumericalPoint evaluations(inDimension);
  NumericalPoint derivatives(inDimension);
  for (UnsignedInteger i = 0; i < inDimension; ++i)
  {
    const NumericalScalar x(inP[i]);
    const NumericalScalar y(functions_[i](x));
    const NumericalScalar dy(functions_[i].gradient(x));
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
UnsignedInteger ProductUniVariateFunctionGradientImplementation::getInputDimension() const
{
  return functions_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger ProductUniVariateFunctionGradientImplementation::getOutputDimension() const
{
  return 1;
}


/* Method save() stores the object through the StorageManager */
void ProductUniVariateFunctionGradientImplementation::save(Advocate & adv) const
{
  NumericalMathGradientImplementation::save(adv);
  adv.saveAttribute("functions_", functions_);
}

/* Method load() reloads the object from the StorageManager */
void ProductUniVariateFunctionGradientImplementation::load(Advocate & adv)
{
  NumericalMathGradientImplementation::load(adv);
  adv.loadAttribute("functions_", functions_);
}


END_NAMESPACE_OPENTURNS
