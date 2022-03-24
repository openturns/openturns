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
#include "openturns/TensorProductExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/Indices.hxx"
#include "openturns/SpecFunc.hxx"

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

/* Constructor with parameters */
SmolyakExperiment::SmolyakExperiment(const WeightedExperimentCollection & collection, 
    const UnsignedInteger level
)
  : WeightedExperimentImplementation()
  , collection_(collection)
  , level_(level)
  , nodes_(0, 0)
  , isAlreadyComputedNodesAndWeights_(false)
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

/* Compute minimum of multiindex */
UnsignedInteger SmolyakExperiment::indicesMinimum(Indices indices) const
{
  const UnsignedInteger dimension = indices.getSize();
  UnsignedInteger multiindexMin = indices[0];
  for (UnsignedInteger j = 0; j < dimension; ++j) multiindexMin = std::min(indices[j], multiindexMin);
  return multiindexMin;
}

/* Compute multiindex set for combination rule*/
IndicesCollection SmolyakExperiment::computeCombination() const
{
  const UnsignedInteger dimension = collection_.getSize();
  // Compute the maximum 1-norm of the multi-index (Gerstner & Griebel, 1998), page 215
  const UnsignedInteger maximumSum = level_ + dimension - 1;
  LOGDEBUG(OSS() << "  maximumSum = " << maximumSum);
  // Create a multi-index set from norm 1
  const LinearEnumerateFunction enumerateFunction(dimension);
  // Compute the size
  UnsignedInteger combinationIndicesCollectionSize = 0;
  Indices indices(dimension);
  for (UnsignedInteger strataIndex = level_; strataIndex < level_ + dimension; ++strataIndex)
  {
    LOGDEBUG(OSS() << "  strataIndex = " <<  strataIndex);
    const UnsignedInteger strataCardinal = enumerateFunction.getStrataCardinal(strataIndex);
    const UnsignedInteger cumulatedCardinal = enumerateFunction.getStrataCumulatedCardinal(
        strataIndex
    );
    const UnsignedInteger indexStart = cumulatedCardinal - strataCardinal;
    for (UnsignedInteger i = indexStart; i < cumulatedCardinal; ++i)
    {
      indices = enumerateFunction(i);
      LOGDEBUG(OSS() << "  indices = " <<  indices);
      UnsignedInteger multiindexMin = indicesMinimum(indices);
      LOGDEBUG(OSS() << "  multiindexMin = " <<  multiindexMin);
      if (multiindexMin > 0)
      {
        // Do not consider a multi-index which has a zero component:
        // this is an empty quadrature
        LOGDEBUG(OSS() << "  Store");
        ++combinationIndicesCollectionSize;
      }
    } // loop over the indices in the strata
  } // loop over the strata
  LOGDEBUG(OSS() << "  combinationIndicesCollectionSize = " << combinationIndicesCollectionSize);
  // Fill the indices
  IndicesCollection combinationIndicesCollection(combinationIndicesCollectionSize, dimension);
  UnsignedInteger multiindexIndex = 0;
  for (UnsignedInteger strataIndex = level_; strataIndex < level_ + dimension; ++strataIndex)
  {
    LOGDEBUG(OSS() << "  strataIndex = " <<  strataIndex);
    const UnsignedInteger strataCardinal = enumerateFunction.getStrataCardinal(strataIndex);
    const UnsignedInteger cumulatedCardinal = enumerateFunction.getStrataCumulatedCardinal(
        strataIndex
    );
    const UnsignedInteger indexStart = cumulatedCardinal - strataCardinal;
    for (UnsignedInteger i = indexStart; i < cumulatedCardinal; ++i)
    {
      indices = enumerateFunction(i);
      LOGDEBUG(OSS() << "  indices = " <<  indices);
      UnsignedInteger multiindexMin = indicesMinimum(indices);
      LOGDEBUG(OSS() << "  multiindexMin = " <<  multiindexMin);
      if (multiindexMin > 0)
      {
        // Do not consider a multi-index which has a zero component:
        // this is an empty quadrature
        LOGDEBUG(OSS() << "  Store");
        std::copy(indices.begin(), indices.end(), combinationIndicesCollection.begin_at(multiindexIndex));
        ++multiindexIndex;
      }
    } // loop over the indices in the strata
  } // loop over the strata
  return combinationIndicesCollection;
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
  // Compute multiindex set
  IndicesCollection combinationIndicesCollection(computeCombination());
  LOGDEBUG(OSS() << "  combinationIndicesCollection = " << combinationIndicesCollection);
  // Create elementary Smolyak quadratures
  Sample nodes(0, dimension);
  Point weights(0);
  const UnsignedInteger numberOfUnitaryQuadratures= combinationIndicesCollection.getSize();
  for (UnsignedInteger i = 0; i < numberOfUnitaryQuadratures; ++i)
  {
    LOGDEBUG(OSS() << "  i = " << i);
    WeightedExperimentCollection collection;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      LOGDEBUG(OSS() << combinationIndicesCollection(i, j));
      collection[i] = collection_[i];
      collection[i].setSize(combinationIndicesCollection(i, j));
    } // Loop over the dimensions
    TensorProductExperiment elementaryExperiment(collection);
    Point elementaryWeights(0);
    Sample elementaryNodes(elementaryExperiment.generateWithWeights(elementaryWeights));
    // Compute Smolyak coefficient
    UnsignedInteger marginalLevelsSum = 0;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      marginalLevelsSum += combinationIndicesCollection(i, j);
    } // Loop over the dimensions
    exponent = level_ + dimension - marginalLevelsSum - 1;
    Scalar smolyakSign;
    if (exponent % 2 == 0)
    {
      smolyakSign = 1.0;
    }
    else:
    {
      smolyakSign = -1.0;
    }
    const UnsignedInteger binomial = SpecFunc::BinomialCoefficient(dimension - 1, marginalLevelsSum - level_);
    const Scalar smolyakFactor = smolyakSign * binomial;
    nodes.add(elementaryNodes);
    weights.add(smolyak_factor * elementaryWeights);
  } // Loop over the marginal levels
  // Reduce to unique nodes and weights
  // TODO
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
