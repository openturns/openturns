//                                               -*- C++ -*-
/**
 *  @brief Maximum likelihood estimation
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
#ifndef OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX
#define OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/DistributionFactory.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MaximumLikelihoodFactory
 */
class OT_API MaximumLikelihoodFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  MaximumLikelihoodFactory();

  /** Parameters constructor */
  MaximumLikelihoodFactory(const Distribution & distribution);

  /** Virtual constructor */
  MaximumLikelihoodFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  using DistributionFactoryImplementation::build;

  /* Here is the interface that all derived class must implement */
  /** Build a distribution based on a sample */
  Distribution build(const Sample & sample) const override;

  /** Build a distribution based on a set of parameters */
  Distribution build(const Point & parameters) const override;

  /** Build a distribution using its default constructor */
  Distribution build() const override;

  /** Build a distribution based on a set of parameters */
  Point buildParameter(const Sample & sample) const;

  /** Optimization solver accessor */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /** Accessor to optimization bounds */
  void setOptimizationBounds(const Interval & optimizationBounds);
  Interval getOptimizationBounds() const;

  /** Accessor to inequality constraint */
  void setOptimizationInequalityConstraint(const Function & optimizationInequalityConstraint);

  /** Accessor to known parameter */
  void setKnownParameter(const Point & values, const Indices & positions);
  Point getKnownParameterValues() const;
  Indices getKnownParameterIndices() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /* The underlying distribution */
  Distribution distribution_;

  /* Solver & optimization problem for log-likelihood maximization */
  OptimizationAlgorithm solver_;

  // Bounds used for parameter optimization
  Interval optimizationBounds_;

  // Inequality constraint used for parameter optimization
  Function optimizationInequalityConstraint_;

  /* Known parameter */
  Point knownParameterValues_;
  Indices knownParameterIndices_;

}; /* class MaximumLikelihoodFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MAXIMUMLIKELIHOODFACTORY_HXX */
