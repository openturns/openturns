//                                               -*- C++ -*-
/**
 *  @brief The MeixnerDistribution distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MEIXNERDISTRIBUTION_HXX
#define OPENTURNS_MEIXNERDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/PiecewiseHermiteEvaluation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MeixnerDistribution
 *
 * The MeixnerDistribution distribution.
 */
class OT_API MeixnerDistribution
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  MeixnerDistribution();

  /** Parameters constructor */
  MeixnerDistribution(const Scalar beta,
                      const Scalar alpha,
                      const Scalar delta,
                      const Scalar gamma);


  /** Comparison operator */
  Bool operator ==(const MeixnerDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual MeixnerDistribution * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;
  Complex computeLogCharacteristicFunction(const Scalar x) const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to MeixnerDistribution */

  /** Alpha accessor */
  void setAlpha(const Scalar alpha);
  Scalar getAlpha() const;

  /** Beta accessor */
  void setBeta(const Scalar beta);
  Scalar getBeta() const;

  /** Delta accessor */
  void setDelta(const Scalar delta);
  Scalar getDelta() const;

  /** Gamma accessor */
  void setGamma(const Scalar gamma);
  Scalar getGamma() const;

  /** @deprecated Mu accessor */
  void setMu(const Scalar mu);
  Scalar getMu() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** Initialize optimization solver parameter using the ResourceMap */
  void initializeOptimizationAlgorithmParameter();

  /** Optimization solver accessor */
  OptimizationAlgorithm getOptimizationAlgorithm() const;
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);

protected:

  /** Optimization solver */
  mutable OptimizationAlgorithm  solver_;

private:

  /** Get the quantile of the distribution, i.e the value Xp such that P(X <= Xp) = prob */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Set simultaneously the tree scale and shape parameters */
  void setBetaAlphaDelta(const Scalar beta,
                         const Scalar alpha,
                         const Scalar delta);

  /** @deprecated Set simultaneously the tree scale and shape parameters */
  void setAlphaBetaDelta(const Scalar alpha,
                         const Scalar beta,
                         const Scalar delta);

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Update the derivative attributes */
  void update();

  /** The main parameter set of the distribution */
  Scalar beta_;
  Scalar alpha_;
  Scalar delta_;
  Scalar gamma_;

  /** The logarithm of the normalization factor */
  Scalar logNormalizationFactor_;

  /** Bounds for the ratio of uniform sampling algorithm */
  Scalar b_;
  Scalar c_;
  Scalar dc_;

  /** CDF approximation */
  PiecewiseHermiteEvaluation cdfApproximation_;

  /** Complementary CDF approximation */
  PiecewiseHermiteEvaluation ccdfApproximation_;

}; /* class MeixnerDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MEIXNERDISTRIBUTION_HXX */
