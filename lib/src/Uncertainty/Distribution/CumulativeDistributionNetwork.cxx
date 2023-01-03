//                                               -*- C++ -*-
/**
 *  @brief This class implements the multivariate distribution based on a
 *         bipartite graph and a collection of distributions
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
#include "openturns/CumulativeDistributionNetwork.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CumulativeDistributionNetwork)

static const Factory<CumulativeDistributionNetwork> Factory_CumulativeDistributionNetwork;

/* Default constructor */
CumulativeDistributionNetwork::CumulativeDistributionNetwork()
  : DistributionImplementation()
  , distributionCollection_(1)
  , graph_()
{
  setName("CumulativeDistributionNetwork");
  // Set an empty range
  setDistributionCollection(DistributionCollection(1));
}

/* Parameters constructor */
CumulativeDistributionNetwork::CumulativeDistributionNetwork(const DistributionCollection & coll,
    const BipartiteGraph & graph)
  : DistributionImplementation()
  , distributionCollection_()
  , graph_(graph)
{
  setName("CumulativeDistributionNetwork");
  // We could NOT set distributionCollection_ in the member area of the constructor
  // because we must check before if the collection is valid (ie, if all the
  // distributions of the collection have the same dimension). We do this by calling
  setDistributionCollection( coll );
}

/* Comparison operator */
Bool CumulativeDistributionNetwork::operator ==(const CumulativeDistributionNetwork & other) const
{
  if (this == &other) return true;
  return (distributionCollection_ == other.distributionCollection_) && (graph_ == other.graph_);
}

Bool CumulativeDistributionNetwork::equals(const DistributionImplementation & other) const
{
  const CumulativeDistributionNetwork* p_other = dynamic_cast<const CumulativeDistributionNetwork*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String CumulativeDistributionNetwork::__repr__() const
{
  OSS oss(true);
  oss << "class=" << CumulativeDistributionNetwork::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " distributionCollection=" << distributionCollection_
      << " graph=" << graph_;
  return oss;
}

String CumulativeDistributionNetwork::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "([";
  String separator("");
  for (UnsignedInteger i = 0; i < distributionCollection_.getSize(); ++i)
  {
    oss << separator << distributionCollection_[i] << ")";
    separator = ", ";
  }
  oss << "]" << graph_.__str__() << ")";
  return oss;
}

/* Compute the numerical range of the distribution given the parameters values */
void CumulativeDistributionNetwork::computeRange()
{
  const UnsignedInteger size = distributionCollection_.getSize();
  if (size == 0) return;
  const UnsignedInteger dim = getDimension();
  const Point infiniteLowerBounds(dim, SpecFunc::LowestScalar);
  const Point infiniteUpperBounds(dim,  SpecFunc::MaxScalar);
  const Interval::BoolCollection infiniteLowerBoundsFlags(dim, false);
  const Interval::BoolCollection infiniteUpperBoundsFlags(dim, false);
  Interval range(infiniteLowerBounds, infiniteUpperBounds, infiniteLowerBoundsFlags, infiniteUpperBoundsFlags);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Point lowerBounds(infiniteLowerBounds);
    Point upperBounds(infiniteUpperBounds);
    Interval::BoolCollection lowerBoundsFlags(infiniteLowerBoundsFlags);
    Interval::BoolCollection upperBoundsFlags(infiniteUpperBoundsFlags);
    Interval cdfRange(distributionCollection_[i].getRange());
    const UnsignedInteger nodeSize = graph_.cend_at(i) - graph_.cbegin_at(i);
    for (UnsignedInteger j = 0; j < nodeSize; ++j)
    {
      const UnsignedInteger index = graph_(i, j);
      lowerBounds[index] = cdfRange.getLowerBound()[j];
      upperBounds[index] = cdfRange.getUpperBound()[j];
      lowerBoundsFlags[index] = cdfRange.getFiniteLowerBound()[j];
      upperBoundsFlags[index] = cdfRange.getFiniteUpperBound()[j];
      range = range.intersect(Interval(lowerBounds, upperBounds, lowerBoundsFlags, upperBoundsFlags));
    }
  } // Red nodes
  setRange(range);
}

/* Distribution collection accessor */
void CumulativeDistributionNetwork::setDistributionCollection(const DistributionCollection & coll)
{
  const UnsignedInteger size = coll.getSize();
  // Check the number of distributions
  if (size != graph_.getRedNodes().getSize()) throw InvalidArgumentException(HERE) << "Error: the given collection of distributions has a size=" << size << " different from the number of red nodes=" << graph_.getRedNodes().getSize();
  // Check the dimension of the distributions
  Bool parallel = true;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const UnsignedInteger nodeSize = graph_.cend_at(i) - graph_.cbegin_at(i);
    if (coll[i].getDimension() != nodeSize) throw InvalidArgumentException(HERE) << "Error: the distribution " << i << " has a dimension=" << size << " which is different from the number of links=" << nodeSize << " starting from red node " << i;
    parallel = parallel && coll[i].getImplementation()->isParallel();
  }
  setParallel(parallel);
  distributionCollection_ = coll;
  LOGINFO(OSS() << "graph=" << graph_ << ", dim=" << graph_.getBlackNodes().getSize());
  setDimension(graph_.getBlackNodes().getSize());
  computeRange();
}

/* Distribution collection accessor */
CumulativeDistributionNetwork::DistributionCollection CumulativeDistributionNetwork::getDistributionCollection() const
{
  return distributionCollection_;
}

/* Graph accessor */
void CumulativeDistributionNetwork::setGraph(const BipartiteGraph & graph)
{
  const UnsignedInteger size = distributionCollection_.getSize();
  // Check the number of distributions
  if (size != graph.getRedNodes().getSize()) throw InvalidArgumentException(HERE) << "Error: the given graph has a number of red nodes=" << graph_.getRedNodes().getSize() << " different from the collection of distributions size=" << size;
  // Check the dimension of the distributions
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger nodeSize = graph.cend_at(i) - graph.cbegin_at(i);
    if (distributionCollection_[i].getDimension() != nodeSize) throw InvalidArgumentException(HERE) << "Error: the number of links=" << nodeSize << " starting from red node " << i << " is different from distribution " << i << " dimension=" << size;
  }
  graph_ = graph;
}

/* Graph accessor */
BipartiteGraph CumulativeDistributionNetwork::getGraph() const
{
  return graph_;
}

/* Virtual constructor */
CumulativeDistributionNetwork * CumulativeDistributionNetwork::clone() const
{
  return new CumulativeDistributionNetwork(*this);
}


/* Get one realization of the CumulativeDistributionNetwork */
Point CumulativeDistributionNetwork::getRealization() const
{
  return DistributionImplementation::getRealizationByInversion();
}

/* Get many realizations of the CumulativeDistributionNetwork */
Sample CumulativeDistributionNetwork::getSample(const UnsignedInteger size) const
{
  return DistributionImplementation::getSampleByInversion(size);
}

/* Extract the components of a full dimension point to feed the index distribution */
Point CumulativeDistributionNetwork::reducePoint(const Point & point,
    const UnsignedInteger index) const
{
  const UnsignedInteger size = graph_.cend_at(index) - graph_.cbegin_at(index);
  Point reducedPoint(size);
  for (UnsignedInteger i = 0; i < size; ++i) reducedPoint[i] = point[graph_(index, i)];
  return reducedPoint;
}

/* Get the PDF of the CumulativeDistributionNetwork */
Scalar CumulativeDistributionNetwork::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  return DistributionImplementation::computePDF(point);
}


/* Get the CDF of the CumulativeDistributionNetwork */
Scalar CumulativeDistributionNetwork::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Scalar cdf = distributionCollection_[0].computeCDF(reducePoint(point, 0));
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 1; i < size; ++i) cdf *= distributionCollection_[i].computeCDF(reducePoint(point, i));
  return cdf;
}


/* Get the i-th marginal distribution */
Distribution CumulativeDistributionNetwork::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  DistributionCollection contributors(0);
  Collection<Indices> marginalGraph(0);
  for (UnsignedInteger j = 0; j < distributionCollection_.getSize(); ++j)
  {
    // Check if the current contributor contains k
    UnsignedInteger localIndex = dimension;
    UnsignedInteger k = 0;
    for (IndicesCollection::const_iterator cit = graph_.cbegin_at(j); cit != graph_.cend_at(j); ++cit, ++k)
      if (i == (*cit))
      {
        localIndex = k;
        break;
      }
    // If the marginal index is in the current indices
    if (localIndex < dimension)
    {
      contributors.add(distributionCollection_[j].getMarginal(localIndex));
      marginalGraph.add(Indices(1, 0));
    }
  } // Loop over the CDFs
  if (contributors.getSize() == 1) return contributors[0].getImplementation()->clone();
  return new CumulativeDistributionNetwork(contributors, BipartiteGraph(IndicesCollectionImplementation(marginalGraph)));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution CumulativeDistributionNetwork::getMarginal(const Indices & indices) const
{
  LOGINFO(OSS() << "in getMarginal(" << indices << "), contributors=" << distributionCollection_ << ", graph=" << graph_);
  if (indices.getSize() == 1) return getMarginal(indices[0]);
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  return DistributionImplementation::getMarginal(indices);
  /*
  // General case
  DistributionCollection contributors(0);
  BipartiteGraph marginalGraph(0);
  for (UnsignedInteger j = 0; j < distributionCollection_.getSize(); ++j)
    {
      // Check if the current contributor contains k
      Indices localIndices(0);
      Indices currentIndices(graph_[j]);
      LOGINFO(OSS() << "j=" << j << ", currentIndices=" << currentIndices);
      for (UnsignedInteger k = 0; k < indices.getSize(); ++k)
  {
    const UnsignedInteger i = indices[k];
    for (UnsignedInteger n = 0; n < currentIndices.getSize(); ++n)
      if (i == currentIndices[n])
        {
  	localIndices.add(n);
  	break;
        }
    LOGINFO(OSS() << "k=" << k << ", localIndices=" << localIndices);
  } // Loop over the marginal indices
      LOGINFO(OSS() << "j=" << j << ", currentIndices=" << currentIndices << ", localIndices=" << localIndices);
      // If the marginal index is in the current indices
      if (localIndices.getSize() > 0)
  {
    contributors.add(distributionCollection_[j].getMarginal(localIndices));
    Indices graphPart(localIndices.getSize());
    graphPart.fill();
    marginalGraph.add(graphPart);
  }
    } // Loop over the CDFs
  if (contributors.getSize() == 1) return contributors[0].getImplementation()->clone();
  LOGINFO(OSS() << "in getMarginal(" << indices << "), marginal contributors=" << contributors << ", marginalGraph=" << marginalGraph);
  return new CumulativeDistributionNetwork(contributors, marginalGraph);
  */
}

/* Check if the distribution is continuous */
Bool CumulativeDistributionNetwork::isContinuous() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isContinuous()) return false;
  return true;
}

/* Check if the distribution is discrete */
Bool CumulativeDistributionNetwork::isDiscrete() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isDiscrete()) return false;
  return true;
}

/* Check if the distribution is integral */
Bool CumulativeDistributionNetwork::isIntegral() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) if (!distributionCollection_[i].isIntegral()) return false;
  return true;
}

/* Tell if the distribution has independent copula */
Bool CumulativeDistributionNetwork::hasIndependentCopula() const
{
  const UnsignedInteger size = distributionCollection_.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    if (!distributionCollection_[i].hasIndependentCopula()) return false;
  return true;
}

/* Method save() stores the object through the StorageManager */
void CumulativeDistributionNetwork::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "distributionCollection_", distributionCollection_ );
  adv.saveAttribute( "graph_", graph_ );
}

/* Method load() reloads the object from the StorageManager */
void CumulativeDistributionNetwork::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "distributionCollection_", distributionCollection_ );
  adv.loadAttribute( "graph_", graph_ );
  // To compute the range
  computeRange();
}

END_NAMESPACE_OPENTURNS
