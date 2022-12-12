//                         -*- C++ -*-
/**
 *  @brief Abstract top-level view of a TensorProductExperiment
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
#include "openturns/TensorProductExperiment.hxx"
#include "openturns/Tuples.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/BlockIndependentDistribution.hxx"
#include <iostream>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TensorProductExperiment)

typedef Collection< Point > PointCollection;

/* Default constructor */
TensorProductExperiment::TensorProductExperiment()
  : WeightedExperimentImplementation()
  , collection_(0)
  , nodes_(0, 0)
{
  // Nothing to do
}

/* Constructor with parameters */
TensorProductExperiment::TensorProductExperiment(const WeightedExperimentCollection & collection)
  : WeightedExperimentImplementation()
  , collection_(collection)
  , nodes_(0, 0)
{
  const UnsignedInteger numberOfMarginalExperiments = collection_.getSize();
  BlockIndependentDistribution::DistributionCollection distributionCollection(numberOfMarginalExperiments);
  UnsignedInteger size = 1;
  for (UnsignedInteger i = 0; i < numberOfMarginalExperiments; ++i)
  {
    size *= collection_[i].getSize();
    distributionCollection[i] = collection_[i].getDistribution();
  }
  WeightedExperimentImplementation::setSize(size);
  const BlockIndependentDistribution distribution(distributionCollection);
  WeightedExperimentImplementation::setDistribution(distribution);
}

/* Virtual constructor */
TensorProductExperiment * TensorProductExperiment::clone() const
{
  return new TensorProductExperiment(*this);
}

/* String converter */
String TensorProductExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " collection=" << collection_;
  return oss;
}

Bool TensorProductExperiment::hasUniformWeights() const
{
  bool hasUniformWeights = true;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++i)
  {
    if (!collection_[i].hasUniformWeights())
    {
      hasUniformWeights = false;
      break;
    }
  }

  return hasUniformWeights;
}

/* Sample generation */
Sample TensorProductExperiment::generateWithWeights(Point & weights) const
{
  LOGDEBUG(OSS() << "generateWithWeights()");
  computeNodesAndWeights();
  weights = weights_;
  return nodes_;
}

/* Compute the tensor product nodes and weights */
void TensorProductExperiment::computeNodesAndWeights() const
{
  LOGDEBUG(OSS() << "TensorProductExperiment::computeNodesAndWeights()");
  const UnsignedInteger numberOfMarginalExperiments = collection_.getSize();
  LOGDEBUG(OSS() << "  numberOfMarginalExperiments = " << numberOfMarginalExperiments);
  if (!(numberOfMarginalExperiments > 0))
    throw InvalidArgumentException(HERE) << "Error: expected a positive number of marginal experiments, here it is " << numberOfMarginalExperiments;
  // Compute marginal nodes and weights
  // Compute dimension
  // Compute size for each marginal experiment.
  LOGDEBUG(OSS() << "Compute marginal nodes and weights");
  Collection<Sample> marginalNodesCollection(numberOfMarginalExperiments);
  Collection<Point> marginalWeightsCollection(numberOfMarginalExperiments);
  UnsignedInteger dimension = 0;
  Indices marginalSizeIndices(numberOfMarginalExperiments);
  for (UnsignedInteger i = 0; i < numberOfMarginalExperiments; ++i)
  {
    Point weights;
    const Sample nodes(collection_[i].generateWithWeights(weights));
    marginalNodesCollection[i] = nodes;
    marginalWeightsCollection[i] = weights;
    dimension += nodes.getDimension();
    marginalSizeIndices[i] = nodes.getSize();
  }
  LOGDEBUG(OSS() << "dimension = " << dimension);
  LOGDEBUG(OSS() << "marginalSizeIndices = " << marginalSizeIndices);
  // Create index set
  LOGDEBUG(OSS() << "Create index set");
  Tuples tuples(marginalSizeIndices);
  const IndicesCollection indicesCollection(tuples.generate());
  // Compute nodes and weights
  LOGDEBUG(OSS() << "Compute nodes and weights");
  const UnsignedInteger size = indicesCollection.getSize();
  LOGDEBUG(OSS() << "Total size = " << size);
  weights_ = Point(size);
  nodes_ = Sample(size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    weights_[i] = 1.0;
    UnsignedInteger marginalExperimentIndex = 0;
    for (UnsignedInteger j = 0; j < numberOfMarginalExperiments; ++j)
    {
      const UnsignedInteger marginalIndex = indicesCollection(i, j);
      const Point marginalWeights(marginalWeightsCollection[j]);
      weights_[i] *= marginalWeights[marginalIndex];
      const Sample marginalNodes(marginalNodesCollection[j]);
      UnsignedInteger marginalDimension = marginalNodes.getDimension();
      for (UnsignedInteger k = 0; k < marginalDimension; ++k)
      {
        nodes_(i, marginalExperimentIndex) = marginalNodes(marginalIndex, k);
        ++marginalExperimentIndex;
      } // Loop for k over the dimensions
    } // Loop for j over the marginal experiments
  } // Loop for i over the n-D nodes
}

/* Method save() stores the object through the StorageManager */
void TensorProductExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void TensorProductExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
}

/* Distribution collection accessor */
TensorProductExperiment::WeightedExperimentCollection TensorProductExperiment::getWeightedExperimentCollection() const
{
  return collection_;
}

/* Distribution collection accessor */
void TensorProductExperiment::setWeightedExperimentCollection(const WeightedExperimentCollection & coll)
{
  collection_ = coll;
}

END_NAMESPACE_OPENTURNS
