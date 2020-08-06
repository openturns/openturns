//                                               -*- C++ -*-
/**
 *  @brief The LogUniform distribution
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LOGUNIFORM_HXX
#define OPENTURNS_LOGUNIFORM_HXX

#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogUniform
 *
 * The LogUniform distribution.
 */
class OT_API LogUniform
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  LogUniform();

  /** Parameters constructor */
  LogUniform(const Scalar aLog,
             const Scalar bLog);


  /** Comparison operator */
  Bool operator ==(const LogUniform & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  LogUniform * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution, i.e. the gradient of its PDF w.r.t. point */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;

  /** Get the gradient of the PDF w.r.t the parameters of the distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the gradient of the CDF w.r.t the parameters of the distribution */
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

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to LogUniform */

  /** ALog accessor */
  void setALog(const Scalar aLog);
  Scalar getALog() const;

  /** BLog accessor */
  void setBLog(const Scalar bLog);
  Scalar getBLog() const;

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

  /** Get the quantile of the distribution, i.e the value Xp such that P(X <= Xp) = prob */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** The main parameter set of the distribution */
  /** Lower bound on the logarithmic scale */
  Scalar aLog_;
  /** Upper bound on the logarithmic scale */
  Scalar bLog_;

  /** Derivative parameters */
  /** Lower bound on the natural scale */
  Scalar a_;
  /** Upper bound on the natural scale */
  Scalar b_;

}; /* class LogUniform */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGUNIFORM_HXX */
