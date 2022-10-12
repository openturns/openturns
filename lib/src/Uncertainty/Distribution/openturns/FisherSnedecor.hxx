//                                               -*- C++ -*-
/**
 *  @brief The Fisher-Snedecor distribution
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
#ifndef OPENTURNS_FISHERSNEDECOR_HXX
#define OPENTURNS_FISHERSNEDECOR_HXX

#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FisherSnedecor
 *
 * The Fisher-Snedecor distribution.
 */
class OT_API FisherSnedecor
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  FisherSnedecor();

  /** Parameters constructor */
  FisherSnedecor(const Scalar d1,
                 const Scalar d2);


  /** Comparison operator */
  Bool operator ==(const FisherSnedecor & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Interface inherited from Distribution */

  /** Virtual constructor */
  FisherSnedecor * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  // LogPDFGradient
  using ContinuousDistribution::computeLogPDFGradient;
  Point computeLogPDFGradient(const Point & point) const override;

  // PDFGradient
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to FisherSnedecor */

  /** D1 accessor */
  void setD1(const Scalar d1);
  Scalar getD1() const;

  /** D2 accessor */
  void setD2(const Scalar d2);
  Scalar getD2() const;


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Get the quantile of the Triangular distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Update the derivative attributes */
  void update();

  /** The main parameter set of the distribution */
  Scalar d1_;
  Scalar d2_;
  Scalar normalizationFactor_;

}; /* class FisherSnedecor */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FISHERSNEDECOR_HXX */
