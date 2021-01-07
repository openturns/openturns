//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all copula
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
#ifndef OPENTURNS_COPULAIMPLEMENTATION_HXX
#define OPENTURNS_COPULAIMPLEMENTATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CopulaImplementation
 */
class OT_API CopulaImplementation
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CopulaImplementation();

  /** Virtual constructor */
  CopulaImplementation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const CopulaImplementation & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** Compute the survival function */
  Scalar computeSurvivalFunction(const Point & point) const override;

  /** Get the mean of the copula */
  Point getMean() const override;

  /** Get the Spearman correlation of the copula */
  CorrelationMatrix getSpearmanCorrelation() const override;

  /** Get the standard deviation of the copula */
  Point getStandardDeviation() const override;

  /** Get the skewness of the copula */
  Point getSkewness() const override;

  /** Get the kurtosis of the copula */
  Point getKurtosis() const override;

  /** Get the i-th marginal copula */
  using DistributionImplementation::getMarginal;
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the copula */
  Distribution getCopula() const override;

  /** String converter */
  String __repr__() const override;


  /* Methods inherited from upper classes */

  /** Generic implementation of the quantile computation for continuous copulas */
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const override;

protected:

  /** Compute the covariance of the copula */
  void computeCovariance() const override;

  /** Compute the mathematical and numerical range of the copula.
      Its mathematical range is the smallest closed interval outside
      of which the PDF is zero, and the numerical range is the interval
      outside of which the PDF is rounded to zero in double precision */
  void computeRange() override;

private:

  // Structure used to implement the computeQuantile() method efficiently
  struct QuantileWrapper
  {
    QuantileWrapper(const DistributionImplementation * p_distribution)
      : p_distribution_(p_distribution)
      , dimension_(p_distribution->getDimension())
    {
      // Nothing to do
    }

    Point computeDiagonal(const Point & u) const
    {
      const Point point(dimension_, u[0]);
      const Scalar cdf = p_distribution_->computeCDF(point);
      const Point value(1, cdf);
      return value;
    }

    const DistributionImplementation * p_distribution_;
    const UnsignedInteger dimension_;
  }; // struct QuantileWrapper


}; /* class CopulaImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COPULAIMPLEMENTATION_HXX */
