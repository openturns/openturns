//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for orthogonal polynomial factories
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
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/HermiteFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(OrthogonalUniVariatePolynomialFamily)

/* Default constructor */
OrthogonalUniVariatePolynomialFamily::OrthogonalUniVariatePolynomialFamily()
  : TypedInterfaceObject<OrthogonalUniVariatePolynomialFactory>(new HermiteFactory)
{
  // Nothing to do
}


/* Constructor from implementation */
OrthogonalUniVariatePolynomialFamily::OrthogonalUniVariatePolynomialFamily(const OrthogonalUniVariatePolynomialFactory & implementation)
  : TypedInterfaceObject<OrthogonalUniVariatePolynomialFactory>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
OrthogonalUniVariatePolynomialFamily::OrthogonalUniVariatePolynomialFamily(OrthogonalUniVariatePolynomialFactory * p_implementation)
  : TypedInterfaceObject<OrthogonalUniVariatePolynomialFactory>(p_implementation)
{
  // Nothing to do
}

/* The method to get the polynomial of any degree */
OrthogonalUniVariatePolynomial OrthogonalUniVariatePolynomialFamily::build(const UnsignedInteger degree) const
{
  return getImplementation()->build(degree);
}


/* Measure accessor */
Distribution OrthogonalUniVariatePolynomialFamily::getMeasure() const
{
  return getImplementation()->getMeasure();
}


/* Calculate the coefficients of recurrence a0, a1, a2 such that
   Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
OrthogonalUniVariatePolynomialFamily::Coefficients OrthogonalUniVariatePolynomialFamily::getRecurrenceCoefficients(const UnsignedInteger n) const
{
  return getImplementation()->getRecurrenceCoefficients(n);
}


/* String converter */
String OrthogonalUniVariatePolynomialFamily::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

/* Roots of the polynomial of degree n */
Point OrthogonalUniVariatePolynomialFamily::getRoots(const UnsignedInteger n) const
{
  return getImplementation()->getRoots(n);
}

/* Nodes and weights of the polynomial of degree n as the eigenvalues of the associated Jacobi matrix and the square
   of the first component of the associated normalized eigenvectors */
Point OrthogonalUniVariatePolynomialFamily::getNodesAndWeights(const UnsignedInteger n,
    Point & weights) const
{
  return getImplementation()->getNodesAndWeights(n, weights);
}


END_NAMESPACE_OPENTURNS
