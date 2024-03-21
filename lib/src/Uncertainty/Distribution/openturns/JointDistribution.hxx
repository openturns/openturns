//                                               -*- C++ -*-
/**
 *  @brief The class that implements assembly distributions
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_JOINTDISTRIBUTION_HXX
#define OPENTURNS_JOINTDISTRIBUTION_HXX

#include "openturns/Distribution.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class JointDistribution
 *
 * The class describes the probabilistic concept of distributions
 * made from marginal distributions and from a copula.
 */
class OT_API JointDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** A type for distribution collection */
  typedef Collection<Distribution>                 DistributionCollection;
  typedef PersistentCollection<Distribution>       DistributionPersistentCollection;

  /** Default constructor for save/load methods : 1D distribution with default Uniform marginal and IndependentCopula */
  JointDistribution();

  /** Default constructor, independent copula is supposed */
  explicit JointDistribution(const DistributionCollection & coll);

  /** Default constructor */
  JointDistribution(const DistributionCollection & coll,
                       const Distribution & copula);


  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const JointDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;
  String __repr_markdown__() const override;
  String _repr_html_() const override;

  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll);
  DistributionCollection getDistributionCollection() const;

  /** Copula accessor */
  void setCopula(const Distribution & copula);
  Distribution getCopula() const override;



  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  JointDistribution * clone() const override;

  /** Get one realization of the JointDistribution */
  Point getRealization() const override;
protected:
  Sample getSampleParallel(const UnsignedInteger size) const;
public:
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the DDF of the JointDistribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the JointDistribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the logarithm of the PDF of the JointDistribution */
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the JointDistribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the survival function of the JointDistribution */
  using DistributionImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Get the PDF gradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDF gradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;
  Point computeSequentialConditionalQuantile(const Point & q) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const override;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const override;

  /** Get the standard distribution */
  Distribution getStandardDistribution() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /** Check if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


private:
  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** The collection of distribution of the JointDistribution */
  DistributionPersistentCollection distributionCollection_;

  /** The copula of the JointDistribution */
  Distribution copula_;

}; /* class JointDistribution */


// @deprecated
class OT_API ComposedDistribution
  : public JointDistribution
{
CLASSNAME
public:
  ComposedDistribution()
    : JointDistribution() {}

  explicit ComposedDistribution(const DistributionCollection & coll)
    : JointDistribution(coll) {}

  /** Default constructor */
  ComposedDistribution(const DistributionCollection & coll,
                       const Distribution & copula)
    : JointDistribution(coll, copula) {}

};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_JOINTDISTRIBUTION_HXX */
