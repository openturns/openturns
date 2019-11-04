//                                               -*- C++ -*-
/**
 *  @brief The UserDefined distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_USERDEFINED_HXX
#define OPENTURNS_USERDEFINED_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DiscreteDistribution.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UserDefined
 *
 * The UserDefined distribution.
 */
class OT_API UserDefined
  : public DiscreteDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  UserDefined();

  /** Constructor from a sample */
  explicit UserDefined(const Sample & points);

  /** Constructor from a sample and associated weights */
  UserDefined(const Sample & points,
              const Point & weights);


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
  Point getRealization() const;

  /** Get a sample of the distribution */
  Sample getSample(const UnsignedInteger size) const;

  /** Get the PDF of the distribution */
  using DiscreteDistribution::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using DiscreteDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the PDFGradient of the distribution */
  using DiscreteDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  using DiscreteDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  Sample getSupport(const Interval & interval) const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to UserDefined */

  /** Data accessors */
  void setData(const Sample & points,
               const Point & weights);
  Sample getX() const;
  Point getP() const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const;

  /** Merge the identical points of the support */
  void compactSupport(const Scalar epsilon = ResourceMap::GetAsScalar("DiscreteDistribution-SupportEpsilon"));

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
  Scalar computeScalarQuantile(const Scalar prob,
                               const Bool tail = false) const;

  /** The collection of couple (xi, pi) */
  Sample points_;
  Point probabilities_;

  /** The cumulative probabilities si = sum(pk, k=0..i) */
  Point cumulativeProbabilities_;

  /** Flag to accelerate computations in case of uniform weights */
  Bool hasUniformWeights_;

  /** Structures for the alias sampling method */
  mutable Point base_;
  mutable Indices alias_;
}; /* class UserDefined */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_USERDEFINED_HXX */
