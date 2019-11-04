//                                               -*- C++ -*-
/**
 *  @brief A class that implements an independent copula
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
#ifndef OPENTURNS_INDEPENDENTCOPULA_HXX
#define OPENTURNS_INDEPENDENTCOPULA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/CopulaImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IndependentCopula
 *
 * The class implements an independent copula
 */
class OT_API IndependentCopula
  : public CopulaImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  explicit IndependentCopula(const UnsignedInteger dimension = 2);


  /** Comparison operator */
  Bool operator ==(const IndependentCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual IndependentCopula * clone() const;

  /** Get one realization of the IndependentCopula distribution */
  Point getRealization() const;

  /** Get the DDF of the distribution */
  using CopulaImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the IndependentCopula distribution */
  using CopulaImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the IndependentCopula distribution */
  using CopulaImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Compute the survival function */
  using CopulaImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Get the product minimum volume interval containing a given probability of the distribution */
  Interval computeMinimumVolumeIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

  /** Get the product bilateral confidence interval containing a given probability of the distribution */
  Interval computeBilateralConfidenceIntervalWithMarginalProbability(const Scalar prob, Scalar & marginalProbOut) const;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const;

  /** Get the PDF gradient of the distribution */
  Point computePDFGradient(const Point & point) const;

  /** Get the CDF gradient of the distribution */
  Point computeCDFGradient(const Point & point) const;

  /** Get the quantile of the IndependentCopula distribution */
  using DistributionImplementation::computeQuantile;
#ifndef SWIG
  Point computeQuantile(const Scalar prob,
                        const Bool tail,
                        Scalar & marginalProb) const;
#endif

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const;

  /** Compute the DDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  virtual Scalar computeConditionalDDF(const Scalar x, const Point & y) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  virtual Scalar computeConditionalPDF(const Scalar x, const Point & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  virtual Scalar computeConditionalCDF(const Scalar x, const Point & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  virtual Scalar computeConditionalQuantile(const Scalar q, const Point & y) const;

  /** Get the isoprobabilistic transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilistic transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Tell if the distribution is elliptical */
  Bool isElliptical() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Parameters value and description accessor */
  Point getParameter() const;
  Description getParameterDescription() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

private:

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

}; /* class IndependentCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDEPENDENTCOPULA_HXX */
