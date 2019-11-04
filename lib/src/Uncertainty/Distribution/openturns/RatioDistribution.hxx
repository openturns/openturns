//                                               -*- C++ -*-
/**
 *  @brief The RatioDistribution distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_RATIODISTRIBUTION_HXX
#define OPENTURNS_RATIODISTRIBUTION_HXX

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RatioDistribution
 *
 * The RatioDistribution distribution.
 */
class OT_API RatioDistribution
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  RatioDistribution();

  /** Parameters constructor to use when the two bounds are finite */
  RatioDistribution(const Distribution & left,
                    const Distribution & right);

  /** Comparison operator */
  Bool operator ==(const RatioDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual RatioDistribution * clone() const;

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
  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to RatioDistribution */

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

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:

  // Structure used to wrap the kernel of the integral defining the ratio
  struct PDFKernelWrapper
  {
    PDFKernelWrapper(const Distribution & left,
                     const Distribution & right,
                     const Scalar x):
      left_(left), right_(right), x_(x), isZero_(std::abs(x) == 0.0), pdf0_(isZero_ ? right.computePDF(0.0) : 0.0) {};

    // Compute |u|p_left(u)p_right(ux) where x is the argument of the pdf of left*right
    Point eval(const Point & point) const
    {
      const Scalar u = point[0];
      const Scalar absU = std::abs(u);
      // First special case: |u|==0
      if (absU == 0.0) return Point(1, 0.0);
      // Secon special case: x==0
      if (isZero_)
      {
        if (pdf0_ == 0.0) return Point(1, 0.0);
        return Point(1, absU * pdf0_ * left_.computePDF(point));
      }
      const Scalar value = left_.computePDF(point);
      // If the given point is outside of the support of left
      if (value == 0.0) return Point(1, 0.0);
      return Point(1, absU * right_.computePDF(u * x_) * value);
    };

    const Distribution left_;
    const Distribution right_;
    const Scalar x_;
    const Bool isZero_;
    const Scalar pdf0_;
  }; // struct PDFKernelWrapper

  // Structure used to wrap the kernel of the integral defining the ratio
  struct CFKernelWrapper
  {
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

    const Distribution left_;
    const Distribution right_;
    const Scalar x_;
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

}; /* class RatioDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RATIODISTRIBUTION_HXX */
