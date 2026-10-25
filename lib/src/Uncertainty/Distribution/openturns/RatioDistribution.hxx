//                                               -*- C++ -*-
/**
 *  @brief The RatioDistribution distribution
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
#ifndef OPENTURNS_RATIODISTRIBUTION_HXX
#define OPENTURNS_RATIODISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/GaussKronrod.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RatioDistribution
 *
 * The RatioDistribution distribution, defined as the distribution of X / Y
 * where X and Y are two independent univariate continuous random variables.
 */
class OT_API RatioDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  RatioDistribution();

  /** Parameters constructor */
  RatioDistribution(const Distribution & left,
                    const Distribution & right);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const RatioDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  RatioDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Scalar scalar) const override;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Scalar scalar) const override;
  Scalar computeCDF(const Point & point) const override;

  /** Get the complementary CDF of the distribution */
  using DistributionImplementation::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Scalar scalar) const override;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the raw moments of the distribution */
  Point getMoment(const UnsignedInteger n) const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to RatioDistribution */

  /** Distribution accessor */
  void setLeft(const Distribution & left);
  Distribution getLeft() const;

  /** Distribution accessor */
  void setRight(const Distribution & right);
  Distribution getRight() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Compute the CDF in the far tails when the denominator support contains zero */
  Scalar computeCDFTail(const Scalar z) const;

  /** Compute the complementary CDF in the far upper tail */
  Scalar computeCDFTailComplement(const Scalar z) const;

  /** Compute the tail integral used by the CDF and complementary CDF */
  Scalar computeCDFTailIntegral(const Scalar z) const;

  /** Compute the inverse moment E[1 / Y^n] of the denominator */
  Scalar computeInverseMoment(const UnsignedInteger n) const;

  /** The numerator and denominator distributions */
  Pointer<DistributionImplementation> p_left_;
  Pointer<DistributionImplementation> p_right_;

  /** Integration algorithm */
  GaussKronrod algo_;

}; /* class RatioDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RATIODISTRIBUTION_HXX */
