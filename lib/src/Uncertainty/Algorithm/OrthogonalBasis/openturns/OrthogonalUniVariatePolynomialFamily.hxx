//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for orthogonal polynomial factories
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
#ifndef OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIALFAMILY_HXX
#define OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIALFAMILY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalUniVariatePolynomialFamily
 *
 * This is the interface class for orthogonal polynomial factories
 */

class OT_API OrthogonalUniVariatePolynomialFamily
  : public TypedInterfaceObject<OrthogonalUniVariatePolynomialFactory>
{
  CLASSNAME
public:

  typedef OrthogonalUniVariatePolynomialFactory::Coefficients                   Coefficients;

  /** Default constructor */
  OrthogonalUniVariatePolynomialFamily();

  /** Constructor from implementation */
  OrthogonalUniVariatePolynomialFamily(const OrthogonalUniVariatePolynomialFactory & implementation);

  /** String converter */
  virtual String __repr__() const;

  /** The method to get the polynomial of any degree. */
  OrthogonalUniVariatePolynomial build(const UnsignedInteger degree) const;

  /** Roots of the polynomial of degree n as the eigenvalues of the associated matrix */
  Point getRoots(const UnsignedInteger n) const;

  /** Nodes and weights of the polynomial of degree n as the eigenvalues of the associated matrix, to build quadrature rules */
  Point getNodesAndWeights(const UnsignedInteger n,
                           Point & weightsOut) const;

  /** Measure accessor */
  Distribution getMeasure() const;

  /** Calculate the coefficients of recurrence a0, a1, a2 such that
      Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const;

protected:

private:

} ; /* class OrthogonalUniVariatePolynomialFamily */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIALFAMILY_HXX */
