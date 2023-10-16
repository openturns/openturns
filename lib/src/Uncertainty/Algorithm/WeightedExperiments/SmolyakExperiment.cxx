//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an Smolyak experiment
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
#include "openturns/SmolyakExperiment.hxx"
#include "openturns/TensorProductExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ComposedDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SmolyakExperiment)

/* Default constructor */
SmolyakExperiment::SmolyakExperiment()
  : WeightedExperimentImplementation()
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
{
  if (level == 0) throw InvalidArgumentException(HERE) << "Error: the level is zero";
  const UnsignedInteger numberOfMarginalExperiments = collection_.getSize();
  Collection<Distribution> distributionCollection(numberOfMarginalExperiments);
  for (UnsignedInteger i = 0; i < numberOfMarginalExperiments; ++ i)
  {
    distributionCollection[i] = collection_[i].getDistribution();
    const UnsignedInteger marginalDimension = distributionCollection[i].getDimension();
    if (marginalDimension != 1) throw InvalidArgumentException(HERE) << "Error: the marginal with index " << i << " has dimension " << marginalDimension << " which is different from 1";
    if (collection_[i].isRandom()) throw InvalidArgumentException(HERE) << "Error: the marginal with index " << i << " is random.";
  }
  const ComposedDistribution distribution(distributionCollection);
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
  // level=1 => only one node
  return (level_ == 1);
}

/* Sample generation */
Sample SmolyakExperiment::generate() const
{
  Point weights;
  return generateWithWeights(weights);
}

/* Compute the minimum of a multiindex */
UnsignedInteger SmolyakExperiment::indicesMinimum(const Indices & indices) const
{
  const UnsignedInteger dimension = indices.getSize();
  UnsignedInteger multiIndexMin = indices[0];
  for (UnsignedInteger j = 1; j < dimension; ++ j) multiIndexMin = std::min(indices[j], multiIndexMin);
  return multiIndexMin;
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
  const UnsignedInteger cardinalMax = enumerateFunction.getStrataCumulatedCardinal(level_ - 1);
  const UnsignedInteger cardinalMin = enumerateFunction.getStrataCumulatedCardinal(level_ - dimension - 1);
  const UnsignedInteger combinationIndicesCollectionSize = cardinalMax - cardinalMin;
  LOGDEBUG(OSS() << "  combinationIndicesCollectionSize = " << combinationIndicesCollectionSize);
  // Fill the indices
  IndicesCollection combinationIndicesCollection(combinationIndicesCollectionSize, dimension);
  UnsignedInteger multiIndexIndex = 0;
  const UnsignedInteger strataIndexMin = (level_ < dimension) ? 0 : level_ - dimension;
  LOGDEBUG(OSS() << "  strataIndexMin = " << strataIndexMin);
  LOGDEBUG(OSS() << "  level_ = " << level_);
  for (UnsignedInteger strataIndex = strataIndexMin; strataIndex < level_; ++ strataIndex)
  {
    LOGDEBUG(OSS() << "  strataIndex = " <<  strataIndex);
    const UnsignedInteger strataCardinal = enumerateFunction.getStrataCardinal(strataIndex);
    const UnsignedInteger cumulatedCardinal = enumerateFunction.getStrataCumulatedCardinal(
          strataIndex
        );
    const UnsignedInteger indexStart = cumulatedCardinal - strataCardinal;
    for (UnsignedInteger i = indexStart; i < cumulatedCardinal; ++ i)
    {
      Indices indices(enumerateFunction(i));
      for (UnsignedInteger j = 0; j < dimension; ++ j) indices[j] += 1;
      LOGDEBUG(OSS() << "  indices = " <<  indices);
      std::copy(indices.begin(), indices.end(), combinationIndicesCollection.begin_at(multiIndexIndex));
      ++ multiIndexIndex;
    } // loop over the indices in the strata
  } // loop over the strata
  return combinationIndicesCollection;
}

/* Comparison class with std:map interface. */
class PointApproximateComparison
{
public:
  PointApproximateComparison(const Scalar absoluteEpsilon,
                             const Scalar relativeEpsilon)
    : absoluteEpsilon_(absoluteEpsilon)
    , relativeEpsilon_(relativeEpsilon)
  {
    // Nothing to do
  }

  /* Compare two points, according to lexicographic order
  * Returns true if x < y, false otherwise.
  */
  bool operator()(const Point x, const Point y) const
  {
    const UnsignedInteger dimension = x.getDimension();
    if (y.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the two points must have the same dimension. Here x has dimension " << dimension << " while y has dimension " << y.getDimension();
    bool comparison = false;
    for (UnsignedInteger k = 0; k < dimension; ++ k)
    {
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
    } // Loop over the dimensions
    return comparison;
  }
private:
  // Absolute tolerance for comparison
  Scalar absoluteEpsilon_;
  // Relative tolerance for comparison
  Scalar relativeEpsilon_;
};

// Implement merge with std::map
void SmolyakExperiment::mergeNodesAndWeights(Sample & nodes, Point & weights) const
{
  LOGDEBUG(OSS() << "SmolyakExperiment::mergeNodesAndWeights()");
  const Scalar relativeEpsilon = ResourceMap::GetAsScalar("SmolyakExperiment-MergeRelativeEpsilon");
  const Scalar absoluteEpsilon = ResourceMap::GetAsScalar("SmolyakExperiment-MergeAbsoluteEpsilon");
  const UnsignedInteger duplicatedSize = nodes.getSize();
  LOGDEBUG(OSS() << "Number of (potentially) duplicated nodes =" << duplicatedSize);
  if (weights.getDimension() != duplicatedSize) throw InvalidArgumentException(HERE) << "Error: the weights must have dimension " << duplicatedSize << " but have dimension " << weights.getDimension();
  UnsignedInteger dimension = nodes.getDimension();
  // Fill the map
  std::map<Point, Scalar, PointApproximateComparison> nodeWeightMap(PointApproximateComparison(absoluteEpsilon, relativeEpsilon));
  for (UnsignedInteger i = 0; i < duplicatedSize; ++ i)
  {
    std::map<Point, Scalar>::iterator search = nodeWeightMap.find(nodes[i]);
    if (search != nodeWeightMap.end())
    {
      LOGDEBUG(OSS() << "[" << i << "], found     : " << search->first << " = " << search->second);
      search->second += weights[i];
    }
    else
    {
      LOGDEBUG(OSS() << "[" << i << "], not found : " << nodes[i]);
      nodeWeightMap[nodes[i]] = weights[i];
    }
  } // Loop over (potentially) duplicated nodes, weights
  // Extract the map
  const UnsignedInteger size = nodeWeightMap.size();
  LOGDEBUG(OSS() << "size = " <<  size);
  LOGDEBUG(OSS() << "Extract the map");
  nodes = Sample(size, dimension);
  weights = Point(size);
  if (size > 0)
  {
    UnsignedInteger index = 0;
    for (std::map<Point, Scalar>::iterator it = nodeWeightMap.begin(); it != nodeWeightMap.end(); ++ it)
    {
      LOGDEBUG(OSS() << "[" << index << "], add " << it->first << " = " << it->second);
      nodes[index] = it->first;
      weights[index] = it->second;
      ++ index;
    } // Loop over unique nodes, weights in map
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
Sample SmolyakExperiment::generateWithWeights(Point & weights) const
{
  LOGDEBUG(OSS() << "SmolyakExperiment::computeNodesAndWeights()");
  const UnsignedInteger dimension = collection_.getSize();
  LOGDEBUG(OSS() << "  dimension = " << dimension);
  if (!(dimension > 0))
    throw InvalidArgumentException(HERE) << "Error: expected a positive number of marginal experiments, here it is " << dimension;
  // Compute multiindex set
  const IndicesCollection combinationIndicesCollection(computeCombination());
  LOGDEBUG(OSS() << "  combinationIndicesCollection = " << combinationIndicesCollection);
  // Create elementary Smolyak quadratures
  Sample nodes(0, dimension);
  weights.clear();
  const UnsignedInteger numberOfUnitaryQuadratures = combinationIndicesCollection.getSize();
  for (UnsignedInteger i = 0; i < numberOfUnitaryQuadratures; ++ i)
  {
    WeightedExperimentCollection collection(collection_);
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      collection[j].setSize(combinationIndicesCollection(i, j));
    TensorProductExperiment elementaryExperiment(collection);
    Point elementaryWeights(0);
    const Sample elementaryNodes(elementaryExperiment.generateWithWeights(elementaryWeights));
    // Compute Smolyak coefficient
    UnsignedInteger marginalLevelsSum = 0;
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      marginalLevelsSum += combinationIndicesCollection(i, j);
    const UnsignedInteger exponent = level_ + dimension - marginalLevelsSum - 1;
    const Scalar smolyakSign = (exponent % 2 == 0) ? 1.0 : -1.0;
    const UnsignedInteger binomial = SpecFunc::BinomialCoefficient(dimension - 1, marginalLevelsSum - level_);
    const Scalar smolyakFactor = smolyakSign * binomial;
    nodes.add(elementaryNodes);
    weights.add(smolyakFactor * elementaryWeights);
  } // Loop over the marginal levels
  // Reduce to unique nodes and weights
  if (ResourceMap::GetAsBool("SmolyakExperiment-MergeQuadrature"))
    mergeNodesAndWeights(nodes, weights);
  return nodes;
}

/* Distribution collection accessor */
SmolyakExperiment::WeightedExperimentCollection SmolyakExperiment::getExperimentCollection() const
{
  return collection_;
}

/* Distribution collection accessor */
void SmolyakExperiment::setExperimentCollection(const WeightedExperimentCollection & coll)
{
  collection_ = coll;
}

/** Distribution collection accessor */
void SmolyakExperiment::setLevel(const UnsignedInteger level)
{
  level_ = level;
}

UnsignedInteger SmolyakExperiment::getLevel() const
{
  return level_;
}

/* Size accessor */
void SmolyakExperiment::setSize(const UnsignedInteger /*size*/)
{
  throw NotDefinedException(HERE) << "in SmolyakExperiment::setSize";
}

UnsignedInteger SmolyakExperiment::getSize() const
{
  // only known at generation time
  return generate().getSize();
}

/* Compare two points approximately
   This is for testing purposes only. */
Bool SmolyakExperiment::ComparePointsApproximately(const Point & x, const Point & y)
{
  const Scalar relativeEpsilon = ResourceMap::GetAsScalar("SmolyakExperiment-MergeRelativeEpsilon");
  const Scalar absoluteEpsilon = ResourceMap::GetAsScalar("SmolyakExperiment-MergeAbsoluteEpsilon");
  const PointApproximateComparison comparison(absoluteEpsilon, relativeEpsilon);
  return comparison(x, y);
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
