//                                               -*- C++ -*-
/**
 *  @brief Implement the modified Gram Schmidt algorithm to compute the coefficients of
 *         tthe 3 terms recurrence relation of an orthonormal polynomial family
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
#ifndef OPENTURNS_GRAMSCHMIDTALGORITHM_HXX
#define OPENTURNS_GRAMSCHMIDTALGORITHM_HXX

#include "openturns/OrthonormalizationAlgorithmImplementation.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class GramSchmidtAlgorithm
 *
 * OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
 */

class OT_API GramSchmidtAlgorithm
  : public OrthonormalizationAlgorithmImplementation
{
  CLASSNAME;
public:

  typedef Collection<NumericalScalar> NumericalScalarCollection;
  typedef Collection<Coefficients>    CoefficientsCollection;

  /** Default constructor */
  GramSchmidtAlgorithm();

  /** Parameter constructor */
  GramSchmidtAlgorithm(const Distribution & measure);

  /** Parameter constructor */
  GramSchmidtAlgorithm(const Distribution & measure,
                       const OrthogonalUniVariatePolynomialFamily & referenceFamily);

  /** Virtual constructor */
  virtual GramSchmidtAlgorithm * clone() const;

  /** Calculate the coefficients of recurrence a0, a1, a2 such that
      Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const;

  /** Reference univariate orthogonal polynomial family accessor */
  void setReferenceFamily(const OrthogonalUniVariatePolynomialFamily & family);
  OrthogonalUniVariatePolynomialFamily getReferenceFamily() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Return the order-th raw moment of the underlying measure */
  NumericalScalar getStandardMoment(const UnsignedInteger order) const;

  /** Build the coefficients of the kth orthonormal polynomial */
  UniVariatePolynomial buildPolynomial(const UnsignedInteger k) const;

  /** Compute the dot product between two general polynomials according to the measure */
  NumericalScalar dotProduct(const UniVariatePolynomial & p1,
                             const UniVariatePolynomial & p2) const;

  /** Cache to store the raw moments */
  mutable NumericalScalarCollection standardMoments_;

  /** Cache to store the coefficients of the orthonormal polynomials */
  mutable CoefficientsCollection coefficientsCache_;

  /** Starting family of polynomials */
  OrthogonalUniVariatePolynomialFamily referenceFamily_;

  /** Flag to tell if we use the canonical basis */
  Bool useCanonicalBasis_;
} ; /* class GramSchmidtAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GRAMSCHMIDTALGORITHM_HXX */
