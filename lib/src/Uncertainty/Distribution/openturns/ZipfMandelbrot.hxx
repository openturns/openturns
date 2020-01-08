//                                               -*- C++ -*-
/**
 *  @brief The ZipfMandelbrot distribution
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
#ifndef OPENTURNS_ZIPFMANDELBROT_HXX
#define OPENTURNS_ZIPFMANDELBROT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ZipfMandelbrot
 *
 * The ZipfMandelbrot distribution.
 */
class OT_API ZipfMandelbrot
  : public DiscreteDistribution
{
  CLASSNAME
public:

  typedef Collection<Scalar> ScalarCollection;

  /** Default constructor */
  ZipfMandelbrot();

  /** Parameters constructor */
  ZipfMandelbrot(const UnsignedInteger n,
                 const Scalar q,
                 const Scalar s);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const ZipfMandelbrot & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual ZipfMandelbrot * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to ZipfMandelbrot */

  /** Q accessor */
  void setQ(const Scalar q);
  Scalar getQ() const;

  /** S accessor */
  void setS(const Scalar s);
  Scalar getS() const;

  /** N accessor */
  void setN(const UnsignedInteger n);
  UnsignedInteger getN() const;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const;

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

  /* Compute generalized harmonic numbers */
  Scalar getHarmonicNumbers(UnsignedInteger const) const;
  void computeHarmonicNumbers();

  ScalarCollection harmonicNumbers_;


private:

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The maximum value of the ZipfMandelbrot distribution */
  UnsignedInteger n_;

  /** The q probability parameter of the ZipfMandelbrot distribution */
  Scalar q_;

  /** The s probability parameter of the ZipfMandelbrot distribution */
  Scalar s_;

}; /* class ZipfMandelbrot */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ZIPFMANDELBROT_HXX */
