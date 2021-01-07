//                                               -*- C++ -*-
/**
 *  @brief A class that implements a min copula
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MINCOPULA_HXX
#define OPENTURNS_MINCOPULA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MinCopula
 *
 * The class implements a min copula
 */
class OT_API MinCopula
  : public DistributionImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  explicit MinCopula(const UnsignedInteger dim = 2);

  /** Comparison operator */
  Bool operator ==(const MinCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  MinCopula * clone() const override;

  /** Get one realization of the MinCopula distribution */
  Point getRealization() const override;

  /** Get the DDF of the MinCopula distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the MinCopula distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the MinCopula distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the survival function */
  using DistributionImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using DistributionImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const override;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /** Check if the distribution is constinuous */
  Bool isContinuous() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

}; /* class MinCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MINCOPULA_HXX */
