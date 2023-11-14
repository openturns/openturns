//                                               -*- C++ -*-
/**
 *  @brief The Dirac distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DIRAC_HXX
#define OPENTURNS_DIRAC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Dirac
 *
 * The Dirac distribution.
 */
class OT_API Dirac
  : public DiscreteDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  Dirac();

  /** Parameters constructor */
  explicit Dirac(const Scalar p);

  /** Parameters constructor */
  explicit Dirac(const Point & point);

  /** Comparison operator */
  using DiscreteDistribution::operator ==;
  Bool operator ==(const Dirac & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Interface inherited from Distribution */

  /** Virtual constructor */
  Dirac * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get a sample of the distribution */
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const override;

  /** Get the PDFGradient of the distribution */
  using DiscreteDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  using DiscreteDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const override;

  /** Compute the characteristic function, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const override;
  Complex computeLogCharacteristicFunction(const Scalar x) const override;

  /** Compute the generating function, i.e. psi(z) = E(z^X) */
  Complex computeGeneratingFunction(const Complex & z) const override;
  Complex computeLogGeneratingFunction(const Complex & z) const override;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DiscreteDistribution::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const override;

  /* Interface specific to Dirac */

  /** Point accessor */
  void setPoint(const Point & point);
  Point getPoint() const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

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

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** The support of the Dirac distribution */
  Point point_;

}; /* class Dirac */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DIRAC_HXX */
