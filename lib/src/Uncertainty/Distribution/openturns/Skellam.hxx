//                                               -*- C++ -*-
/**
 *  @brief The Skellam distribution
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
#ifndef OPENTURNS_SKELLAM_HXX
#define OPENTURNS_SKELLAM_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Skellam
 *
 * The Skellam distribution.
 */
class OT_API Skellam
  : public DiscreteDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  Skellam();

  /** Parameters constructor */
  Skellam(const NumericalScalar lambda1,
          const NumericalScalar lambda2);


  /** Comparison operator */
  Bool operator ==(const Skellam & other) const;
 protected:
  Bool equals(const DistributionImplementation & other) const;
 public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual Skellam * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the PDFGradient of the distribution */
  using DiscreteDistribution::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDFGradient of the distribution */
  using DiscreteDistribution::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Compute the characteristic function, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;
  NumericalComplex computeLogCharacteristicFunction(const NumericalScalar x) const;

  /** Compute the generating function, i.e. psi(z) = E(z^X) */
  NumericalComplex computeGeneratingFunction(const NumericalComplex & z) const;
  NumericalComplex computeLogGeneratingFunction(const NumericalComplex & z) const;

  /** Get the quantile of the distribution */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  NumericalSample getSupport(const Interval & interval) const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Parameters value accessors */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to Skellam */

  /** Lambda1/Lambda2 accessor */
  void setLambda1Lambda2(const NumericalScalar lambda1,
                         const NumericalScalar lambda2);

  /** Lambda1 accessor */
  void setLambda1(const NumericalScalar lambda1);
  NumericalScalar getLambda1() const;

  /** Lambda2 accessor */
  void setLambda2(const NumericalScalar lambda2);
  NumericalScalar getLambda2() const;

  /** Maximum iterations accessor */
  void setMaximumIteration(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumIteration() const;

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

  /** The Lambda1 of the Skellam distribution */
  NumericalScalar lambda1_;

  /** The Lambda2 of the Skellam distribution */
  NumericalScalar lambda2_;

  /** The maximum iterations for the PDF and CDF computations */
  UnsignedInteger maximumIteration_;

}; /* class Skellam */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SKELLAM_HXX */
