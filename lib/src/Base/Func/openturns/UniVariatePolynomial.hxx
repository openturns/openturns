//                                               -*- C++ -*-
/**
 *  @brief This is a 1D polynomial
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
#ifndef OPENTURNS_UNIVARIATEPOLYNOMIAL_HXX
#define OPENTURNS_UNIVARIATEPOLYNOMIAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/UniVariatePolynomialImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class UniVariatePolynomial
 *
 * This is a 1D polynomial
 */

class OT_API UniVariatePolynomial
  : public TypedInterfaceObject<UniVariatePolynomialImplementation>
{
  CLASSNAME;
public:

  typedef UniVariatePolynomialImplementation::NumericalComplexCollection NumericalComplexCollection;
  typedef UniVariatePolynomialImplementation::Coefficients               Coefficients;

  /** Default constructor */
  UniVariatePolynomial();

  /** Constructor from coefficients */
  UniVariatePolynomial(const Coefficients & coefficients);

  /** Constructor from implementation */
  UniVariatePolynomial(const UniVariatePolynomialImplementation & implementation);

  /** Constructor from implementation pointer */
  UniVariatePolynomial(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  UniVariatePolynomial(UniVariatePolynomialImplementation * p_implementation);
#endif

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;
  virtual String __str__(const String & variableName,
                         const String & offset) const;

  /** UniVariatePolynomial are evaluated as functors */
  NumericalScalar operator() (const NumericalScalar x) const;
  NumericalComplex operator() (const NumericalComplex z) const;

  /** UniVariatePolynomialImplementation derivative */
  NumericalScalar derivative(const NumericalScalar x) const;

  /** Compute the derivative of the polynomial */
  UniVariatePolynomial derivate() const;

  /** Multiply the polynomial P by a NumericalScalar */
  UniVariatePolynomial operator * (const NumericalScalar scalar) const;

  /** Multiply the polynomial P by a polynomial Q */
  UniVariatePolynomial operator * (const UniVariatePolynomial & uniVariatePolynomial) const;

  /** Multiply the polynomial by (x to the power deg) */
  UniVariatePolynomial incrementDegree (const UnsignedInteger degree = 1) const;

  /** Realize the summation of two polynomials of any degree */
  UniVariatePolynomial operator + (const UniVariatePolynomial & uniVariatePolynomial) const;

  /** Realize the substraction of two polynomials of any degree */
  UniVariatePolynomial operator - (const UniVariatePolynomial & uniVariatePolynomial) const;

  /** Coefficients accessor */
  void setCoefficients(const Coefficients & coefficients);
  Coefficients  getCoefficients() const;

  /** Get the degree of the polynomial */
  UnsignedInteger getDegree() const;

  /** Root of the polynomial of degree n as the eigenvalues of the associated matrix */
  NumericalComplexCollection getRoots() const;

  /** Method to draw the graph of the polynomial between given bounds */
  Graph draw(const NumericalScalar xMin,
             const NumericalScalar xMax,
             const UnsignedInteger pointNumber) const;

} ; /* class UniVariatePolynomial */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_UNIVARIATEPOLYNOMIAL_HXX */
