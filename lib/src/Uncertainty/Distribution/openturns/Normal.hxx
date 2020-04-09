//                                               -*- C++ -*-
/**
 *  @brief The Normal distribution
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
#ifndef OPENTURNS_NORMAL_HXX
#define OPENTURNS_NORMAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/EllipticalDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Normal
 *
 * The Normal (gaussian) distribution.
 */
class OT_API Normal
  : public EllipticalDistribution
{

  CLASSNAME
public:

  /** Default constructor */
  explicit Normal(const UnsignedInteger dimension = 1);

  /** Dimension 1 constructor */
  Normal(const Scalar mu, const Scalar sd);

  /** Constructor for multiD normal distribution */
  Normal(const Point & mean,
         const Point & sigma,
         const CorrelationMatrix & R);

  Normal(const Point & mean,
         const CovarianceMatrix & C);


  /** Comparison operator */
  using EllipticalDistribution::operator==;
  Bool operator ==(const Normal & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual Normal * clone() const;

  /** Compute the density generator of the ellipticalal generator, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R^(-1)(x-mu))                      */
  Scalar computeDensityGenerator(const Scalar betaSquare) const;
  Scalar computeLogDensityGenerator(const Scalar betaSquare) const;

  /** Compute the derivative of the density generator */
  Scalar computeDensityGeneratorDerivative(const Scalar betaSquare) const;

  /** Compute the seconde derivative of the density generator */
  Scalar computeDensityGeneratorSecondDerivative(const Scalar betaSquare) const;

  /** Get one realization of the Normal distribution */
  Point getRealization() const;
  Sample getSample(const UnsignedInteger size) const;

  /** Get the PDF of the Normal distribution */
  using EllipticalDistribution::computePDF;
  Scalar computePDF(const Scalar scalar) const;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the Normal distribution */
  using EllipticalDistribution::computeCDF;
  Scalar computeCDF(const Scalar scalar) const;
  Scalar computeCDF(const Point & point) const;

  using EllipticalDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Scalar scalar) const;
  Scalar computeComplementaryCDF(const Point & point) const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;
  Complex computeCharacteristicFunction(const Point & x) const;
  Complex computeLogCharacteristicFunction(const Scalar x) const;
  Complex computeLogCharacteristicFunction(const Point & x) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Get the CDF gradient of the distribution */
  using EllipticalDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Compute the radial distribution CDF */
  Scalar computeRadialDistributionCDF(const Scalar radius,
                                      const Bool tail = false) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using EllipticalDistribution::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const;
  Point computeSequentialConditionalPDF(const Point & x) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using EllipticalDistribution::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const;
  Point computeSequentialConditionalCDF(const Point & x) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using EllipticalDistribution::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const;
  Point computeSequentialConditionalQuantile(const Point & q) const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  Scalar getRoughness() const;

  /** Get the skewness of the distribution */
  Point getSkewness() const;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

  /** Correlation matrix accessor */
  void setCorrelation(const CorrelationMatrix & R);

  /** Get the copula of a distribution */
  Distribution getCopula() const;

  /* Interface specific to Implementation */

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Quantile computation for dimension=1 */
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** Check if the distribution has independent copula */
  void checkIndependentCopula();

  /** The log normalization factor of the Normal distribution */
  Scalar logNormalizationFactor_;

  /** Store the independence status */
  Bool hasIndependentCopula_;

}; /* class Normal */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMAL_HXX */
