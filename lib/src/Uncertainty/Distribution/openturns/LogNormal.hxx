//                                               -*- C++ -*-
/**
 *  @brief The LogNormal distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LOGNORMAL_HXX
#define OPENTURNS_LOGNORMAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogNormal
 *
 * The LogNormal distribution.
 */
class OT_API LogNormal
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  LogNormal();

  /** Parameters constructor */
  LogNormal(const Scalar muLog,
            const Scalar sigmaLog,
            const Scalar gamma = 0.0);

  /** Comparison operator */
  Bool operator ==(const LogNormal & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  LogNormal * clone() const override;

  /** Get one realization of the LogNormal distribution */
  Point getRealization() const override;

  /** Get the DDF of the LogNormal distribution */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the LogNormal distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the LogNormal distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;
  Complex computeLogCharacteristicFunction(const Scalar x) const override;

  /** Get the PDFGradient of the LogNormal distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the LogNormal distribution */
  using ContinuousDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const override;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to LogNormal */

  /** MuLog accessor */
  void setMuLog(const Scalar muLog);
  Scalar getMuLog() const;

  /** SigmaLog accessor */
  void setSigmaLog(const Scalar sigmaLog);
  Scalar getSigmaLog() const;


  /** Gamma accessor */
  void setGamma(const Scalar gamma);
  Scalar getGamma() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** MuLogSigmaLog accessor */
  void setMuLogSigmaLog(const Scalar muLog,
                        const Scalar sigmaLog);

private:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Get the quantile of the LogNormal distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the integrand that is involved in the computation of the characteristic function */
  Complex characteristicIntegrand(const Scalar eta,
                                  const Scalar sStar) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** The main parameter set of the distribution */
  Scalar muLog_;
  Scalar sigmaLog_;
  Scalar gamma_;
  Scalar normalizationFactor_;
  /** Normalization factor for the characteristic function */
  Scalar H_;
  /** Hermite integration */
  mutable Point hermiteNodes_;
  mutable Point hermiteWeights_;

}; /* class LogNormal */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGNORMAL_HXX */
