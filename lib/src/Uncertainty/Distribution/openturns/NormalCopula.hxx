//                                               -*- C++ -*-
/**
 *  @brief A class that implements a normal copula
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
#ifndef OPENTURNS_NORMALCOPULA_HXX
#define OPENTURNS_NORMALCOPULA_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NormalCopula
 *
 * The class implements a normal copula
 */
class OT_API NormalCopula
  : public DistributionImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  explicit NormalCopula(const UnsignedInteger dim = 2);

  /** Default constructor */
  explicit NormalCopula(const CorrelationMatrix & correlation);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const NormalCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  NormalCopula * clone() const override;

  /** Get one realization of the NormalCopula distribution */
  Point getRealization() const override;
protected:
  Sample getSampleParallel(const UnsignedInteger size) const;
public:
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the DDF of the NormalCopula distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the NormalCopula distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the NormalCopula distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using DistributionImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the shape matrix of the copula */
  CorrelationMatrix getShapeMatrix() const override;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const override;

  /** Get the PDF gradient of the distribution */
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDF gradient of the distribution */
  Point computeCDFGradient(const Point & point) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeConditionalPDF(const Point & x, const Sample & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;
  Point computeSequentialConditionalQuantile(const Point & q) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using DistributionImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const override;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Dependence coefficients */
  CorrelationMatrix computeUpperTailDependenceMatrix() const override;
  CorrelationMatrix computeLowerTailDependenceMatrix() const override;

  /** Parameters value and description accessor */
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Compute the correlation matrix of a Normal Copula from its Spearman correlation matrix */
  static CorrelationMatrix GetCorrelationFromSpearmanCorrelation(const CorrelationMatrix & matrix);

  /** Compute the correlation matrix of a Normal Copula from its Kendall correlation matrix */
  static CorrelationMatrix GetCorrelationFromKendallCorrelation(const CorrelationMatrix & matrix);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  // Normal copula parameter
  CorrelationMatrix correlation_;

  // Underlying generic representative
  Normal normal_;

}; /* class NormalCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMALCOPULA_HXX */
