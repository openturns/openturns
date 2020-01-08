//                                               -*- C++ -*-
/**
 *  @brief The GeneralizedExtremeValue distribution
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
#ifndef OPENTURNS_GENERALIZEDEXTREMEVALUE_HXX
#define OPENTURNS_GENERALIZEDEXTREMEVALUE_HXX

#include "openturns/WeibullMax.hxx"
#include "openturns/Frechet.hxx"
#include "openturns/Gumbel.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralizedExtremeValue
 *
 * The GeneralizedExtremeValue distribution.
 */
class OT_API GeneralizedExtremeValue
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  GeneralizedExtremeValue();

  /** Parameters constructor to use when the two bounds are finite */
  GeneralizedExtremeValue(const Scalar mu,
                          const Scalar sigma,
                          const Scalar xi);

  /** Parameter constructor from Weibull/Frechet/Gumbel */
  GeneralizedExtremeValue(const Distribution & distribution);

  /** Comparison operator */
  Bool operator ==(const GeneralizedExtremeValue & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual GeneralizedExtremeValue * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;
  using DistributionImplementation::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;
  Complex computeLogCharacteristicFunction(const Scalar x) const;

  /** Get the PDFGradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to GeneralizedExtremeValue */

  /** Mu accessor */
  void setMu(const Scalar mu);
  Scalar getMu() const;

  /** Sigma bound accessor */
  void setSigma(const Scalar sigma);
  Scalar getSigma() const;

  /** Xi accessor */
  void setXi(const Scalar xi);
  Scalar getXi() const;

  /** Actual distribution accessor */
  void setActualDistribution(const Distribution & distribution);
  Distribution getActualDistribution() const;

  /** Actual distribution converter */
  Frechet asFrechet() const;
  WeibullMax asWeibullMax() const;
  Gumbel asGumbel() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Set the whole set of parameters with check */
  void setMuSigmaXi(const Scalar mu,
                    const Scalar sigma,
                    const Scalar xi);

  /** The actual distribution in {WeibullMin, Frechet, Gumbel} */
  Distribution actualDistribution_;
  Scalar mu_;
  Scalar sigma_;
  Scalar xi_;
}; /* class GeneralizedExtremeValue */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GENERALIZEDEXTREMEVALUE_HXX */
