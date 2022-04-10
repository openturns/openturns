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
#include "openturns/BlockIndependentDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SmolyakExperiment)

/* Default constructor */
SmolyakExperiment::SmolyakExperiment()
  : WeightedExperimentImplementation()
  , collection_(0)
  , level_(0)
  , nodes_(0, 0)
  , isAlreadyComputed_(false)
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
  , isAlreadyComputed_(false)
{
  const UnsignedInteger numberOfMarginalExperiments = collection_.getSize();
  BlockIndependentDistribution::DistributionCollection distributionCollection(numberOfMarginalExperiments);
  for (UnsignedInteger i = 0; i < numberOfMarginalExperiments; ++i)
  {
    distributionCollection[i] = collection_[i].getDistribution();
  }
  const BlockIndependentDistribution distribution(distributionCollection);
  WeightedExperimentImplementation::setDistribution(distribution);
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
Sample SmolyakExperiment::generateWithWeights(Point & weights)
{
  computeNodesAndWeights();
  weights = weights_;
  WeightedExperimentImplementation::setSize(nodes_.getSize());
  isAlreadyComputed_ = true;
  return nodes_;
}

/* Compute the minimum of a multiindex */
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
  UnsignedInteger cardinalMax = enumerateFunction.getStrataCumulatedCardinal(level_ - 1);
  UnsignedInteger cardinalMin = enumerateFunction.getStrataCumulatedCardinal(level_ - dimension - 1);
  UnsignedInteger combinationIndicesCollectionSize = cardinalMax - cardinalMin;
  // Fill the indices
  IndicesCollection combinationIndicesCollection(combinationIndicesCollectionSize, dimension);
  UnsignedInteger multiindexIndex = 0;
  Indices indices(dimension);
  for (UnsignedInteger strataIndex = level_ - dimension; strataIndex < level_; ++strataIndex)
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
      for (UnsignedInteger j = 0; j < dimension; ++j) indices[j] += 1;
      LOGDEBUG(OSS() << "  indices = " <<  indices);
      std::copy(indices.begin(), indices.end(), combinationIndicesCollection.begin_at(multiindexIndex));
      ++multiindexIndex;
    } // loop over the indices in the strata
  } // loop over the strata
  return combinationIndicesCollection;
}

/* Comparison class with std:map interface. */
class PointApproximateComparison
{
public:
    PointApproximateComparison(const Scalar absoluteEpsilon, 
                      const Scalar relativeEpsilon):
                      absoluteEpsilon_(absoluteEpsilon)
                      , relativeEpsilon_(relativeEpsilon)
    {
        // Nothing to do
    };

    /* Compare two points, according to lexicographic order
    * Returns true if x < y, false otherwise.
    */
    bool operator()(const Point x, const Point y) const {
        const UnsignedInteger dimension = x.getDimension();
        if (y.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the two points must have the same dimension. Here x has dimension " << dimension << " while y has dimension " << y.getDimension();
        bool comparison = false;
        for (UnsignedInteger k = 0; k < dimension; ++k)
        {
            // std::cout << "  " << x[k] << " < " << y[k] << " ?" << std::endl;
            const Scalar maximumXY = std::max(std::abs(x[k]), std::abs(y[k]));
            const Scalar delta = absoluteEpsilon_ + relativeEpsilon_ * maximumXY;
            if (x[k] + delta < y[k])
            {
                comparison = true;
                break;
            } 
            else if (x[k] > y[k] + delta)
            {
                break;
            }
        }
        return comparison;
    }
private:
    // Absolute tolerance for comparison
    Scalar absoluteEpsilon_;
    // Relative tolerance for comparison
    Scalar relativeEpsilon_;
};

// Implement merge with std::map
void SmolyakExperiment::mergeNodesAndWeights(
    const Sample duplicatedNodes, const Point duplicatedWeights) const
{
  LOGDEBUG(OSS() << "SmolyakExperiment::mergeNodesAndWeights()");
  const Scalar relativeEpsilon = ResourceMap::GetAsScalar( "SmolyakExperiment-DefaultPointRelativeEpsilon" );
  const Scalar absoluteEpsilon = ResourceMap::GetAsScalar( "SmolyakExperiment-DefaultPointAbsoluteEpsilon" );
  UnsignedInteger duplicatedSize = duplicatedNodes.getSize();
  LOGDEBUG(OSS() << "Number of (potentially) duplicated nodes =" << duplicatedSize);
  if (duplicatedWeights.getDimension() != duplicatedSize) throw InvalidArgumentException(HERE) << "Error: the weights must have dimension " << duplicatedSize << " but have dimension " << duplicatedWeights.getDimension();
    UnsignedInteger dimension = duplicatedNodes.getDimension();
  // Fill the map
  std::map<Point, Scalar, PointApproximateComparison> nodeWeightMap(PointApproximateComparison(absoluteEpsilon, relativeEpsilon));
  for (UnsignedInteger i = 0; i < duplicatedSize; ++i)
  {
    std::map<Point, Scalar>::iterator search = nodeWeightMap.find(duplicatedNodes[i]);
    if (search != nodeWeightMap.end()) {
      LOGDEBUG(OSS() << "[" << i << "], found     : " << search->first << " = " << search->second);
      search->second += duplicatedWeights[i];
    } else {
      LOGDEBUG(OSS() << "[" << i << "], not found : " << duplicatedNodes[i]);
      nodeWeightMap[duplicatedNodes[i]] = duplicatedWeights[i];
    }
  }
  // print_NodeWeightMap(nodeWeightMap);
  // Extract the map
  UnsignedInteger size = nodeWeightMap.size();
  LOGDEBUG(OSS() << "Extract the map");
  nodes_ = Sample(size, dimension);
  weights_ = Point(size);
  UnsignedInteger index = 0;
  for (std::map<Point, Scalar>::iterator it = nodeWeightMap.begin(); it != nodeWeightMap.end(); ++ it)
  {
    LOGDEBUG(OSS() << "[" << index << "], add " << it->first << " = " << it->second);
    nodes_[index] = it->first;
    weights_[index] = it->second;
    ++ index;
  }
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
  LOGDEBUG(OSS() << "SmolyakExperiment::computeNodesAndWeights()");
  const UnsignedInteger dimension = collection_.getSize();
  LOGDEBUG(OSS() << "  dimension = " << dimension);
  if (!(dimension > 0))
    throw InvalidArgumentException(HERE) << "Error: expected a positive number of marginal experiments, here it is " << dimension;
  // Compute multiindex set
  IndicesCollection combinationIndicesCollection(computeCombination());
  LOGDEBUG(OSS() << "  combinationIndicesCollection = " << combinationIndicesCollection);
  // Create elementary Smolyak quadratures
  Sample duplicatedNodes(0, dimension);
  Point duplicatedWeights(0);
  const UnsignedInteger numberOfUnitaryQuadratures= combinationIndicesCollection.getSize();
  for (UnsignedInteger i = 0; i < numberOfUnitaryQuadratures; ++i)
  {
    WeightedExperimentCollection collection;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      WeightedExperiment marginalExperiment(collection_[j]);
      marginalExperiment.setLevel(combinationIndicesCollection(i, j));
      collection.add(marginalExperiment);
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
    const UnsignedInteger exponent = level_ + dimension - marginalLevelsSum - 1;
    Scalar smolyakSign;
    if (exponent % 2 == 0) smolyakSign = 1.0;
    else smolyakSign = -1.0;
    const UnsignedInteger binomial = SpecFunc::BinomialCoefficient(dimension - 1, marginalLevelsSum - level_);
    const Scalar smolyakFactor = smolyakSign * binomial;
    duplicatedNodes.add(elementaryNodes);
    duplicatedWeights.add(smolyakFactor * elementaryWeights);
  } // Loop over the marginal levels
  // Reduce to unique nodes and weights
  mergeNodesAndWeights(duplicatedNodes, duplicatedWeights);
}

/* Distribution collection accessor */
const SmolyakExperiment::WeightedExperimentCollection & SmolyakExperiment::getWeightedExperimentCollection() const
{
  return collection_;
}

/* Distribution collection accessor */
void SmolyakExperiment::setWeightedExperimentCollection(const WeightedExperimentCollection & coll)
{
  collection_ = coll;
  isAlreadyComputed_ = false;
}

/* Get size */
UnsignedInteger SmolyakExperiment::getSize() const
{
  if (!isAlreadyComputed_) throw InvalidArgumentException(HERE) << "Error: the size is not set yet. Please generate the experiment.";
  return WeightedExperimentImplementation::getSize();
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
