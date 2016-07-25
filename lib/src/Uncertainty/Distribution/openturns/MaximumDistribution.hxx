//                                               -*- C++ -*-
/**
 *  @brief The MaximumDistribution distribution
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
#ifndef OPENTURNS_MAXIMUMDISTRIBUTION_HXX
#define OPENTURNS_MAXIMUMDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Collection.hxx"

/*
The maximum distribution of F is the distribution of X = max(X_1, ... , X_n)
where (X_1, ... , X_n) ~ F.
P(X < x) = P(X_1 < x && ... && X_n < x)
 */
BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MaximumDistribution
 *
 * The MaximumDistribution distribution.
 */
class OT_API MaximumDistribution
  : public DistributionImplementation
{
  CLASSNAME;
public:

  /** A type for distribution collection */
  typedef Collection<Distribution> DistributionCollection;

  /** Default constructor */
  MaximumDistribution();

  /** Parameters constructor */
  MaximumDistribution(const Distribution & distribution);

  /** Parameters constructor */
  MaximumDistribution(const DistributionCollection & collection);

  /** Parameters constructor */
  MaximumDistribution(const Distribution & distribution,
		      const UnsignedInteger size);

  /** Comparison operator */
  Bool operator ==(const MaximumDistribution & other) const;
 protected:
  Bool equals(const DistributionImplementation & other) const;
 public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual MaximumDistribution * clone() const;

  /** Get one realization of the distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /* Interface specific to MaximumDistribution */

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:

  /* Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /* The underlying joint distribution */
  Distribution distribution_;

  /* Flag to tell if all the components are equal and independent */
  Bool allSame_;

  /* Number of variables upon which the maximum is taken */
  UnsignedInteger variablesNumber_;

}; /* class MaximumDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMDISTRIBUTION_HXX */
