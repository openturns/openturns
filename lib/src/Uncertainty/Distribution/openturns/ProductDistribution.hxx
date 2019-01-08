//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

  // Structure used to wrap the kernel of the integral defining the PDF of the product
  struct PDFKernelWrapper
  {
    const Distribution left_;
    const Distribution right_;
    const Scalar x_;
    const Bool isZero_;
    const Scalar pdf0_;

    PDFKernelWrapper(const Distribution & left,
                     const Distribution & right,
                     const Scalar x):
      left_(left), right_(right), x_(x), isZero_(std::abs(x) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon")), pdf0_(isZero_ ? right.computePDF(0.0) : 0.0) {};

    Point eval(const Point & point) const
    {
      const Scalar value = left_.computePDF(point);
      if (value == 0.0) return Point(1, 0.0);
      const Scalar u = point[0];
      const Scalar absU = std::abs(u);
      // x_ == 0
      if (isZero_)
      {
        if (pdf0_ == 0.0) return Point(1, 0.0);
        if (absU == 0.0) return Point(1, SpecFunc::MaxScalar);
        return Point(1, value * pdf0_ / absU);
      }
      // x_ != 0
      if (absU == 0.0)
      {
        const Scalar epsilon = 1e-7;
        return Point(1, value * 0.5 * (right_.computePDF(x_ / epsilon) + right_.computePDF(-x_ / epsilon)) / epsilon);
      }
      return Point(1, value * right_.computePDF(x_ / u) / absU);
    };
  }; // struct PDFKernelWrapper

  // Structure used to wrap the kernel of the integral defining the CDF of the product
  struct CDFKernelWrapper
  {
    const Distribution left_;
    const Distribution right_;
    const Scalar x_;
    const Bool isZero_;
    const Scalar cdf0_;
    const Scalar ccdf0_;

    CDFKernelWrapper(const Distribution & left,
                     const Distribution & right,
                     const Scalar x):
      left_(left), right_(right), x_(x), isZero_(std::abs(x) == 0.0), cdf0_(isZero_ ? right.computeCDF(0.0) : 0.0), ccdf0_(isZero_ ? right.computeComplementaryCDF(0.0) : 0.0) {};

    Point eval(const Point & point) const
    {
      const Scalar value = left_.computePDF(point);
      if (value == 0.0) return Point(1, 0.0);
      // x_ == 0
      if (isZero_) return Point(1, value * cdf0_);
      const Scalar u = point[0];
      if (u == 0.0) return Point(1, x_ < 0.0 ? 0.0 : value);
      return Point(1, value * right_.computeCDF(x_ / u));
    };

    Point evalComplementary(const Point & point) const
    {
      const Scalar value = left_.computePDF(point);
      if (value == 0.0) return Point(1, 0.0);
      // x_ == 0
      if (isZero_) return Point(1, value * ccdf0_);
      const Scalar u = point[0];
      if (u == 0.0) return Point(1, x_ < 0.0 ? 0.0 : value);
      return Point(1, value * right_.computeComplementaryCDF(x_ / u));
    };

  }; // struct CDFKernelWrapper

  // Structure used to wrap the kernel of the integral defining the product
  struct CFKernelWrapper
  {
    const Distribution left_;
    const Distribution right_;
    const Scalar x_;

    CFKernelWrapper(const Distribution & left,
                    const Distribution & right,
                    const Scalar x):
      left_(left), right_(right), x_(x) {};

    Point eval(const Point & point) const
    {
      Point value(2);
      const Scalar u = point[0];
      const Complex phi(right_.computeCharacteristicFunction(u * x_));
      const Scalar pdf = left_.computePDF(point);
      value[0] = pdf * phi.real();
      value[1] = pdf * phi.imag();
      return value;
    };
  }; // struct CFKernelWrapper

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The main parameter set of the distribution */
  Distribution left_;
  Distribution right_;

}; /* class ProductDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTDISTRIBUTION_HXX */
