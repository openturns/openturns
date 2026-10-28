//                                               -*- C++ -*-
/**
 *  @brief Estimate the factor distributions of a cumulative distribution
 *         network from data by maximum likelihood
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
#ifndef OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORKFACTORY_HXX
#define OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORKFACTORY_HXX

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/BipartiteGraph.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Interval.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CumulativeDistributionNetworkFactory
 *
 * Estimate the factors of a cumulative distribution network, the graph being
 * known. The parameters of the factors are concatenated in graph order and
 * optimized by maximum likelihood of the exact network density.
 */
class OT_API CumulativeDistributionNetworkFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** A type for distribution collection */
  typedef Collection<Distribution>           DistributionCollection;
  typedef PersistentCollection<Distribution> DistributionPersistentCollection;

  /** Default constructor */
  CumulativeDistributionNetworkFactory();

  /** Parameters constructor */
  CumulativeDistributionNetworkFactory(const BipartiteGraph & graph,
                                       const DistributionCollection & distributions);

  /** Virtual constructor */
  CumulativeDistributionNetworkFactory * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  using DistributionFactoryImplementation::build;

  /** Build a distribution using its default constructor */
  Distribution build() const override;

  /** Build a distribution based on a sample */
  Distribution build(const Sample & sample) const override;

  /** Build a distribution based on a set of parameters */
  Distribution build(const Point & parameters) const override;

  /** Build the distribution and the parameter distribution */
  DistributionFactoryResult buildEstimator(const Sample & sample) const override;

  /** Graph accessor */
  void setGraph(const BipartiteGraph & graph);
  BipartiteGraph getGraph() const;

  /** Distribution template accessor */
  void setDistributionCollection(const DistributionCollection & distributions);
  DistributionCollection getDistributionCollection() const;

  /** Number of parameters of the network */
  UnsignedInteger getParameterDimension() const;

  /** Starting point accessor */
  void setStartingPoint(const Point & startingPoint);
  Point getStartingPoint() const;

  /** Optimization algorithm accessor */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & solver);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /** Optimization bounds accessor */
  void setOptimizationBounds(const Interval & bounds);
  Interval getOptimizationBounds() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Initialize the optimization solver using the ResourceMap */
  void initializeSolver();

  /** Check that the graph and the distribution templates are consistent */
  void checkConsistency() const;

  /** Concatenate the parameters of the templates */
  Point getTemplateParameter() const;

  /** Build the factor distributions from a concatenated parameter */
  DistributionCollection buildFactors(const Point & parameter) const;

  /** Optimize the concatenated parameter on a sample */
  Point computeParameter(const Sample & sample) const;

  /** Graph defining the network */
  BipartiteGraph graph_;

  /** Distribution templates, one per red node */
  DistributionPersistentCollection distributionCollection_;

  /** Optimization algorithm */
  OptimizationAlgorithm solver_;

  /** Optimization bounds */
  Interval optimizationBounds_;

  /** Starting point */
  Point startingPoint_;

}; /* class CumulativeDistributionNetworkFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CUMULATIVEDISTRIBUTIONNETWORKFACTORY_HXX */
