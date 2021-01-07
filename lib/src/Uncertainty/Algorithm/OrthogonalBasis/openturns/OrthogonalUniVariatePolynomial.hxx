//                                               -*- C++ -*-
/**
 *  @brief This is an orthogonal 1D polynomial
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
#ifndef OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIAL_HXX
#define OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIAL_HXX

#include "openturns/UniVariatePolynomialImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalUniVariatePolynomial
 *
 * This is an orthogonal 1D polynomial. The polynomial P0 is constant equal to 1.0, and by convention we note P-1(x) the null polynomial. For n>=1 we have: Pn+1(x) = (an * x + bn) * Pn(x) + cn * Pn-1(x)
 */

class OT_API OrthogonalUniVariatePolynomial
  : public UniVariatePolynomialImplementation
{
  CLASSNAME
public:

  typedef Collection<Coefficients>           CoefficientsCollection;
  typedef PersistentCollection<Coefficients> CoefficientsPersistentCollection;

  /** Default constructor */
  OrthogonalUniVariatePolynomial();

  /** Constructor from coefficients */
  OrthogonalUniVariatePolynomial(const CoefficientsCollection & recurrenceCoefficients);

  /** Virtual constructor */
  OrthogonalUniVariatePolynomial * clone() const override;

  /** OrthogonalUniVariatePolynomial are evaluated as functors */
  Scalar operator() (const Scalar x) const override;

  /** Recurrence coefficients accessor */
  CoefficientsCollection getRecurrenceCoefficients() const;

  /** Roots of the polynomial of degree n as the eigenvalues of the associated Jacobi matrix */
  ComplexCollection getRoots() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  friend class OrthogonalUniVariatePolynomialFactory;

  /** Constructor from recurrence coefficients and coefficients. It is protected to prevent the end user to give incoherent coefficients. */
  OrthogonalUniVariatePolynomial(const CoefficientsCollection & recurrenceCoefficients,
                                 const Coefficients & coefficients);

private:

  /** Build the coefficients of the polynomial based on the recurrence coefficients */
  Coefficients buildCoefficients(const UnsignedInteger n);

  /** The recurrence coefficients (an, bn, cn) that defines the orthogonal polynomial for n >= 0. The polynomial P0 is constant equal to 1.0, and by convention we note P-1(x) the null polynomial. For n>=1 we have: Pn+1(x) = (an * x + bn) * Pn(x) + cn * Pn-1(x). The recurrence coefficients are stored starting with (a1, b1, c1). */
  CoefficientsPersistentCollection recurrenceCoefficients_;

} ; /* class OrthogonalUniVariatePolynomial */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIAL_HXX */
