//                                               -*- C++ -*-
/**
 *  @brief HistogramPolynomial polynomial factory
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_HISTOGRAMPOLYNOMIALFACTORY_HXX
#define OPENTURNS_HISTOGRAMPOLYNOMIALFACTORY_HXX

#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"
#include "openturns/LegendreFactory.hxx"
#include "openturns/Histogram.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HistogramPolynomialFactory
 *
 * Histogram polynomial factory
 */

class OT_API HistogramPolynomialFactory
  : public OrthogonalUniVariatePolynomialFactory
{
  CLASSNAME
public:


  /** Default constructor, associated with the default Histogram distribution */
  HistogramPolynomialFactory();

  /** Parameter constructor */
  HistogramPolynomialFactory(const Scalar first,
                             const Point & width,
                             const Point & height);

  /** Virtual constructor */
  HistogramPolynomialFactory * clone() const override;

  /** Calculate the coefficients of recurrence a0n, a1n, a2n such that
      Pn+1(x) = (a0n * x + a1n) * Pn(x) + a2n * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const override;

  /** First accessor */
  Scalar getFirst() const;

  /** Width accessor */
  Point getWidth() const;

  /** Height accessor */
  Point getHeight() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Compute dot products taking into account the singularities of the weights */
  Point computeDotProduct(const OrthogonalUniVariatePolynomial & qN) const;

  /** Underlying histogram */
  const Histogram * p_histogram_;

  /** Factory used to compute the dot-products */
  LegendreFactory legendre_;

  /** Cache to store the recurrence coefficients */
  mutable CoefficientsPersistentCollection monicRecurrenceCoefficients_;

  /** Cache to store the squared norm of the monic orthogonal polynomials */
  mutable Point monicSquaredNorms_;

} ; /* class HistogramPolynomialFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HISTOGRAMPOLYNOMIALFACTORY_HXX */
