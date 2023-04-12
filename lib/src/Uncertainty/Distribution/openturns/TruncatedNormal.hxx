//                                               -*- C++ -*-
/**
 *  @brief The TruncatedNormal distribution
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
#ifndef OPENTURNS_TRUNCATEDNORMAL_HXX
#define OPENTURNS_TRUNCATEDNORMAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TruncatedNormal
 *
 * The TruncatedNormal distribution.
 */
class OT_API TruncatedNormal
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  TruncatedNormal();

  /** Parameters constructor */
  TruncatedNormal(const Scalar mu,
                  const Scalar sigma,
                  const Scalar a,
                  const Scalar b);



  /** Comparison operator */
  using ContinuousDistribution::operator ==;
  Bool operator ==(const TruncatedNormal & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  TruncatedNormal * clone() const override;

  /** Get one realization of the TruncatedNormal distribution */
  Point getRealization() const override;

  /** Get the DDF of the TruncatedNormal distribution */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the TruncatedNormal distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the logarithm of the PDF of the TruncatedNormal distribution */
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the TruncatedNormal distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the product minimum volume interval containing a given probability of the distribution */
  Interval computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const override;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;
  Complex computeLogCharacteristicFunction(const Scalar x) const override;

  /** Get the PDFGradient of the TruncatedNormal distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the LogPDFGradient of the TruncatedNormal distribution */
  using ContinuousDistribution::computeLogPDFGradient;
  Point computeLogPDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the TruncatedNormal distribution */
  using ContinuousDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to TruncatedNormal */

  /** Mu accessor */
  void setMu(const Scalar mu);
  Scalar getMu() const;

  /** Sigma accessor */
  void setSigma(const Scalar sigma);
  Scalar getSigma() const;

  /** A accessor */
  void setA(const Scalar a);
  Scalar getA() const;

  /** B accessor */
  void setB(const Scalar b);
  Scalar getB() const;

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

  /** Get the quantile of the TruncatedNormal distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** The main parameter set of the distribution */
  Scalar mu_;
  Scalar sigma_;
  Scalar a_;
  Scalar b_;
  /** Useful quantities */
  Scalar aNorm_;
  Scalar bNorm_;
  Scalar phiANorm_;
  Scalar phiBNorm_;
  Scalar PhiANorm_;
  Scalar PhiBNorm_;
  Scalar normalizationFactor_;

}; /* class TruncatedNormal */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRUNCATEDNORMAL_HXX */
