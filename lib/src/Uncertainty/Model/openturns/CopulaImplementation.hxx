//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all copula
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_COPULAIMPLEMENTATION_HXX
#define OPENTURNS_COPULAIMPLEMENTATION_HXX

#include "OTprivate.hxx"
#include "ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CopulaImplementation
 */
class OT_API CopulaImplementation
  : public ContinuousDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  CopulaImplementation();

  /** Virtual constructor */
  virtual CopulaImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const CopulaImplementation & other) const;

  /** Compute the survival function */
  virtual NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

  /** Get the mean of the copula */
  NumericalPoint getMean() const;

  /** Get the Spearman correlation of the copula */
  CorrelationMatrix getSpearmanCorrelation() const;

  /** Get the Kendall concordance of the copula */
  CorrelationMatrix getKendallTau() const;

  /** Get the standard deviation of the copula */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the copula */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the copula */
  NumericalPoint getKurtosis() const;

  /** Get the i-th marginal copula */
  using ContinuousDistribution::getMarginal;
#ifndef SWIG
  Implementation getMarginal(const UnsignedInteger i) const;
#endif

  /** Get the copula */
  Implementation getCopula() const;

  /** Check if the copula is a copula */
  Bool isCopula() const;

  /** String converter */
  String __repr__() const;


  /* Methods inherited from upper classes */

  /** Generic implementation of the quantile computation for continuous copulas */
  NumericalPoint computeQuantile(const NumericalScalar prob,
                                 const Bool tail = false) const;

protected:

  /** Compute the covariance of the copula */
  void computeCovariance() const;

  /** Compute the mathematical and numerical range of the copula.
      Its mathematical range is the smallest closed interval outside
      of which the PDF is zero, and the numerical range is the interval
      outside of which the PDF is rounded to zero in double precision */
  void computeRange();

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

    NumericalPoint computeDiagonal(const NumericalPoint & u) const
    {
      const NumericalPoint point(dimension_, u[0]);
      const NumericalScalar cdf(p_distribution_->computeCDF(point));
      const NumericalPoint value(1, cdf);
      return value;
    }

    const DistributionImplementation * p_distribution_;
    const UnsignedInteger dimension_;
  }; // struct QuantileWrapper


}; /* class CopulaImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COPULAIMPLEMENTATION_HXX */
