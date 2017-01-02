//                                               -*- C++ -*-
/**
 *  @brief The Multinomial distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MULTINOMIAL_HXX
#define OPENTURNS_MULTINOMIAL_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Multinomial
 *
 * The Multinomial distribution.
 */
class OT_API Multinomial
  : public DiscreteDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  Multinomial();


  /** Parameters constructor */
  Multinomial(const UnsignedInteger n,
              const NumericalPoint & p);


  /** Comparison operator */
  Bool operator ==(const Multinomial & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual Multinomial * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  NumericalScalar computeConditionalPDF(const NumericalScalar x,
                                        const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  NumericalScalar computeConditionalCDF(const NumericalScalar x,
                                        const NumericalPoint & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  NumericalScalar computeConditionalQuantile(const NumericalScalar q,
      const NumericalPoint & y) const;

  /** Get the i-th marginal distribution */
  using DiscreteDistribution::getMarginal;
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  NumericalSample getSupport(const Interval & interval) const;
  NumericalSample getSupport() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /* Interface specific to Multinomial */

  /** P vector accessor */
  void setP(const NumericalPoint & p);
  NumericalPoint getP() const;

  /** N accessor */
  void setN(const UnsignedInteger n);
  UnsignedInteger getN() const;

  /** SmallA accessor */
  void setSmallA(const NumericalScalar smallA);
  NumericalScalar getSmallA() const;

  /** Eta accessor */
  void setEta(const NumericalScalar eta);
  NumericalScalar getEta() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Compute the generating function of a sum of truncated Poisson distributions as needed in the computeCDF() method */
  NumericalComplex computeGlobalPhi(const NumericalComplex & z,
                                    const NumericalPoint & x) const;

  /** Compute the generating function of a truncated Poisson distributions as needed in the computeCDF() method */
  NumericalComplex computeLocalPhi(const NumericalComplex & z,
                                   const NumericalScalar lambda,
                                   const NumericalScalar a) const;

  /** Quantile computation for dimension=1 */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The range of the output */
  UnsignedInteger n_;

  /** The vector of probabilities of the Multinomial distribution */
  NumericalPoint p_;

  /** The sum of probabilities of the Multinomial distribution */
  NumericalScalar sumP_;

  /** Normalization factor for the CDF */
  NumericalScalar normalizationCDF_;

  /** Radius of the discretization in Poisson's formula */
  NumericalScalar r_;

  /** Threshold for the A parameter of the CDF algorithm */
  NumericalScalar smallA_;

  /** Normalization for the CDF algorithm */
  NumericalScalar eta_;

}; /* class Multinomial */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTINOMIAL_HXX */
