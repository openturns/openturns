//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Parametrized distributions
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX
#define OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/DistributionParameters.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametrizedDistribution
 *
 * A subclass for Parametrized distributions.
 */
class OT_API ParametrizedDistribution
  : public DistributionImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ParametrizedDistribution();

  /** Constructor with parameter */
  explicit ParametrizedDistribution(const DistributionParameters & distParam);

  /** Virtual constructor */
  virtual ParametrizedDistribution * clone() const;

  /** Comparison operator */
  Bool operator ==(const ParametrizedDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeComplementaryCDF;
  virtual Scalar computeComplementaryCDF(const Point & point) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;
  Complex computeLogCharacteristicFunction(const Scalar x) const;

  /** Get the PDFGradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  virtual Point computeQuantile(const Scalar prob,
                                const Bool tail = false) const;

  /** Get the product minimum volume interval containing a given probability of the distribution */
  Interval computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

  /** Get the product bilateral confidence interval containing a given probability of the distribution */
  Interval computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

  /** Get the product unilateral confidence interval containing a given probability of the distribution */
  Interval computeUnilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, const Bool tail, Scalar & marginalProbOut) const;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Check if the distribution is elliptical */
  virtual Bool isElliptical() const;

  /** Check if the distribution is continuous */
  virtual Bool isContinuous() const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  virtual Scalar getRoughness() const;

  /** Get the mean of the distribution */
  virtual Point getMean() const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

  virtual Interval getRange() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  DistributionParameters distributionParameters_;
  Distribution distribution_;

}; /* class ParametrizedDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX */
