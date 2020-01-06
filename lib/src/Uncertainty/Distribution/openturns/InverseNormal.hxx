//                                               -*- C++ -*-
/**
 *  @brief The InverseNormal distribution
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
  Bool operator ==(const InverseNormal & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual InverseNormal * clone() const;

  /** Get one realization of the InverseNormal distribution */
  Point getRealization() const;

  /** Get the PDF of the InverseNormal distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the InverseNormal distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the minimum volume level set containing a given probability of the distribution */
  LevelSet computeMinimumVolumeLevelSetWithThreshold(const Scalar prob, Scalar & thresholdOut) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;
  Complex computeLogCharacteristicFunction(const Scalar x) const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const;

  /** @deprecated Interface specific to InverseNormal */
  void setLambdaMu(const Scalar lambda,
                   const Scalar mu);

  /** Interface specific to InverseNormal */
  void setMuLambda(const Scalar mu,
                   const Scalar lambda);

  /** lambda accessor */
  Scalar getLambda() const;

  /** mu accessor */
  Scalar getMu() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The main parameter set of the distribution */
  Scalar mu_;
  Scalar lambda_;

}; /* class InverseNormal */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENORMAL_HXX */
