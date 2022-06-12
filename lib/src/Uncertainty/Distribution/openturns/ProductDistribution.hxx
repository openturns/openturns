//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  ProductDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Scalar scalar) const override;
  Scalar computePDF(const Point & point) const override;
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
  Scalar computeCDF(const Scalar scalar) const override;
  Scalar computeCDF(const Point & point) const override;
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
  Scalar computeProbability(const Interval & interval) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the raw moments of the distribution */
  Point getMoment(const UnsignedInteger n) const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to ProductDistribution */

  /** Distribution accessor */
  void setLeft(const Distribution & left);
  Distribution getLeft() const;

  /** Distribution accessor */
  void setRight(const Distribution & right);
  Distribution getRight() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

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

  /** The main parameter set of the distribution */
  Pointer<DistributionImplementation> p_left_;
  Pointer<DistributionImplementation> p_right_;

  /** Integration algorithm */
  GaussKronrod algo_;

}; /* class ProductDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTDISTRIBUTION_HXX */
