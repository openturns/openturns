//                                               -*- C++ -*-
/**
 *  @brief The PointConditionalDistribution distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_POINTCONDITIONALDISTRIBUTION_HXX
#define OPENTURNS_POINTCONDITIONALDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PointConditionalDistribution
 *
 * The PointConditionalDistribution distribution.
 */
class OT_API PointConditionalDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PointConditionalDistribution();

  /** Parameters constructor */
  PointConditionalDistribution(const Distribution & distribution,
                               const Indices & conditioningIndices,
                               const Point & conditioningValues);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const PointConditionalDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  PointConditionalDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the probability of an interval */
  using DistributionImplementation::computeProbability;
  Scalar computeProbability(const Interval & interval) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;
  Point computeSequentialConditionalQuantile(const Point & q) const override;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const override;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const override;

  /** Get the quantile of the distribution */
  Point computeQuantile(const Scalar prob, const Bool tail = false) const override;
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /** Check if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Check if the distribution is integral */
  Bool isIntegral() const override;

  /** Get the support of a discrete distribution that intersects a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Get the PDF gradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDF gradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /* Interface specific to PointConditionalDistribution */

  /** Conditioned distribution accessor */
  Distribution getDistribution() const;

  /** Probability table accessor */
  Indices getConditioningIndices() const;
  Point getConditioningValues() const;

  /** Get the simplified version (or clone the distribution) */
  Distribution getSimplifiedVersion() const;

  /** Integration algorithm accessor */
  void setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm);
  IntegrationAlgorithm getIntegrationAlgorithm() const;

protected:

private:
  /** Expand the given marginal point to the underlying distribution argument point */
  Point expandPoint(const Point & point) const;

  /* Get the simplified version */
  void dispatchConditioning(const Collection<Distribution> & distributions, Distribution & simplified) const;

  /* Get the simplified version */
  Bool hasSimplifiedVersion(Distribution & simplified) const;

  // decompose elliptical distribution mu/cov
  Point decompose(CovarianceMatrix & C) const;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Update byproducts */
  void update();

  // main attributes
  Distribution distribution_;
  Indices conditioningIndices_;
  Point conditioningValues_;

  // cache some byproducts
  Indices nonConditioningIndices_;
  Distribution marginalConditionedDistribution_;
  Scalar logNormalizationFactor_ = 0.0;
  Distribution reorderedDistribution_;
  Point conditioningCDF_;

  // for ratio of uniforms method
  Scalar r_ = 1.0;
  Scalar supU_ = 0.0;
  Point infV_;
  Point supV_;

  // for discrete sampling using alias method
  Point base_;
  Indices alias_;
  Point probabilities_;
  Sample support_;

  // simplified instance
  Distribution simplifiedVersion_;
  Bool useSimplifiedVersion_ = false;

  // for CDF integration
  IntegrationAlgorithm integrationAlgorithm_;

}; /* class PointConditionalDistribution */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTCONDITIONALDISTRIBUTION_HXX */
