//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all distribution factories
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
#ifndef OPENTURNS_DISTRIBUTIONIMPLEMENTATIONFACTORY_HXX
#define OPENTURNS_DISTRIBUTIONIMPLEMENTATIONFACTORY_HXX

#include "openturns/DistributionFactoryResult.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistributionParameters.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DistributionFactoryImplementation
 */
class OT_API DistributionFactoryImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Collection<Point>                 PointCollection;
  typedef Collection<PointWithDescription>  PointWithDescriptionCollection;
  typedef DistributionImplementation::Implementation Implementation;

  /** Default constructor */
  explicit DistributionFactoryImplementation(const UnsignedInteger bootstrapSize = ResourceMap::GetAsUnsignedInteger("DistributionFactory-DefaultBootstrapSize"));

  /** Virtual constructor */
  DistributionFactoryImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /* Here is the interface that all derived class must implement */
  /** Build a distribution based on a sample */
  virtual Distribution build(const Sample & sample) const;

  /** Build a distribution based on a set of parameters */
  virtual Distribution build(const Point & parameters) const;

  /** Build a distribution using its default constructor */
  virtual Distribution build() const;

  /** Build the distribution and the parameter distribution */
  virtual DistributionFactoryResult buildEstimator(const Sample & sample) const;

  /** Build the distribution and the parameter distribution in a new parametrization */
  virtual DistributionFactoryResult buildEstimator(const Sample & sample,
      const DistributionParameters & parameters) const;

  /** Bootstrap size accessor */
  UnsignedInteger getBootstrapSize() const;
  void setBootstrapSize(const UnsignedInteger bootstrapSize);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /* Bootstrap estimator */
  virtual DistributionFactoryResult buildBootStrapEstimator(const Sample & sample, const Bool isGaussian = false) const;

  /* Build the distribution and the parameter distribution */
  virtual DistributionFactoryResult buildMaximumLikelihoodEstimator(const Sample & sample, const Bool isRegular = false) const;

  /* Number of bootstrap resampling for covariance estimation */
  UnsignedInteger bootstrapSize_;

}; /* class DistributionFactoryImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONIMPLEMENTATIONFACTORY_HXX */
