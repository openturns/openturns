//                                               -*- C++ -*-
/**
 *  @brief Implement the modified adaptive Stieltjes algorithm to compute
 *         the coefficients of the 3 terms recurrence relation of an
 *         orthonormal polynomial family
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
#ifndef OPENTURNS_ADAPTIVESTIELTJESALGORITHM_HXX
#define OPENTURNS_ADAPTIVESTIELTJESALGORITHM_HXX

#include <map>

#include "openturns/OrthonormalizationAlgorithmImplementation.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AdaptiveStieltjesAlgorithm
 *
 * OrthogonalUniVariatePolynomialStandardDistribution polynomial factory
 */

class OT_API AdaptiveStieltjesAlgorithm
  : public OrthonormalizationAlgorithmImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  AdaptiveStieltjesAlgorithm();

  /** Parameter constructor */
  explicit AdaptiveStieltjesAlgorithm(const Distribution & measure);

  /** Virtual constructor */
  AdaptiveStieltjesAlgorithm * clone() const override;

  /** Calculate the coefficients of recurrence a0, a1, a2 such that
      Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Compute dot products taking into account the singularities of the weights */
  Point computeDotProduct(const Function & kernel,
                          const UnsignedInteger n) const;

  // Structure used to compute the two dot-products needed for the computation of three-terms relation coefficients
  struct DotProductWrapper
  {
    DotProductWrapper(const OrthogonalUniVariatePolynomial & qN,
                      const Distribution & weight):
      qN_(qN), weight_(weight)
    {
      // Nothing to do
    };

    // This method allows one to compute <qN, qN>
    Point kernelSym(const Point & point) const
    {
      const Scalar pdf = weight_.computePDF(point);
      const Scalar x = point[0];
      const Scalar qNX = qN_(x);
      Point result(1);
      result[0] = qNX * qNX * pdf;
      return result;
    };

    // This method allows one to compute <qN, qN> and <x.qN, qN>
    Point kernelGen(const Point & point) const
    {
      const Scalar pdf = weight_.computePDF(point);
      const Scalar x = point[0];
      const Scalar qNX = qN_(x);
      const Scalar xQNX = x * qNX;
      Point result(2);
      result[0] = qNX * qNX * pdf;
      result[1] = xQNX * qNX * pdf;
      return result;
    };

    const OrthogonalUniVariatePolynomial & qN_;
    const Distribution & weight_;
  }; // struct DotProductWrapper

  /** Cache to store the recurrence coefficients */
  mutable Sample monicRecurrenceCoefficients_;

  /** Cache to store the squared norm of the monic orthogonal polynomials */
  mutable Point monicSquaredNorms_;

  /** Flag to tell if the underlying distribution is symmetric */
  Bool isElliptical_;

} ; /* class AdaptiveStieltjesAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ADAPTIVESTIELTJESALGORITHM_HXX */
