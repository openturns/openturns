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
  CLASSNAME;
public:

  enum BoundSide { LOWER, UPPER };

  /** Default constructor */
  TruncatedDistribution();

  /** Parameters constructor to use when the two bounds are finite */
  TruncatedDistribution(const Distribution & distribution,
                        const NumericalScalar lowerBound,
                        const NumericalScalar upperBound,
                        const NumericalScalar thresholdRealization = ResourceMap::GetAsNumericalScalar("TruncatedDistribution-DefaultThresholdRealization"));

  /** Parameters constructor to use when one of the bounds is not finite */
  TruncatedDistribution(const Distribution & distribution,
                        const NumericalScalar bound,
                        const BoundSide side = LOWER,
                        const NumericalScalar thresholdRealization = ResourceMap::GetAsNumericalScalar("TruncatedDistribution-DefaultThresholdRealization"));

  /** Parameters constructor to use when the two bounds are finite */
  TruncatedDistribution(const Distribution & distribution,
                        const Interval & truncationInterval,
                        const NumericalScalar thresholdRealization = ResourceMap::GetAsNumericalScalar("TruncatedDistribution-DefaultThresholdRealization"));

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
  NumericalPoint getRealization() const;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;
  using DistributionImplementation::computeComplementaryCDF;
  NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;

  /** Get the PDFGradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDFGradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Parameters value accessors */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to TruncatedDistribution */

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Lower bound accessor */
  void setLowerBound(const NumericalScalar lowerBound);
  NumericalScalar getLowerBound() const;

  /** Lower bound finite flag accessor */
  void setFiniteLowerBound(const Bool finiteLowerBound);
  Bool getFiniteLowerBound() const;

  /** Upper bound accessor */
  void setUpperBound(const NumericalScalar upperBound);
  NumericalScalar getUpperBound() const;

  /** Upper bound finite flag accessor */
  void setFiniteUpperBound(const Bool finiteUpperBound);
  Bool getFiniteUpperBound() const;

  /** Threshold realization accessor */
  void setThresholdRealization(const NumericalScalar thresholdRealization);
  NumericalScalar getThresholdRealization() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Get the support of a distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  NumericalSample getSupport(const Interval & interval) const;

  /** Get the PDF singularities inside of the range - 1D only */
  NumericalPoint getSingularities() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:

  /** Get the quantile of the distribution */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The main parameter set of the distribution */
  Distribution distribution_;
  NumericalScalar lowerBound_;
  Bool finiteLowerBound_;
  NumericalScalar upperBound_;
  Bool finiteUpperBound_;
  NumericalScalar thresholdRealization_;
  /** Usefull quantities */
  NumericalScalar pdfLowerBound_;
  NumericalScalar pdfUpperBound_;
  NumericalScalar cdfLowerBound_;
  NumericalScalar cdfUpperBound_;
  NumericalScalar normalizationFactor_;
}; /* class TruncatedDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRUNCATEDDISTRIBUTION_HXX */
