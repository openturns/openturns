//                                               -*- C++ -*-
/**
 *  @brief The Fisher-Snedecor distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;
public:

  /** Default constructor */
  FisherSnedecor();

  /** Parameters constructor */
  FisherSnedecor(const NumericalScalar d1,
                 const NumericalScalar d2);


  /** Comparison operator */
  Bool operator ==(const FisherSnedecor & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual FisherSnedecor * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;
  using ContinuousDistribution::computeLogPDF;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;

  // LogPDFGradient
  using ContinuousDistribution::computeLogPDFGradient;
  NumericalPoint computeLogPDFGradient(const NumericalPoint & point) const;

  // PDFGradient
  using ContinuousDistribution::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using ContinuousDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Parameters value accessors */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to FisherSnedecor */

  /** D1 accessor */
  void setD1(const NumericalScalar d1);
  NumericalScalar getD1() const;

  /** D2 accessor */
  void setD2(const NumericalScalar d2);
  NumericalScalar getD2() const;


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Get the quantile of the Triangular distribution */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Update the derivative attributes */
  void update();

  /** The main parameter set of the distribution */
  NumericalScalar d1_;
  NumericalScalar d2_;
  NumericalScalar normalizationFactor_;

}; /* class FisherSnedecor */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FISHERSNEDECOR_HXX */
