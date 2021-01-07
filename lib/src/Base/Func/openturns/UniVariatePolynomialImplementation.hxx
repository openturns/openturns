//                                               -*- C++ -*-
/**
 *  @brief This is a 1D polynomial
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_UNIVARIATEPOLYNOMIALIMPLEMENTATION_HXX
#define OPENTURNS_UNIVARIATEPOLYNOMIALIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class UniVariatePolynomialImplementation
 *
 * This is a 1D polynomial
 */

class OT_API UniVariatePolynomialImplementation
  : public UniVariateFunctionImplementation
{
  CLASSNAME
public:

  typedef Pointer<UniVariatePolynomialImplementation> Implementation;
  typedef Matrix::ComplexCollection          ComplexCollection;
  typedef Point                              Coefficients;

  /** Default constructor */
  UniVariatePolynomialImplementation();

  /** Constructor from coefficients */
  UniVariatePolynomialImplementation(const Coefficients & coefficients);

  /** Virtual constructor */
  UniVariatePolynomialImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;
  virtual String __str__(const String & variableName,
                         const String & offset) const;

  /** UniVariatePolynomialImplementation are evaluated as functors */
  Scalar operator() (const Scalar x) const override;
  Complex operator() (const Complex z) const;

  /** UniVariatePolynomialImplementation derivative */
  Scalar gradient(const Scalar x) const override;

  Scalar hessian(const Scalar x) const override;

  /** Compute the derivative of the polynomial */
  UniVariatePolynomialImplementation derivate() const;

  /** Multiply the polynomial P by a Scalar */
  UniVariatePolynomialImplementation operator * (const Scalar scalar) const;

  /** Multiply the polynomial P by a polynomial Q */
  UniVariatePolynomialImplementation operator * (const UniVariatePolynomialImplementation & uniVariatePolynomial) const;

  /** Multiply the polynomial by (x to the power degree) */
  UniVariatePolynomialImplementation incrementDegree (const UnsignedInteger degree = 1) const;

  /** Sum of two polynomials of any degree */
  UniVariatePolynomialImplementation operator + (const UniVariatePolynomialImplementation & uniVariatePolynomial) const;

  /** Subtraction of two polynomials of any degree */
  UniVariatePolynomialImplementation operator - (const UniVariatePolynomialImplementation & uniVariatePolynomial) const;

  /** Coefficients accessor */
  void setCoefficients(const Coefficients & coefficients);
  Coefficients getCoefficients() const;

  /** Get the degree of the polynomial */
  UnsignedInteger getDegree() const;

  /** Root of the polynomial of degree n as the eigenvalues of the associated matrix */
  ComplexCollection getRoots() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Remove null leading coefficients */
  void compactCoefficients();

  /** The table of polynomial coefficients in ascending order: P(X) = C0 + C1 * X + ... + Cn * X^n */
  Coefficients coefficients_;

private:

} ; /* Class UniVariatePolynomialImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_UNIVARIATEPOLYNOMIALIMPLEMENTATION_HXX */
