//                                               -*- C++ -*-
/**
 *  @brief The Multinomial distribution
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
#ifndef OPENTURNS_MULTINOMIAL_HXX
#define OPENTURNS_MULTINOMIAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Multinomial
 *
 * The Multinomial distribution.
 */
class OT_API Multinomial
  : public DiscreteDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  Multinomial();


  /** Parameters constructor */
  Multinomial(const UnsignedInteger n,
              const Point & p);


  /** Comparison operator */
  Bool operator ==(const Multinomial & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  Multinomial * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  Sample getSample(const UnsignedInteger size) const override;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  Scalar computePDF(const Indices & point) const override;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Get the survival function of the distribution */
  using DiscreteDistribution::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;

  /** Get the i-th marginal distribution */
  using DiscreteDistribution::getMarginal;
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const override;
  Sample getSupport() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to Multinomial */

  /** P vector accessor */
  void setP(const Point & p);
  Point getP() const;

  /** N accessor */
  void setN(const UnsignedInteger n);
  UnsignedInteger getN() const;

  /** SmallA accessor */
  void setSmallA(const Scalar smallA);
  Scalar getSmallA() const;

  /** Eta accessor */
  void setEta(const Scalar eta);
  Scalar getEta() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Compute the generating function of a sum of truncated Poisson distributions as needed in the computeCDF() method */
  Complex computeGlobalPhi(const Complex & z,
                           const Indices & x) const;

  /** Compute the generating function of a sum of truncated Poisson distributions as needed in the computeProbability() method */
  Complex computeGlobalPhi(const Complex & z,
                           const Indices & a,
                           const Indices & b) const;

  /** Compute the generating function of a truncated Poisson distributions as needed in the computeCDF() method */
  Complex computeLocalPhi(const Complex & z,
                          const Scalar lambda,
                          const UnsignedInteger a) const;

  /** Compute the generating function of a truncated Poisson distributions as needed in the computeProbability() method */
  Complex computeLocalPhi(const Complex & z,
                          const Scalar lambda,
                          const UnsignedInteger a,
                          const UnsignedInteger b) const;

  /** Quantile computation for dimension=1 */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** The range of the output */
  UnsignedInteger n_;

  /** The vector of probabilities of the Multinomial distribution */
  Point p_;

  /** The sum of probabilities of the Multinomial distribution */
  Scalar sumP_;

  /** Normalization factor for the CDF */
  Scalar normalizationCDF_;

  /** Radius of the discretization in Poisson's formula */
  Scalar r_;

  /** Threshold for the A parameter of the CDF algorithm */
  Scalar smallA_;

  /** Normalization for the CDF algorithm */
  Scalar eta_;

}; /* class Multinomial */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTINOMIAL_HXX */
