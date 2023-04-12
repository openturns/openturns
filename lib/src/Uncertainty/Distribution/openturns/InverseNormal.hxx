//                                               -*- C++ -*-
/**
 *  @brief The InverseNormal distribution
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
#ifndef OPENTURNS_INVERSENORMAL_HXX
#define OPENTURNS_INVERSENORMAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNormal
 *
 * The InverseNormal distribution.
 */
class OT_API InverseNormal
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  InverseNormal();

  /** Parameters constructor */
  InverseNormal(const Scalar mu,
                const Scalar lambda);


  /** Comparison operator */
  using ContinuousDistribution::operator ==;
  Bool operator ==(const InverseNormal & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Interface inherited from Distribution */

  /** Virtual constructor */
  InverseNormal * clone() const override;

  /** Get one realization of the InverseNormal distribution */
  Point getRealization() const override;

  /** Get the PDF of the InverseNormal distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the InverseNormal distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;
  Complex computeLogCharacteristicFunction(const Scalar x) const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Interface specific to InverseNormal */
  void setMuLambda(const Scalar mu,
                   const Scalar lambda);

  /** lambda accessor */
  Scalar getLambda() const;

  /** mu accessor */
  Scalar getMu() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** The main parameter set of the distribution */
  Scalar mu_;
  Scalar lambda_;

}; /* class InverseNormal */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENORMAL_HXX */
