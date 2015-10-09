//                                               -*- C++ -*-
/**
 *  @brief The ProductDistribution distribution
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
#ifndef OPENTURNS_PRODUCTDISTRIBUTION_HXX
#define OPENTURNS_PRODUCTDISTRIBUTION_HXX

#include "ContinuousDistribution.hxx"
#include "Distribution.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProductDistribution
 *
 * The ProductDistribution distribution.
 */
class OT_API ProductDistribution
  : public ContinuousDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  ProductDistribution();

  /** Parameters constructor to use when the two bounds are finite */
  ProductDistribution(const Distribution & left,
                      const Distribution & right);

  /** Comparison operator */
  Bool operator ==(const ProductDistribution & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual ProductDistribution * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;
private:
  NumericalScalar computePDFQ1(const NumericalScalar x,
                               const NumericalScalar a,
                               const NumericalScalar b,
                               const NumericalScalar c,
                               const NumericalScalar d) const;
  NumericalScalar computePDFQ2(const NumericalScalar x,
                               const NumericalScalar a,
                               const NumericalScalar b,
                               const NumericalScalar c,
                               const NumericalScalar d) const;
  NumericalScalar computePDFQ3(const NumericalScalar x,
                               const NumericalScalar a,
                               const NumericalScalar b,
                               const NumericalScalar c,
                               const NumericalScalar d) const;
  NumericalScalar computePDFQ4(const NumericalScalar x,
                               const NumericalScalar a,
                               const NumericalScalar b,
                               const NumericalScalar c,
                               const NumericalScalar d) const;
public:
  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using ContinuousDistribution::setParameters;
  void setParameters(const NumericalPoint & parameters);

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

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:

  // Structure used to wrap the kernel of the integral defining the product
  struct PDFKernelWrapper
  {
    PDFKernelWrapper(const Distribution & left,
                     const Distribution & right,
                     const NumericalScalar x):
      left_(left), right_(right), x_(x), isZero_(std::abs(x) == 0.0), pdf0_(isZero_ ? right.computePDF(0.0) : 0.0) {};

    NumericalPoint eval(const NumericalPoint & point) const
    {
      const NumericalScalar value(left_.computePDF(point));
      if (value == 0.0) return NumericalPoint(1, 0.0);
      const NumericalScalar u(point[0]);
      const NumericalScalar absU(std::abs(u));
      // x_ == 0
      if (isZero_)
      {
        if (pdf0_ == 0.0) return NumericalPoint(1, 0.0);
        if (absU == 0.0) return NumericalPoint(1, SpecFunc::MaxNumericalScalar);
        return NumericalPoint(1, value * pdf0_ / absU);
      }
      // x_ != 0
      if (absU == 0.0)
      {
        const NumericalScalar epsilon(1e-7);
        return NumericalPoint(1, value * 0.5 * (right_.computePDF(x_ / epsilon) + right_.computePDF(-x_ / epsilon)) / epsilon);
      }
      return NumericalPoint(1, value * right_.computePDF(x_ / u) / absU);
    };

    const Distribution left_;
    const Distribution right_;
    const NumericalScalar x_;
    const Bool isZero_;
    const NumericalScalar pdf0_;
  }; // struct PDFKernelWrapper

  // Structure used to wrap the kernel of the integral defining the product
  struct CFKernelWrapper
  {
    CFKernelWrapper(const Distribution & left,
                    const Distribution & right,
                    const NumericalScalar x):
      left_(left), right_(right), x_(x) {};

    NumericalPoint eval(const NumericalPoint & point) const
    {
      NumericalPoint value(2);
      const NumericalScalar u(point[0]);
      const NumericalComplex phi(right_.computeCharacteristicFunction(u * x_));
      const NumericalScalar pdf(left_.computePDF(point));
      value[0] = pdf * phi.real();
      value[1] = pdf * phi.imag();
      return value;
    };

    const Distribution left_;
    const Distribution right_;
    const NumericalScalar x_;
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
