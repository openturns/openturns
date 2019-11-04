//                                               -*- C++ -*-
/**
 *  @brief A class that implements a min copula
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
#ifndef OPENTURNS_MINCOPULA_HXX
#define OPENTURNS_MINCOPULA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/CopulaImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MinCopula
 *
 * The class implements a min copula
 */
class OT_API MinCopula
  : public CopulaImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  explicit MinCopula(const UnsignedInteger dim = 2);

  /** Comparison operator */
  Bool operator ==(const MinCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual MinCopula * clone() const;

  /** Get one realization of the MinCopula distribution */
  Point getRealization() const;

  /** Get the DDF of the MinCopula distribution */
  using CopulaImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the MinCopula distribution */
  using CopulaImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the MinCopula distribution */
  using CopulaImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const;

  /** Compute the survival function */
  using CopulaImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** Check if the distribution is constinuous */
  Bool isContinuous() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

private:

  /** Compute the covariance of the distribution */
  virtual void computeCovariance() const;

}; /* class MinCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MINCOPULA_HXX */
