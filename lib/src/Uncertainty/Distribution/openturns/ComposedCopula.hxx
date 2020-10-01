//                                               -*- C++ -*-
/**
 *  @brief The class that implements assembly distributions
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
#ifndef OPENTURNS_COMPOSEDCOPULA_HXX
#define OPENTURNS_COMPOSEDCOPULA_HXX

#include "openturns/CopulaImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComposedCopula
 *
 * The class describes the probabilistic concept of copulas
 * made from a collection of copulas joined by an independent copula
 */
class OT_API ComposedCopula
  : public CopulaImplementation
{
  CLASSNAME
public:

  /** A type for distribution collection */
  typedef Collection<Distribution>                 DistributionCollection;
  typedef PersistentCollection<Distribution>       DistributionPersistentCollection;

  /** Default constructor for save/load methods : 1D distribution with default Uniform marginal and IndependentCopula */
  ComposedCopula();

  /** Default constructor */
  explicit ComposedCopula(const DistributionCollection & coll);

  /** Comparison operator */
  Bool operator ==(const ComposedCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /** Copula collection accessor */
  void setCopulaCollection(const DistributionCollection & coll);
  DistributionCollection getCopulaCollection() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  ComposedCopula * clone() const override;

  /** Get one realization of the ComposedCopula */
  Point getRealization() const override;

  /** Get the DDF of the ComposedCopula */
  using CopulaImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the ComposedCopula */
  using CopulaImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the log-PDF of the ComposedCopula */
  using CopulaImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the ComposedCopula */
  using CopulaImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Compute the survival function */
  using CopulaImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const override;

  /** Get the shape matrix of the distribution, ie the correlation matrix
      of its copula if it is elliptical */
  CorrelationMatrix getShapeMatrix() const override;

  /** Get the PDF gradient of the distribution */
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDF gradient of the distribution */
  Point computeCDFGradient(const Point & point) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalPDF(const Point & x) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;
  Point computeSequentialConditionalCDF(const Point & x) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;
  Point computeSequentialConditionalQuantile(const Point & q) const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using CopulaImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & setParametersCollection) override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Get the isoprobabilistic transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const override;

  /** Get the inverse isoprobabilistic transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


private:

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** The collection of copules of the ComposedCopula */
  DistributionPersistentCollection copulaCollection_;

  /** Flag to tell if the copula is independent */
  Bool isIndependent_;

}; /* class ComposedCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDCOPULA_HXX */
