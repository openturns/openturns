//                                               -*- C++ -*-
/**
 *  @brief The SklarCopula distribution
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
#ifndef OPENTURNS_SKLARCOPULA_HXX
#define OPENTURNS_SKLARCOPULA_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SklarCopula
 *
 * The SklarCopula distribution.
 */
class OT_API SklarCopula
  : public DistributionImplementation
{
  CLASSNAME
public:

  typedef PersistentCollection<Distribution>  DistributionPersistentCollection;


  /** Default constructor */
  SklarCopula();

  /** Parameters constructor */
  explicit SklarCopula(const Distribution & distribution);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const SklarCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;

  /* Interface inherited from Distribution */
  /** Virtual constructor */
  SklarCopula * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Compute the survival function */
  using DistributionImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const override;

  /** Get the quantile of the distribution */
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x,
                               const Point & y) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x,
                               const Point & y) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q,
                                    const Point & y) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using DistributionImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const override;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const override;

  /** Get the standard distribution */
  Distribution getStandardDistribution() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameters) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const override;

  /* Interface specific to SklarCopula */

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Compute the covariance of the copula */
  void computeCovariance() const override;

  /** The distribution from which the copula is extracted */
  Distribution distribution_;

  /** The marginal distributions of the underlying distribution */
  DistributionPersistentCollection marginalCollection_;

private:

}; /* class SklarCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SKLARCOPULA_HXX */
