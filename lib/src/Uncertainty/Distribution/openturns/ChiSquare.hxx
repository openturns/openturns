//                                               -*- C++ -*-
/**
 *  @brief The ChiSquare distribution
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
#ifndef OPENTURNS_CHISQUARE_HXX
#define OPENTURNS_CHISQUARE_HXX

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ChiSquare
 *
 * The ChiSquare distribution.
 */
class OT_API ChiSquare
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  ChiSquare();

  /** Parameters constructor */
  explicit ChiSquare(const Scalar nu);


  /** Comparison operator */
  Bool operator ==(const ChiSquare & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  ChiSquare * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution */
  using ContinuousDistribution::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the logarithm of the PDF of the distribution */
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;
  Complex computeLogCharacteristicFunction(const Scalar x) const override;

  /** Get the PDFGradient of the distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  using ContinuousDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

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

  /* Interface specific to ChiSquare */

  /** Nu accessor */
  void setNu(const Scalar nu);
  Scalar getNu() const;

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

  /** Update the derivative attributes */
  void update();

  /** The main parameter set of the distribution */
  Scalar nu_;
  Scalar normalizationFactor_;

}; /* class ChiSquare */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CHISQUARE_HXX */
