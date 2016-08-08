//                                               -*- C++ -*-
/**
 *  @brief The class that implements a generic mechanism to extract marginal distributions
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
#ifndef OPENTURNS_MARGINALDISTRIBUTION_HXX
#define OPENTURNS_MARGINALDISTRIBUTION_HXX

#include "openturns/Distribution.hxx"
#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MarginalDistribution
 *
 * The class describes the probabilistic concept of marginal distributions
 */
class OT_API MarginalDistribution
  : public DistributionImplementation
{
  CLASSNAME;
public:

  /** Default constructor for save/load methods : 1D distribution with default Uniform underlying distribution */
  MarginalDistribution();

  /** Default constructor, independent copula is supposed */
  MarginalDistribution(const Distribution & distribution,
                       const UnsignedInteger & index);

  MarginalDistribution(const Distribution & distribution,
                       const Indices & indices);

  /** Comparison operator */
  Bool operator ==(const MarginalDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Indices accessor */
  void setIndices(const Indices & indices);
  Indices getIndices() const;

private:
  /** Set the distribution and the indices in one shot */
  void setDistributionAndIndices(const Distribution & distribution,
                                 const Indices & indices);
public:

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual MarginalDistribution * clone() const;

  /** Get one realization of the MarginalDistribution */
  NumericalPoint getRealization() const;
  NumericalSample getSample(const UnsignedInteger size) const;

  /** Get the CDF of the MarginalDistribution */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the survival function of the MarginalDistribution */
  using DistributionImplementation::computeSurvivalFunction;
  NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Get the Spearman correlation of the distribution */
  CorrelationMatrix getSpearmanCorrelation() const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Get the standard distribution */
  Implementation getStandardDistribution() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** Check if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


private:
  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Expand the given marginal point to the underlying distribution argument point */
  NumericalPoint expandPoint(const NumericalPoint & point,
                             const Bool upper = true) const;

  /** Reduce the given point to the marginal point */
  NumericalPoint reducePoint(const NumericalPoint & point) const;

  /** The distribution under the MarginalDistribution */
  Distribution distribution_;

  /** The indices of the MarginalDistribution */
  Indices indices_;

  /** The lower bound of the underlying distribution */
  NumericalPoint lowerBound_;

  /** The upper bound of the underlying distribution */
  NumericalPoint upperBound_;

}; /* class MarginalDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARGINALDISTRIBUTION_HXX */
