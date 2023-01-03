//                                               -*- C++ -*-
/**
 *  @brief The MeixnerDistribution distribution
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
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  MeixnerDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;
  Complex computeLogCharacteristicFunction(const Scalar x) const override;

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

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

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
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Set simultaneously the tree scale and shape parameters */
  void setBetaAlphaDelta(const Scalar beta,
                         const Scalar alpha,
                         const Scalar delta);

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

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
