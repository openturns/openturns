//                                               -*- C++ -*-
/**
 *  @brief The class that implements assembly distributions
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
#ifndef OPENTURNS_COMPOSEDDISTRIBUTION_HXX
#define OPENTURNS_COMPOSEDDISTRIBUTION_HXX

#include "openturns/Distribution.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Copula.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComposedDistribution
 *
 * The class describes the probabilistic concept of distributions
 * made from marginal distributions and from a copula.
 */
class OT_API ComposedDistribution
  : public DistributionImplementation
{
  CLASSNAME;
public:

  /** A type for distribution collection */
  typedef Collection<Distribution>                 DistributionCollection;
  typedef PersistentCollection<Distribution>       DistributionPersistentCollection;

  /** Default constructor for save/load methods : 1D distribution with default Uniform marginal and IndependentCopula */
  ComposedDistribution();

  /** Default constructor, independent copula is supposed */
  explicit ComposedDistribution(const DistributionCollection & coll);

  /** Default constructor */
  ComposedDistribution(const DistributionCollection & coll,
                       const Distribution & copula);


  /** Comparison operator */
  Bool operator ==(const ComposedDistribution & other) const;
 protected:
  Bool equals(const DistributionImplementation & other) const;
 public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll);
  const DistributionCollection & getDistributionCollection() const;

  /** Copula accessor */
  void setCopula(const Distribution & copula);
  Implementation getCopula() const;



  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual ComposedDistribution * clone() const;

  /** Get one realization of the ComposedDistribution */
  NumericalPoint getRealization() const;
protected:
  NumericalSample getSampleParallel(const UnsignedInteger size) const;
public:
  NumericalSample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the ComposedDistribution */
  using DistributionImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the ComposedDistribution */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the ComposedDistribution */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the survival function of the ComposedDistribution */
  using DistributionImplementation::computeSurvivalFunction;
  NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the PDF gradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDF gradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Get the quantile of the distributionImplementation */
  NumericalPoint computeQuantile(const NumericalScalar prob,
                                 const Bool tail = false) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  NumericalScalar computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  NumericalScalar computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

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

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /** Parameters value accessors */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

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
  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The collection of distribution of the ComposedDistribution */
  DistributionPersistentCollection distributionCollection_;

  /** The copula of the ComposedDistribution */
  Distribution copula_;

}; /* class ComposedDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDDISTRIBUTION_HXX */
