//                                               -*- C++ -*-
/**
 *  @brief The NormalGamma distribution
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
#ifndef OPENTURNS_NORMALGAMMA_HXX
#define OPENTURNS_NORMALGAMMA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/BayesDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NormalGamma
 *
 * The NormalGamma distribution.
 */
class OT_API NormalGamma
  : public BayesDistribution
{
  CLASSNAME

public:

  /** Default constructor */
  NormalGamma();

  /** Parameters constructor */
  NormalGamma(const Scalar mu,
              const Scalar kappa,
              const Scalar alpha,
              const Scalar beta);

  /** Comparison operator */
  Bool operator ==(const NormalGamma & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Compute the survival function */
  using ContinuousDistribution::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual NormalGamma * clone() const;

  /* Interface specific to NormalGamma */

  /** Mu accessor */
  void setMu(const Scalar mu);
  Scalar getMu() const;

  /** Kappa accessor */
  void setKappa(const Scalar kappa);
  Scalar getKappa() const;

  /** Alpha accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Beta accessor */
  void setBeta(const Scalar beta);
  Scalar getBeta() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Conditioned distribution accessor */
  using BayesDistribution::setConditionedDistribution;
  using BayesDistribution::getConditionedDistribution;

  /** Conditioning distribution accessor */
  using BayesDistribution::setConditioningDistribution;
  using BayesDistribution::getConditioningDistribution;

  /** Link function accessor */
  using BayesDistribution::setLinkFunction;
  using BayesDistribution::getLinkFunction;

private:

  /** Compute the numerical range of the distribution */
  void computeRange();

  /** Compute the log-normalization factor of the PDF */
  void computeLogNormalization();

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The prior mean */
  Scalar mu_;

  /** The prior precision scale */
  Scalar kappa_;

  /** The prior shape */
  Scalar alpha_;

  /** The prior rate */
  Scalar beta_;

  /** Log-normalization factor of the PDF */
  Scalar logNormalization_;
}; /* class NormalGamma */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMALGAMMA_HXX */
