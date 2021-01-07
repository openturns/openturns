//                                               -*- C++ -*-
/**
 *  @brief The Trapezoidal distribution
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
#ifndef OPENTURNS_TRAPEZOIDAL_HXX
#define OPENTURNS_TRAPEZOIDAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Trapezoidal
 *
 * The Trapezoidal distribution.
 */
class OT_API Trapezoidal
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  Trapezoidal();

  /** Parameters constructor */
  Trapezoidal(const Scalar a,
              const Scalar b,
              const Scalar c,
              const Scalar d);

  /** Comparison operator */
  Bool operator ==(const Trapezoidal & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  Trapezoidal * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution, i.e. the gradient of its PDF w.r.t. point */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;

  /** Get the gradient of the PDF w.r.t the parameters of the distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the gradient of the logPDF w.r.t the parameters of the distribution */
  using ContinuousDistribution::computeLogPDFGradient;
  Point computeLogPDFGradient(const Point & point) const override;

  /** Get the gradient of the CDF w.r.t the parameters of the distribution */
  using ContinuousDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

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

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const override;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to Trapezoidal */

  /** A accessor */
  void setA(const Scalar a);
  Scalar getA() const;

  /** B accessor */
  void setB(const Scalar b);
  Scalar getB() const;

  /** C accessor */
  void setC(const Scalar c);
  Scalar getC() const;

  /** D accessor */
  void setD(const Scalar d);
  Scalar getD() const;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Get the quantile of the distribution, i.e the value Xp such that P(X <= Xp) = prob */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Update attributes */
  void update();

  /** Sigma accessor */
  Scalar getSigma() const;

  /** The main parameter set of the distribution */
  Scalar a_;
  Scalar b_;
  Scalar c_;
  Scalar d_;
  Scalar h_;

}; /* class Trapezoidal */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRAPEZOIDAL_HXX */
