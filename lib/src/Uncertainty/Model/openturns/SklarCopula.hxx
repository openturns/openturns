//                                               -*- C++ -*-
/**
 *  @brief The SklarCopula distribution
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
#ifndef OPENTURNS_SKLARCOPULA_HXX
#define OPENTURNS_SKLARCOPULA_HXX

#include "openturns/CopulaImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SklarCopula
 *
 * The SklarCopula distribution.
 */
class OT_API SklarCopula
  : public CopulaImplementation
{
  CLASSNAME
public:

  typedef PersistentCollection<Distribution>  DistributionPersistentCollection;


  /** Default constructor */
  SklarCopula();

  /** Parameters constructor */
  explicit SklarCopula(const Distribution & distribution);


  /** Comparison operator */
  Bool operator ==(const SklarCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;

  /* Interface inherited from Distribution */
  /** Virtual constructor */
  virtual SklarCopula * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the DDF of the distribution */
  using CopulaImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution */
  using CopulaImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using CopulaImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Compute the survival function */
  using CopulaImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const;

  /** Get the quantile of the distribution */
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x,
                               const Point & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x,
                               const Point & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q,
                                    const Point & y) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Get the standard distribution */
  Distribution getStandardDistribution() const;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameters);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Get the Kendall concordance of the distribution */
  virtual CorrelationMatrix getKendallTau() const;

  /* Interface specific to SklarCopula */

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Compute the covariance of the copula */
  void computeCovariance() const;

  /** The distribution from which the copula is extracted */
  Distribution distribution_;

  /** The marginal distributions of the underlying distribution */
  DistributionPersistentCollection marginalCollection_;

private:

}; /* class SklarCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SKLARCOPULA_HXX */
