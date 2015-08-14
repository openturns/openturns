//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Parametrized distributions
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
#ifndef OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX
#define OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX

#include "DistributionImplementation.hxx"
#include "DistributionParameters.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ParametrizedDistribution
 *
 * A subclass for Parametrized distributions.
 */
class OT_API ParametrizedDistribution
  : public DistributionImplementation
{

  CLASSNAME;
public:

  /** Default constructor */
  ParametrizedDistribution();

  /** Constructor with parameter */
  ParametrizedDistribution(const DistributionParameters & distParam);

  /** Virtual constructor */
  virtual ParametrizedDistribution * clone() const;

  /** Comparison operator */
  Bool operator ==(const ParametrizedDistribution & other) const;

  /** Get the DDF of the distribution */
  using DistributionImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;
  using DistributionImplementation::computeLogPDF;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the CDF of the distributionImplementation */
  using DistributionImplementation::computeComplementaryCDF;
  virtual NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;
  NumericalComplex computeLogCharacteristicFunction(const NumericalScalar x) const;

  /** Get the PDFGradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDFGradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Get the quantile of the distributionImplementation */
  virtual NumericalPoint computeQuantile(const NumericalScalar prob,
                                         const Bool tail = false) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /** Check if the distribution is elliptical */
  virtual Bool isElliptical() const;

  /** Check if the distribution is continuous */
  virtual Bool isContinuous() const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  virtual NumericalScalar getRoughness() const;

  /** Get the mean of the distribution */
  virtual NumericalPoint getMean() const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Implementation getStandardRepresentative() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  DistributionParameters distributionParameters_;
  Distribution distribution_;

}; /* class ParametrizedDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PARAMETRIZEDDISTRIBUTION_HXX */
