//                                               -*- C++ -*-
/**
 *  @brief Polynomial function factory
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
#include "openturns/OrthogonalUniVariatePolynomialFunctionFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Normal.hxx"
#include "openturns/HermiteFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariatePolynomialFunctionFactory)

static const Factory<OrthogonalUniVariatePolynomialFunctionFactory> Factory_OrthogonalUniVariatePolynomialFunctionFactory;


/* Default constructor */
OrthogonalUniVariatePolynomialFunctionFactory::OrthogonalUniVariatePolynomialFunctionFactory()
  : OrthogonalUniVariateFunctionFactory(Normal())
  , polynomialFactory_(HermiteFactory())
{
  initializeCache();
}


/* Parameters constructor */
OrthogonalUniVariatePolynomialFunctionFactory::OrthogonalUniVariatePolynomialFunctionFactory(const OrthogonalUniVariatePolynomialFamily & polynomialFactory)
  : OrthogonalUniVariateFunctionFactory(polynomialFactory.getMeasure())
  , polynomialFactory_(polynomialFactory)
{
  initializeCache();
}


/* Virtual constructor */
OrthogonalUniVariatePolynomialFunctionFactory * OrthogonalUniVariatePolynomialFunctionFactory::clone() const
{
  return new OrthogonalUniVariatePolynomialFunctionFactory(*this);
}


/* String converter */
String OrthogonalUniVariatePolynomialFunctionFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}

/* The method to get the function of any order */
UniVariateFunction OrthogonalUniVariatePolynomialFunctionFactory::build(const UnsignedInteger order) const
{
  UniVariatePolynomial polynomial(polynomialFactory_.build(order));
  return polynomial.getImplementation()->clone();
}


/* Method save() stores the object through the StorageManager */
void OrthogonalUniVariatePolynomialFunctionFactory::save(Advocate & adv) const
{
  OrthogonalUniVariateFunctionFactory::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void OrthogonalUniVariatePolynomialFunctionFactory::load(Advocate & adv)
{
  OrthogonalUniVariateFunctionFactory::load(adv);
}

END_NAMESPACE_OPENTURNS
