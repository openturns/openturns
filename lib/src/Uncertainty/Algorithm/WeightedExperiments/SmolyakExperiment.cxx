//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an Smolyak experiment
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SmolyakExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SmolyakExperiment)

/* Default constructor */
SmolyakExperiment::SmolyakExperiment()
  : WeightedExperimentImplementation()
  , collection_(0)
  , level_(0)
  , nodes_(0, 0)
{
  // Nothing to do
}

/* Virtual constructor */
SmolyakExperiment * SmolyakExperiment::clone() const
{
  return new SmolyakExperiment(*this);
}

/* String converter */
String SmolyakExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
    << " name=" << getName()
    << " level=" << level_
    << " collection=" << collection_;
  return oss;
}

Bool SmolyakExperiment::hasUniformWeights() const
{
  return false;
}

/* Sample generation */
Sample SmolyakExperiment::generateWithWeights(Point & weights) const
{
  if (!isAlreadyComputedNodesAndWeights_) computeNodesAndWeights();
  weights = weights_;
  return nodes_;
}

/* Compute the nodes and weights */
/*         This may involve negative weights.

The algorithm has 3 steps:
- create the multi-index set for the combination technique,
- create the list of elementary Smolyak quadrature corresponding 
  to each multi-index in the set,
- merge the elementary quadratures to avoid duplicate nodes, 
  updating the weights if necessary.

The algorithm to merge the elementary quadratures 
starts with an empty sample of unique nodes Q^U and weights w^U. 
For each candidate node, we search if it is already in the 
sample of unique nodes:
- if the node is not found in Q^U, it is added to Q^U,
  and the weight is added to w^U,
- otherwise, the weight is updated.
*/
void SmolyakExperiment::computeNodesAndWeights() const
{
  const Scalar relativeEpsilon = ResourceMap::GetAsScalar( "SmolyakExperiment-DefaultPointRelativeEpsilon" );
  const Scalar absoluteEpsilon = ResourceMap::GetAsScalar( "SmolyakExperiment-DefaultPointAbsoluteEpsilon" );
  LOGDEBUG(OSS() << "SmolyakExperiment::computeNodesAndWeights()");
  const UnsignedInteger dimension = collection_.getSize();
  LOGDEBUG(OSS() << "  dimension = " << dimension);
  if (!(dimension > 0))
    throw InvalidArgumentException(HERE) << "Error: expected a positive number of marginal experiments, here it is " << dimension;
  // Compute the maximum 1-norm of the multi-index (Gerstner & Griebel, 1998), page 215
  const UnsignedInteger maximumSum = level_ + dimension - 1;
  LOGDEBUG(OSS() << "  maximumSum = " << maximumSum);
  // Create the multi-index set for combination rule
  // Create a multi-index set from norm 1
  const LinearEnumerateFunction enumerateFunction(dimension);
  const UnsignedInteger normOneIndicesCollectionSize =  enumerateFunction.getStrataCardinal(1 + maximumSum);
  // Compute the size
  UnsignedInteger combinationIndicesCollectionSize = 0;
  Indices indices(dimension);
  for (UnsignedInteger flatIndex = 0; flatIndex < normOneIndicesCollectionSize; ++flatIndex)
  {
    indices = enumerateFunction(flatIndex);
    UnsignedInteger multiindexSum = 0;
    for (UnsignedInteger j = 0; j < dimension; ++j) multiindexSum += indices[j];
    UnsignedInteger multiindexMin = indices[0];
    for (UnsignedInteger j = 0; j < dimension; ++j) multiindexMin = std::min(indices[j], multiindexMin);
    // Do not consider a multi-index which has a zero component:
    // this is an empty quadrature
    if (multiindexSum <= level_ + dimension - 1 && multiindexSum >= level_ && multiindexMin > 0) ++combinationIndicesCollectionSize;
  }
  // Store the indices
  IndicesCollection combinationIndicesCollection(combinationIndicesCollectionSize, dimension);
  combinationIndicesCollectionSize = 0;
  for (UnsignedInteger flatIndex = 0; flatIndex < normOneIndicesCollectionSize; ++flatIndex)
  {
    indices = enumerateFunction(flatIndex);
    UnsignedInteger multiindexSum = 0;
    for (UnsignedInteger j = 0; j < dimension; ++j) multiindexSum += indices[j];
    UnsignedInteger multiindexMin = indices[0];
    for (UnsignedInteger j = 0; j < dimension; ++j) multiindexMin = std::min(indices[j], multiindexMin);
    // Do not consider a multi-index which has a zero component:
    // this is an empty quadrature
    if (multiindexSum <= level_ + dimension - 1 && multiindexSum >= level_ && multiindexMin > 0)
    {
        std::copy(indices.begin(), indices.end(), combinationIndicesCollection.begin_at(combinationIndicesCollectionSize));
        ++combinationIndicesCollectionSize;
    }
  }
}

/* Method save() stores the object through the StorageManager */
void SmolyakExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
  adv.saveAttribute("level_", level_);
}

/* Method load() reloads the object from the StorageManager */
void SmolyakExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
  adv.loadAttribute("level_", level_);
}


END_NAMESPACE_OPENTURNS
