//                                               -*- C++ -*-
/**
 *  @brief The MaximumDistribution distribution
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
  CLASSNAME
public:

  /** A type for distribution collection */
  typedef Collection<Distribution> DistributionCollection;

  /** Default constructor */
  MaximumDistribution();

  /** Parameters constructor */
  explicit MaximumDistribution(const Distribution & distribution);

  /** Parameters constructor */
  explicit MaximumDistribution(const DistributionCollection & collection);

  /** Parameters constructor */
  MaximumDistribution(const Distribution & distribution,
                      const UnsignedInteger size);

  /** Comparison operator */
  Bool operator ==(const MaximumDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  MaximumDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /* Interface specific to MaximumDistribution */

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);
  Distribution getDistribution() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

private:

  /* Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /* The underlying joint distribution */
  Distribution distribution_;

  /* Flag to tell if all the components are equal and independent */
  Bool allSame_;

  /* Number of variables upon which the maximum is taken */
  UnsignedInteger variablesNumber_;

}; /* class MaximumDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMDISTRIBUTION_HXX */
