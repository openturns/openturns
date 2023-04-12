//                                               -*- C++ -*-
/**
 *  @brief The maximum entropy order statistics distribution
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
#ifndef OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSDISTRIBUTION_HXX
#define OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/PiecewiseHermiteEvaluation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MaximumEntropyOrderStatisticsDistribution
 *
 * The maximum entropy order statistics distribution
 */
class OT_API MaximumEntropyOrderStatisticsDistribution
  : public ContinuousDistribution
{
  CLASSNAME

  /** The MaximumEntropyOrderStatisticsDistributionWrapper class is closely linked with the MaximumEntropyOrderStatisticsDistribution class */
  friend struct MaximumEntropyOrderStatisticsDistributionWrapper;

  /** The MaximumEntropyOrderStatisticsCopula class is closely linked with the MaximumEntropyOrderStatisticsDistribution class */
  friend class MaximumEntropyOrderStatisticsCopula;

public:

  /** A type for distribution collection */
  typedef Collection<Distribution>                 DistributionCollection;
  typedef PersistentCollection<Distribution>       DistributionPersistentCollection;

  /** Default constructor */
  MaximumEntropyOrderStatisticsDistribution();

  /** Parameters constructor */
  explicit MaximumEntropyOrderStatisticsDistribution(const DistributionCollection & coll,
      const Bool useApproximation = ResourceMap::GetAsBool("MaximumEntropyOrderStatisticsDistribution-UseApproximation"),
      const Bool checkMarginals = ResourceMap::GetAsBool("MaximumEntropyOrderStatisticsDistribution-CheckMarginals"));

  /** Comparison operator */
  using ContinuousDistribution::operator ==;
  Bool operator ==(const MaximumEntropyOrderStatisticsDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll,
                                 const Bool useApproximation = ResourceMap::GetAsBool("MaximumEntropyOrderStatisticsDistribution-UseApproximation"),
                                 const Bool checkMarginals = ResourceMap::GetAsBool("MaximumEntropyOrderStatisticsDistribution-CheckMarginals"));

  /** Marginal distributions accessor */
  DistributionCollection getDistributionCollection() const;

  /** Get the copula of a distribution */
  Distribution getCopula() const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  MaximumEntropyOrderStatisticsDistribution * clone() const override;

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
  Scalar computeCDFOld(const Point & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;
private:
  MaximumEntropyOrderStatisticsDistribution getMarginalAsMaximumEntropyOrderStatisticsDistribution(const Indices & indices) const;
public:
  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Tell if the distribution uses approximations for the exponential terms */
  void useApproximation(const Bool flag = true);

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Get the kth approximation */
  PiecewiseHermiteEvaluation getApproximation(const UnsignedInteger k = 0) const;

  /* Interface specific to MaximumEntropyOrderStatisticsDistribution */

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Constructor with no check of the parameters, to speed-up margina creations */
  MaximumEntropyOrderStatisticsDistribution(const DistributionCollection & coll,
      const Indices & partition,
      const Bool useApproximation,
      const Collection<PiecewiseHermiteEvaluation> & exponentialFactorApproximation,
      const Description & description);

  /** Build a C1 interpolation of the exponential factor between two marginals */
  PiecewiseHermiteEvaluation interpolateExponentialFactor(const UnsignedInteger lower,
      const UnsignedInteger upper,
      const UnsignedInteger maximumSubdivision = ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision"),
      const Scalar shift = ResourceMap::GetAsScalar("MaximumEntropyOrderStatisticsDistribution-SupportShift")) const;

  /** Build a C1 interpolation of the exponential factors in the PDF */
  void interpolateExponentialFactors();

public:
  /** Compute the exponential factor */
  Scalar computeExponentialFactor(const UnsignedInteger k,
                                  const Scalar u,
                                  const Scalar v) const;

  /** Compute the factor */
  Scalar computeFactor(const UnsignedInteger k,
                       const Scalar u,
                       const Scalar v) const;
private:
  /** The main parameter set of the distribution */
  DistributionPersistentCollection distributionCollection_;

  /** partition of non-overlapping marginals */
  Indices partition_;

  /** Flag to tell if we use approximation for the exponential term */
  mutable Bool useApproximation_;

  /** interpolation of the exponential factors in the PDF */
  Collection<PiecewiseHermiteEvaluation> exponentialFactorApproximation_;

  /** Integration algorithm */
  GaussKronrod integrator_;

}; /* class MaximumEntropyOrderStatisticsDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSDISTRIBUTION_HXX */
