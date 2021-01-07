//                                               -*- C++ -*-
/**
 *  @brief The Hypergeometric distribution
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
#ifndef OPENTURNS_HYPERGEOMETRIC_HXX
#define OPENTURNS_HYPERGEOMETRIC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Hypergeometric
 *
 * The Hypergeometric distribution.
 */
class OT_API Hypergeometric
  : public DiscreteDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  Hypergeometric();

  /** Parameters constructor */
  Hypergeometric(const UnsignedInteger n,
                 const UnsignedInteger k,
                 const UnsignedInteger m);

  /** Comparison operator */
  Bool operator ==(const Hypergeometric & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  Hypergeometric * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DiscreteDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using DiscreteDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Get the PDFGradient of the distribution */
  using DiscreteDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  using DiscreteDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const override;

  /** Get the discrete probability levels */
  Point getProbabilities() const override;

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

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to Hypergeometric */

  /** K accessor */
  void setK(const UnsignedInteger h);
  UnsignedInteger getK() const;

  /** M accessor */
  void setM(const UnsignedInteger m);
  UnsignedInteger getM() const;

  /** N accessor */
  void setN(const UnsignedInteger n);
  UnsignedInteger getN() const;

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

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Compute the probability table of the distribution */
  void computeProbabilities();

  /** The population size */
  UnsignedInteger n_;

  /** The number of candidates in the population */
  UnsignedInteger k_;

  /** The size of the sample */
  UnsignedInteger m_;

  /** The probability table of the distribution */
  Point probabilities_;

}; /* class Hypergeometric */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HYPERGEOMETRIC_HXX */
