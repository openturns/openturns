//                                               -*- C++ -*-
/**
 *  @brief The MultivariatePolya distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MULTIVARIATEPOLYA_HXX
#define OPENTURNS_MULTIVARIATEPOLYA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Multinomial.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MultivariatePolya
 *
 * The MultivariatePolya distribution.
 */
class OT_API MultivariatePolya
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  MultivariatePolya();

  /** Parameters constructor */
  MultivariatePolya(const UnsignedInteger n,
                    const Point & alpha,
                    const Scalar c);

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Tell if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const MultivariatePolya & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  MultivariatePolya * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get a sample of the distribution */
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Get the survival function of the distribution */
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Get the i-th marginal distribution */
  using DistributionImplementation::getMarginal;
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

  /* Interface specific to MultivariatePolya */

  /** Alpha vector accessor */
  void setAlpha(const Point & alpha);
  Point getAlpha() const;

  /** C accessor */
  void setC(const Scalar c);
  Scalar getC() const;

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

  /** Update the spectral parameters given the current parameters values */
  void updateSpectral();

  /** Draw one realization of a multinomial distribution with probabilities p */
  static Point drawMultinomial(const UnsignedInteger n,
                               const Point & p);

  /** Compute the log-PDF of the underlying multinomial given the probabilities */
  static Scalar logMultinomialPDF(const UnsignedInteger n,
                                  const Point & p,
                                  const Point & x);

  /** Compute the generating function of a sum of truncated negative binomial distributions as needed in the computeCDF() method */
  Complex computeGlobalPhi(const Complex & z,
                           const Indices & x) const;

  /** Compute the generating function of a sum of truncated negative binomial distributions as needed in the computeProbability() method */
  Complex computeGlobalPhi(const Complex & z,
                           const Indices & a,
                           const Indices & b) const;

  /** Compute the generating function of a truncated negative binomial distribution as needed in the computeCDF() method */
  Complex computeLocalPhi(const Complex & z,
                          const Scalar u,
                          const Scalar t,
                          const UnsignedInteger a) const;

  /** Compute the generating function of a truncated negative binomial distribution as needed in the computeProbability() method */
  Complex computeLocalPhi(const Complex & z,
                          const Scalar u,
                          const Scalar t,
                          const UnsignedInteger a,
                          const UnsignedInteger b) const;

  /** Build the multinomial distribution corresponding to the degenerate case c = 0 */
  Multinomial computeDegenerateDistribution() const;

  /** Quantile computation for dimension=1 */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** The number of draws */
  UnsignedInteger n_;

  /** The vector of concentration parameters of the MultivariatePolya distribution */
  Point alpha_;

  /** The reinforcement parameter of the MultivariatePolya distribution */
  Scalar c_;

  /** The sum of the concentration parameters */
  Scalar q_;

  /** The vector u_ = alpha_ / c_, parameters of the underlying negative binomial distributions */
  Point u_;

  /** The sum of the scaled concentration parameters */
  Scalar U_;

  /** The ratio U_ / (U_ + n_), parameter of the underlying negative binomial distributions */
  Scalar t_;

  /** Normalization factor for the CDF */
  Scalar normalizationCDF_;

  /** Radius of the discretization in Poisson's formula */
  Scalar r_;

  /** Threshold for the A parameter of the CDF algorithm */
  Scalar smallA_;

  /** Normalization for the CDF algorithm */
  Scalar eta_;

}; /* class MultivariatePolya */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTIVARIATEPOLYA_HXX */