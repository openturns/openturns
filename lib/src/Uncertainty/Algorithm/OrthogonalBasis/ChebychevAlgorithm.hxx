//                                               -*- C++ -*-
/**
 *  @brief Implement the modified Chebychev algorithm to compute the coefficients of
 *         the 3 terms recurrence relation of an orthonormal polynomial family
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
#ifndef OPENTURNS_CHEBYCHEVALGORITHM_HXX
#define OPENTURNS_CHEBYCHEVALGORITHM_HXX

#include <map>

#include "OrthonormalizationAlgorithmImplementation.hxx"
#include "OrthogonalUniVariatePolynomialFamily.hxx"
#include "Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ChebychevAlgorithm
 *
 * OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
 */

class OT_API ChebychevAlgorithm
  : public OrthonormalizationAlgorithmImplementation
{
  CLASSNAME;
public:

  typedef Collection<NumericalScalar>                 NumericalScalarCollection;
  typedef Collection<Coefficients>                    CoefficientsCollection;
  typedef std::map<UnsignedInteger, NumericalScalar>                 NumericalScalarCache;

  /** Default constructor */
  ChebychevAlgorithm();

  /** Parameter constructor */
  ChebychevAlgorithm(const Distribution & measure);

  /** Parameter constructor with specific reference family */
  ChebychevAlgorithm(const Distribution & measure,
                     const OrthogonalUniVariatePolynomialFamily & family);

  /** Virtual constructor */
  virtual ChebychevAlgorithm * clone() const;

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

  /** Return the order-th modified moment, i.e. the weighted integral of the order-th
      reference polynomial with respect to the underlying measure */
  NumericalScalar getModifiedMoment(const UnsignedInteger order) const;

  /** Methods for the modified Chebichev algorithm */
  /** Mixed moments E[Pj * Qk] where Pj is the j-th monic orthogonal polynomial
      for the given measure and Qk the k-th monic orthogonal polynomial of
      the reference factory */
  NumericalScalar getMixedMoment(const int j,
                                 const UnsignedInteger k) const;

  /** Recurrence coefficients (alphak, betak) of the monic orthogonal polynomials
      Pk+1(x) = (x - alphak) * Pk(x) - betak * Pk-1(x) */
  Coefficients getMonicRecurrenceCoefficients(const UnsignedInteger k) const;

  /** Recurrence coefficients (ak, bk) of the monic reference polynomials
      Qk+1(x) = (x - ak) * Qk(x) - bk * Qk-1(x) */
  Coefficients getReferenceMonicRecurrenceCoefficients(const UnsignedInteger k) const;

  /** Reference orthogonal polynomial factory for the modified moments */
  OrthogonalUniVariatePolynomialFamily referenceFamily_;

  /** Flag to tell if we use the canonical basis */
  Bool useCanonicalBasis_;

  /** Cache to store the raw moments */
  mutable NumericalScalarCollection standardMoments_;

  /** Cache to store the modified moments */
  mutable NumericalScalarCollection modifiedMoments_;

  /** Cache to store the mixed moments */
  mutable NumericalScalarCache mixedMoments_;

  /** Cache to store the monic recurrence coefficients */
  mutable CoefficientsCollection monicRecurrenceCoefficients_;

  /** Cache to store the monic recurrence coefficients */
  mutable CoefficientsCollection referenceMonicRecurrenceCoefficients_;

} ; /* class ChebychevAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CHEBYCHEVALGORITHM_HXX */
