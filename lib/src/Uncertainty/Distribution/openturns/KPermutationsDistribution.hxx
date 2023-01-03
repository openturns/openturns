//                                               -*- C++ -*-
/**
 *  @brief The KPermutationsDistribution distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KPERMUTATIONSDISTRIBUTION_HXX
#define OPENTURNS_KPERMUTATIONSDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KPermutationsDistribution
 *
 * The KPermutationsDistribution distribution.
 */
class OT_API KPermutationsDistribution
  : public DiscreteDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  KPermutationsDistribution();


  /** Parameters constructor */
  KPermutationsDistribution(const UnsignedInteger k,
                            const UnsignedInteger n);


  /** Comparison operator */
  Bool operator ==(const KPermutationsDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  KPermutationsDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DiscreteDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the quantile of the distribution */
  using DiscreteDistribution::computeQuantile;
  Point computeQuantile(const Scalar prob,
                        const Bool tail,
                        Scalar & marginalProb) const override;

  /** Get the i-th marginal distribution */
  using DiscreteDistribution::getMarginal;
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;

  /* Interface specific to KPermutationsDistribution */

  /** K accessor */
  void setK(const UnsignedInteger k);
  UnsignedInteger getK() const;

  /** N accessor */
  void setN(const UnsignedInteger n);
  UnsignedInteger getN() const;

private:
  /** K/N accessor */
  void setKN(const UnsignedInteger k,
             const UnsignedInteger n);

public:
  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Quantile computation for dimension=1 */
  Scalar computeScalarQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Size of the permutations */
  UnsignedInteger k_;

  /** Size of the base set */
  UnsignedInteger n_;

  /** Log PDF value */
  Scalar logPDFValue_;

}; /* class KPermutationsDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KPERMUTATIONSDISTRIBUTION_HXX */
