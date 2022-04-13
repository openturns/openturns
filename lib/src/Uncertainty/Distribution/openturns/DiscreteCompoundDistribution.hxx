//                                               -*- C++ -*-
/**
 *  @brief Discrete compound distribution
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
#ifndef OPENTURNS_DISCRETECOMPOUNDDISTRIBUTION_HXX
#define OPENTURNS_DISCRETECOMPOUNDDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/UserDefined.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DiscreteCompoundDistribution
 *
 * The DiscreteCompoundDistribution computed from 2 distributions : BaseDistribution (X) and CompoundDistribution(N).
 */
class OT_API DiscreteCompoundDistribution
  : public DiscreteDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  DiscreteCompoundDistribution();

  /** Constructor using distributions */
  DiscreteCompoundDistribution(const Distribution & baseDistribution,
                               const Distribution & compoundDistribution);

  /** Comparison operator */
  Bool operator ==(const DiscreteCompoundDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  DiscreteCompoundDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using DiscreteDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

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

  /** Compute the generating function, i.e. psi(z) = E(z^X) */
  Complex computeGeneratingFunction(const Complex & z) const override;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const override;

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


  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Base distribution accessor */
  Distribution getBaseDistribution() const;

  /** Compound distribution accessor */
  Distribution getCompoundDistribution() const;
protected:


private:
  /** Get upper bound of distribution */
  UnsignedInteger computeIntegerUpperBound();

  /** Get table of probabilities */
  Point computeProbabilities(const UnsignedInteger m) const;

  /** Get the quantile of the distribution */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Create UserDefined distribution_ attribute */
  void setBaseAndCompoundDistributions(const Distribution & baseDistribution,
                                       const Distribution & compoundDistribution);

  /** Base distribution */
  Distribution baseDistribution_;

  /** Compound distribution */
  Distribution compoundDistribution_;

  /** Actual discrete integral compound */
  UserDefined distribution_;

  /** Structures for the alias sampling method */
  mutable Point probabilities_;
  mutable Point base_;
  mutable Indices alias_;
}; /* class DiscreteCompoundDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISCRETECOMPOUNDDISTRIBUTION_HXX */
