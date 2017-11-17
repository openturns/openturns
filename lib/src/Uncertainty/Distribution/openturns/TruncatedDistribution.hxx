//                                               -*- C++ -*-
/**
 *  @brief The TruncatedDistribution distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_TRUNCATEDDISTRIBUTION_HXX
#define OPENTURNS_TRUNCATEDDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TruncatedDistribution
 *
 * The TruncatedDistribution distribution.
 */
class OT_API TruncatedDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  enum BoundSide { LOWER, UPPER };

  /** Default constructor */
  TruncatedDistribution();

  /** Parameters constructor to use when the two bounds are finite */
  TruncatedDistribution(const Distribution & distribution,
                        const Scalar lowerBound,
                        const Scalar upperBound,
                        const Scalar thresholdRealization = ResourceMap::GetAsScalar("TruncatedDistribution-DefaultThresholdRealization"));

  /** Parameters constructor to use when one of the bounds is not finite */
  TruncatedDistribution(const Distribution & distribution,
                        const Scalar bound,
                        const BoundSide side = LOWER,
                        const Scalar thresholdRealization = ResourceMap::GetAsScalar("TruncatedDistribution-DefaultThresholdRealization"));

  /** Parameters constructor to use when the two bounds are finite */
  TruncatedDistribution(const Distribution & distribution,
                        const Interval & truncationInterval,
                        const Scalar thresholdRealization = ResourceMap::GetAsScalar("TruncatedDistribution-DefaultThresholdRealization"));

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const TruncatedDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual TruncatedDistribution * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;
  using DistributionImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const;

  /** Get the PDFGradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to TruncatedDistribution */

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Lower bound accessor @deprecated */
  void setLowerBound(const Scalar lowerBound);
  Scalar getLowerBound() const;

  /** Lower bound finite flag accessor @deprecated */
  void setFiniteLowerBound(const Bool finiteLowerBound);
  Bool getFiniteLowerBound() const;

  /** Upper bound accessor @deprecated */
  void setUpperBound(const Scalar upperBound);
  Scalar getUpperBound() const;

  /** Upper bound finite flag accessor @deprecated */
  void setFiniteUpperBound(const Bool finiteUpperBound);
  Bool getFiniteUpperBound() const;

  /** Threshold realization accessor */
  void setThresholdRealization(const Scalar thresholdRealization);
  Scalar getThresholdRealization() const;

  /** Truncation bounds accessor */
  void setBounds(const Interval & bounds);
  Interval getBounds() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Get the support of a distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The main parameter set of the distribution */
  Distribution distribution_;
  Interval bounds_;
  Scalar thresholdRealization_;
  /** Useful quantities */
  Scalar pdfLowerBound_;
  Scalar pdfUpperBound_;
  Scalar cdfLowerBound_;
  Scalar cdfUpperBound_;
  mutable Scalar normalizationFactor_;

  mutable Interval epsilonRange_;
}; /* class TruncatedDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRUNCATEDDISTRIBUTION_HXX */
