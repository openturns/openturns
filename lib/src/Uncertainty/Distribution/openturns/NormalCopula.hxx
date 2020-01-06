//                                               -*- C++ -*-
/**
 *  @brief A class that implements a normal copula
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
#ifndef OPENTURNS_NORMALCOPULA_HXX
#define OPENTURNS_NORMALCOPULA_HXX

#include "openturns/CopulaImplementation.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NormalCopula
 *
 * The class implements a normal copula
 */
class OT_API NormalCopula
  : public CopulaImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  explicit NormalCopula(const UnsignedInteger dim = 2);

  /** Default constructor */
  explicit NormalCopula(const CorrelationMatrix & correlation);


  /** Comparison operator */
  Bool operator ==(const NormalCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual NormalCopula * clone() const;

  /** Get one realization of the NormalCopula distribution */
  Point getRealization() const;
protected:
  Sample getSampleParallel(const UnsignedInteger size) const;
public:
  Sample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the NormalCopula distribution */
  using CopulaImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the NormalCopula distribution */
  using CopulaImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the NormalCopula distribution */
  using CopulaImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;
  using CopulaImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Get the shape matrix of the copula */
  CorrelationMatrix getShapeMatrix() const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the PDF gradient of the distribution */
  Point computePDFGradient(const Point & point) const;

  /** Get the CDF gradient of the distribution */
  Point computeCDFGradient(const Point & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x,
                               const Point & y) const;
  Point computeConditionalPDF(const Point & x,
                              const Sample & y) const;
  Point computeSequentialConditionalPDF(const Point & x) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x,
                               const Point & y) const;
  Point computeSequentialConditionalCDF(const Point & x) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q,
                                    const Point & y) const;
  Point computeSequentialConditionalQuantile(const Point & q) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Parameters value and description accessor */
  using CopulaImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection);

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Compute the correlation matrix of a Normal Copula from its Spearman correlation matrix */
  static CorrelationMatrix GetCorrelationFromSpearmanCorrelation(const CorrelationMatrix & matrix);

  /** Compute the correlation matrix of a Normal Copula from its Kendall correlation matrix */
  static CorrelationMatrix GetCorrelationFromKendallCorrelation(const CorrelationMatrix & matrix);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:
  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  // Normal copula parameter
  CorrelationMatrix correlation_;

  // Underlying generic representative
  Normal normal_;

}; /* class NormalCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMALCOPULA_HXX */
