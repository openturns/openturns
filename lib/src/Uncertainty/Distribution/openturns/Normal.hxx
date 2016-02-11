//                                               -*- C++ -*-
/**
 *  @brief The Normal distribution
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

  CLASSNAME;
public:

  /** Default constructor */
  explicit Normal(const UnsignedInteger dimension = 1);

  /** Dimension 1 constructor */
  Normal(const NumericalScalar mu, const NumericalScalar sd);

  /** Constructor for multiD normal distribution */
  Normal(const NumericalPoint & mean,
         const NumericalPoint & sigma,
         const CorrelationMatrix & R);

  Normal(const NumericalPoint & mean,
         const CovarianceMatrix & C);


  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual Normal * clone() const;

  /** Compute the density generator of the ellipticalal generator, i.e.
   *  the function phi such that the density of the distribution can
   *  be written as p(x) = phi(t(x-mu)R^(-1)(x-mu))                      */
  NumericalScalar computeDensityGenerator(const NumericalScalar betaSquare) const;

  /** Compute the derivative of the density generator */
  NumericalScalar computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const;

  /** Compute the seconde derivative of the density generator */
  NumericalScalar computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const;

  /** Get one realization of the Normal distribution */
  NumericalPoint getRealization() const;
  NumericalSample getSample(const UnsignedInteger size) const;

  /** Get the CDF of the Normal distribution */
  using EllipticalDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;
  NumericalComplex computeCharacteristicFunction(const NumericalPoint & x) const;
  NumericalComplex computeLogCharacteristicFunction(const NumericalScalar x) const;
  NumericalComplex computeLogCharacteristicFunction(const NumericalPoint & x) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the CDF gradient of the distribution */
  using EllipticalDistribution::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Compute the radial distribution CDF */
  NumericalScalar computeRadialDistributionCDF(const NumericalScalar radius,
      const Bool tail = false) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  NumericalScalar computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  NumericalScalar computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  NumericalScalar computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Get the roughness, i.e. the L2-norm of the PDF */
  NumericalScalar getRoughness() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Get the raw moments of the standardized distribution */
  NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Implementation getStandardRepresentative() const;

  /** Correlation matrix accessor */
  void setCorrelation(const CorrelationMatrix & R);

  /** Get the copula of a distribution */
  Implementation getCopula() const;

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
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Check if the distribution has independent copula */
  void checkIndependentCopula();

  /** The normalization factor of the Normal distribution */
  NumericalScalar normalizationFactor_;

  /** Store the independence status */
  Bool hasIndependentCopula_;

}; /* class Normal */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMAL_HXX */
