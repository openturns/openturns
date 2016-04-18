//                                               -*- C++ -*-
/**
 *  @brief Implement the modified adaptive Stieltjes algorithm to compute
 *         the coefficients of the 3 terms recurrence relation of an
 *         orthonormal polynomial family
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
#ifndef OPENTURNS_ADAPTIVESTIELTJESALGORITHM_HXX
#define OPENTURNS_ADAPTIVESTIELTJESALGORITHM_HXX

#include <map>

#include "openturns/OrthonormalizationAlgorithmImplementation.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AdaptiveStieltjesAlgorithm
 *
 * OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
 */

class OT_API AdaptiveStieltjesAlgorithm
  : public OrthonormalizationAlgorithmImplementation
{
  CLASSNAME;
public:
  typedef Collection<Coefficients>           CoefficientsCollection;
  typedef PersistentCollection<Coefficients> CoefficientsPersistentCollection;

  /** Default constructor */
  AdaptiveStieltjesAlgorithm();

  /** Parameter constructor */
  AdaptiveStieltjesAlgorithm(const Distribution & measure);

  /** Virtual constructor */
  virtual AdaptiveStieltjesAlgorithm * clone() const;

  /** Calculate the coefficients of recurrence a0, a1, a2 such that
      Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  // Structure used to compute the two dot-products needed for the computation of three-terms relation coefficients
  struct DotProductWrapper
  {
    DotProductWrapper(const OrthogonalUniVariatePolynomial & pN,
		      const Distribution & weight):
      pN_(pN), weight_(weight) {};

    NumericalPoint kernelSym(const NumericalPoint & point) const
    {
      const NumericalScalar pdf = weight_.computePDF(point);
      const NumericalScalar x = point[0];
      const NumericalScalar pNX = pN_(x);
      const NumericalScalar xPNX = x * pNX;
      NumericalPoint result(1);
      result[0] = xPNX * xPNX * pdf;
      return result;
    };

    NumericalPoint kernelGen(const NumericalPoint & point) const
    {
      const NumericalScalar pdf = weight_.computePDF(point);
      const NumericalScalar x = point[0];
      const NumericalScalar pNX = pN_(x);
      const NumericalScalar xPNX = x * pNX;
      NumericalPoint result(2);
      result[0] = xPNX * xPNX * pdf;
      result[1] = xPNX * pNX * pdf;
      return result;
    };

    const OrthogonalUniVariatePolynomial & pN_;
    const Distribution & weight_;
  }; // struct DotProductWrapper

  /** Cache to store the recurrence coefficients */
  mutable CoefficientsPersistentCollection recurrenceCoefficients_;

  /** Flag to tell if the underlying distribution is symmetric */
  Bool isElliptical_;

} ; /* class AdaptiveStieltjesAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ADAPTIVESTIELTJESALGORITHM_HXX */
