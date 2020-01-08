//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
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
#ifndef OPENTURNS_PRODUCTDISTRIBUTION_HXX
#define OPENTURNS_PRODUCTDISTRIBUTION_HXX

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProductDistribution
 *
 * The ProductDistribution distribution.
 */
class OT_API ProductDistribution
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  ProductDistribution();

  /** Parameters constructor to use when the two bounds are finite */
  ProductDistribution(const Distribution & left,
                      const Distribution & right);

  /** Comparison operator */
  Bool operator ==(const ProductDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual ProductDistribution * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Scalar scalar) const;
  Scalar computePDF(const Point & point) const;
private:
  Scalar computePDFQ1(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
  Scalar computePDFQ2(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
  Scalar computePDFQ3(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
  Scalar computePDFQ4(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
public:
  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Scalar scalar) const;
  Scalar computeCDF(const Point & point) const;
private:
  Scalar computeCDFQ1(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
  Scalar computeCDFQ2(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
  Scalar computeCDFQ3(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
  Scalar computeCDFQ4(const Scalar x,
                      const Scalar a,
                      const Scalar b,
                      const Scalar c,
                      const Scalar d) const;
public:

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the raw moments of the distribution */
  Point getMoment(const UnsignedInteger n) const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to ProductDistribution */

  /** Distribution accessor */
  void setLeft(const Distribution & left);
  Distribution getLeft() const;

  /** Distribution accessor */
  void setRight(const Distribution & right);
  Distribution getRight() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The main parameter set of the distribution */
  Pointer<DistributionImplementation> p_left_;
  Pointer<DistributionImplementation> p_right_;

  /** Integration algorithm */
  GaussKronrod algo_;

}; /* class ProductDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTDISTRIBUTION_HXX */
