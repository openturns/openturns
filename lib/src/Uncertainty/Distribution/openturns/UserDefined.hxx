//                                               -*- C++ -*-
/**
 *  @brief The UserDefined distribution
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
#ifndef OPENTURNS_USERDEFINED_HXX
#define OPENTURNS_USERDEFINED_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/UserDefinedPair.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefined
 *
 * The UserDefined distribution.
 */
class OT_API UserDefined
  : public DiscreteDistribution
{
  CLASSNAME;
public:

  typedef Collection<UserDefinedPair>           UserDefinedPairCollection;
  typedef PersistentCollection<UserDefinedPair> UserDefinedPairPersistentCollection;

  /** Default constructor */
  UserDefined();

  /** @deprecated Constructor from PairCollection */
  explicit UserDefined(const UserDefinedPairCollection & collection);

  /** Constructor from a sample */
  explicit UserDefined(const NumericalSample & points);

  /** Constructor from a sample and associated weights */
  UserDefined(const NumericalSample & points,
              const NumericalPoint & weights);


  /** Comparison operator */
  Bool operator ==(const UserDefined & other) const;
 protected:
  Bool equals(const DistributionImplementation & other) const;
 public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual UserDefined * clone() const;

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

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  NumericalSample getSupport(const Interval & interval) const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /** Parameters value accessors */
  NumericalPoint getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to UserDefined */

  /** Data accessors */
  void setData(const NumericalSample & points,
               const NumericalPoint & weights);
  NumericalSample getX() const;
  NumericalPoint getP() const;

  /** @deprecated Pair collection accessor */
  void setPairCollection(const UserDefinedPairCollection & collection);
  UserDefinedPairCollection getPairCollection() const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Merge the identical points of the support */
  void compactSupport(const NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("DiscreteDistribution-SupportEpsilon"));

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

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

  /** Quantile computation for dimension=1 */
  NumericalScalar computeScalarQuantile(const NumericalScalar prob,
                                        const Bool tail = false) const;

  /** The collection of couple (xi, pi) */
  NumericalSample points_;
  NumericalPoint probabilities_;

  /** The cumulative probabilities si = sum(pk, k=0..i) */
  NumericalPoint cumulativeProbabilities_;

  /** Flag to accelerate computations in case of uniform weights */
  Bool hasUniformWeights_;

}; /* class UserDefined */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINED_HXX */
