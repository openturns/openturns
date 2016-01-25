//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all distribution factories
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
#ifndef OPENTURNS_DISTRIBUTIONIMPLEMENTATIONFACTORY_HXX
#define OPENTURNS_DISTRIBUTIONIMPLEMENTATIONFACTORY_HXX

#include "DistributionFactoryResult.hxx"
#include "ResourceMap.hxx"
#include "DistributionParameters.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DistributionFactoryImplementation
 */
class OT_API DistributionFactoryImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  typedef Collection<NumericalPoint>                 NumericalPointCollection;
  typedef Collection<NumericalPointWithDescription>  NumericalPointWithDescriptionCollection;
  typedef DistributionImplementation::Implementation Implementation;

  /** Default constructor */
  explicit DistributionFactoryImplementation(const UnsignedInteger bootstrapSize = ResourceMap::GetAsUnsignedInteger("DistributionFactoryImplementation-DefaultBootstrapSize"));

  /** Virtual constructor */
  virtual DistributionFactoryImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /* Here is the interface that all derived class must implement */
  /** Build a distribution based on a sample */
  virtual Implementation build(const NumericalSample & sample) const;

  /** Build a distribution based on a sample and gives the covariance matrix of the estimate */
  virtual Implementation build(const NumericalSample & sample,
                               CovarianceMatrix & covariance) const;

  /** Build a distribution based on a set of parameters */
  virtual Implementation build(const NumericalPoint & parameters) const;

  /** Build a distribution using its default constructor */
  virtual Implementation build() const;

  /** Build the distribution and the parameter distribution */
  virtual DistributionFactoryResult buildEstimator(const NumericalSample & sample) const;

  /** Build the distribution and the parameter distribution in a new parametrization */
  virtual DistributionFactoryResult buildEstimator(const NumericalSample & sample,
                                                   const DistributionParameters & parameters) const;

  /** Bootstrap size accessor */
  UnsignedInteger getBootstrapSize() const;
  void setBootstrapSize(const UnsignedInteger bootstrapSize);

  /** Accessor to known parameter */
  void setKnownParameter(const NumericalPoint & values, const Indices & positions);
  NumericalPoint getKnownParameterValues() const;
  Indices getKnownParameterIndices() const;

protected:
  /* Bootstrap estimator */
  virtual DistributionFactoryResult buildBootStrapEstimator(const NumericalSample & sample, const Bool isGaussian = false) const;

  /* Build the distribution and the parameter distribution */
  virtual DistributionFactoryResult buildMaximumLikelihoodEstimator(const NumericalSample & sample, const Bool isRegular = false) const;

  /* Number of bootstrap resampling for covariance estimation */
  UnsignedInteger bootstrapSize_;

  /* Known parameter */
  NumericalPoint knownParameterValues_;
  Indices knownParameterIndices_;

}; /* class DistributionFactoryImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONIMPLEMENTATIONFACTORY_HXX */
