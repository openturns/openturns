//                                               -*- C++ -*-
/**
 *  @brief The PiecewiseLinearDistribution distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PIECEWISELINEARDISTRIBUTION_HXX
#define OPENTURNS_PIECEWISELINEARDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PiecewiseLinearDistribution
 *
 * The PiecewiseLinearDistribution distribution.
 */
class OT_API PiecewiseLinearDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PiecewiseLinearDistribution();

  /** Parameters constructor */
  PiecewiseLinearDistribution(const Point & x,
                              const Point & y);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const PiecewiseLinearDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  PiecewiseLinearDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Scalar scalar) const override;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Scalar scalar) const override;
  Scalar computeCDF(const Point & point) const override;

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  Scalar getRoughness() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to PiecewiseLinearDistribution */

  /** X accessor */
  void setX(const Point & x);
  Point getX() const;

  /** Y accessor */
  void setY(const Point & y);
  Point getY() const;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Update precomputed quantities from x_ and y_ */
  void update();

  /** The main parameter set of the distribution */
  Point x_;
  Point y_;

  /** Normalized y values used for computation */
  Point yNorm_;

  /** Precomputed segment integrals */
  Point probabilities_;

}; /* class PiecewiseLinearDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PIECEWISELINEARDISTRIBUTION_HXX */
