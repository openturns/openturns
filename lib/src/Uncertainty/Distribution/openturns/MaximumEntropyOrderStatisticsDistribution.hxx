//                                               -*- C++ -*-
/**
 *  @brief The maximum entropy order statistics distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSDISTRIBUTION_HXX
#define OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"
#include "openturns/PiecewiseHermiteEvaluationImplementation.hxx"
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
  CLASSNAME;

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
  Bool operator ==(const MaximumEntropyOrderStatisticsDistribution & other) const;
 protected:
  Bool equals(const DistributionImplementation & other) const;
 public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll,
                                 const Bool useApproximation = ResourceMap::GetAsBool("MaximumEntropyOrderStatisticsDistribution-UseApproximation"),
                                 const Bool checkMarginals = ResourceMap::GetAsBool("MaximumEntropyOrderStatisticsDistribution-CheckMarginals"));

  /** Marginal distributions accessor */
  DistributionCollection getDistributionCollection() const;

  /** Get the copula of a distribution */
  Implementation getCopula() const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual MaximumEntropyOrderStatisticsDistribution * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;
  using ContinuousDistribution::computeLogPDF;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;
  NumericalScalar computeCDFOld(const NumericalPoint & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  NumericalScalar computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  NumericalScalar computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  NumericalScalar computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;
private:
  MaximumEntropyOrderStatisticsDistribution getMarginalAsMaximumEntropyOrderStatisticsDistribution(const Indices & indices) const;
public:
  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /** Tell if the distribution uses approximations for the exponential terms */
  void useApproximation(const Bool flag = true);

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Get the kth approximation */
  PiecewiseHermiteEvaluationImplementation getApproximation(const UnsignedInteger k = 0) const;

  /* Interface specific to MaximumEntropyOrderStatisticsDistribution */

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Constructor with no check of the parameters, to speed-up margina creations */
  MaximumEntropyOrderStatisticsDistribution(const DistributionCollection & coll,
      const Indices & partition,
      const Bool useApproximation,
      const Collection<PiecewiseHermiteEvaluationImplementation> & exponentialFactorApproximation,
      const Description & description);

  /** Build a C1 interpolation of the exponential factor between two marginals */
  PiecewiseHermiteEvaluationImplementation interpolateExponentialFactor(const UnsignedInteger lower,
      const UnsignedInteger upper,
      const UnsignedInteger maximumSubdivision = ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision"),
      const NumericalScalar shift = ResourceMap::GetAsNumericalScalar("MaximumEntropyOrderStatisticsDistribution-SupportShift")) const;

  /** Build a C1 interpolation of the exponential factors in the PDF */
  void interpolateExponentialFactors();

public:
  /** Compute the exponential factor */
  NumericalScalar computeExponentialFactor(const UnsignedInteger k,
      const NumericalScalar u,
      const NumericalScalar v) const;

  /** Compute the factor */
  NumericalScalar computeFactor(const UnsignedInteger k,
                                const NumericalScalar u,
                                const NumericalScalar v) const;
private:
  /** The main parameter set of the distribution */
  DistributionPersistentCollection distributionCollection_;

  /** partition of non-overlapping marginals */
  Indices partition_;

  /** Flag to tell if we use approximation for the exponential term */
  mutable Bool useApproximation_;

  /** interpolation of the exponential factors in the PDF */
  Collection<PiecewiseHermiteEvaluationImplementation> exponentialFactorApproximation_;

  /** Integration algorithm */
  GaussKronrod integrator_;

}; /* class MaximumEntropyOrderStatisticsDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMENTROPYORDERSTATISTICSDISTRIBUTION_HXX */
