//                                               -*- C++ -*-
/**
 *  @brief The SmoothedUniform distribution
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
#ifndef OPENTURNS_SMOOTHEDUNIFORM_HXX
#define OPENTURNS_SMOOTHEDUNIFORM_HXX

#include "openturns/RandomMixture.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SmoothedUniform
 *
 * The SmoothedUniform distribution.
 */
class OT_API SmoothedUniform
  : public RandomMixture
{
  CLASSNAME
public:

  /** Default constructor */
  SmoothedUniform();

  /** Parameters constructor */
  SmoothedUniform(const Scalar a,
                  const Scalar b,
                  const Scalar sigma);


  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const SmoothedUniform & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual SmoothedUniform * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;
  Sample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the distribution, i.e. the gradient of its PDF w.r.t. point */
  using RandomMixture::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using RandomMixture::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using RandomMixture::computeCDF;
  Scalar computeCDF(const Point & point) const;
  using RandomMixture::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;

  /** Get the gradient of the PDF w.r.t the parameters of the distribution */
  using RandomMixture::computePDFGradient;
  Point computePDFGradient(const Point & point) const;

  /** Get the gradient of the CDF w.r.t the parameters of the distribution */
  using RandomMixture::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to SmoothedUniform */

  /** A accessor */
  void setA(const Scalar a);
  Scalar getA() const;

  /** B accessor */
  void setB(const Scalar b);
  Scalar getB() const;

  /** Sigma accessor */
  void setSigma(const Scalar sigma);
  Scalar getSigma() const;

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

  /** Get the quantile of the distribution, i.e the value Xp such that P(X <= Xp) = prob */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** The main parameter set of the distribution */
  Scalar a_;
  Scalar b_;
  Scalar sigma_;

}; /* class SmoothedUniform */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SMOOTHEDUNIFORM_HXX */
