//                                               -*- C++ -*-
/**
 *  @brief The CombinationsDistribution distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_COMBINATIONSDISTRIBUTION_HXX
#define OPENTURNS_COMBINATIONSDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CombinationsDistribution
 *
 * The CombinationsDistribution distribution.
 */
class OT_API CombinationsDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  CombinationsDistribution();


  /** Parameters constructor */
  CombinationsDistribution(const UnsignedInteger k,
                           const UnsignedInteger n);


  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const CombinationsDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  CombinationsDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob,
                        const Bool tail,
                        Scalar & marginalProb) const override;

  /** Get the Spearman correlation of the distribution */
  CorrelationMatrix getSpearmanCorrelation() const override;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const override;

  /** Get the i-th marginal distribution */
  using DistributionImplementation::getMarginal;
  Distribution getMarginal(const UnsignedInteger i) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const override;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const override;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Tell if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;

  /* Interface specific to CombinationsDistribution */

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

  /** Helper to compute the CDF */
  Scalar exploreTree(const Scalar j,
                     const UnsignedInteger lower,
                     const UnsignedInteger upper,
                     const UnsignedInteger count,
                     const Point & xReduced) const;

  void update();

  /** Size of the permutations */
  UnsignedInteger k_ = 0;

  /** Size of the base set */
  UnsignedInteger n_ = 0;

  /** Log PDF value */
  Scalar logPDFValue_ = 0.0;

}; /* class CombinationsDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KPERMUTATIONSDISTRIBUTION_HXX */
