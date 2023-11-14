//                                               -*- C++ -*-
/**
 *  @brief  This class implements Bounding Volume Hierarchy (BVH) to speed-up point location.
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BoundingVolumeHierarchy.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

#include <algorithm>
#include <stack>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoundingVolumeHierarchy
 */

CLASSNAMEINIT(BoundingVolumeHierarchy)

static const Factory<BoundingVolumeHierarchy> Factory_BoundingVolumeHierarchy;

/* Constructor without parameters */
BoundingVolumeHierarchy::BoundingVolumeHierarchy()
  : EnclosingSimplexAlgorithmImplementation()
  , p_root_(0)
  , binNumber_(ResourceMap::GetAsUnsignedInteger("BoundingVolumeHierarchy-BinNumber"))
  , strategy_(ResourceMap::GetAsString("BoundingVolumeHierarchy-Strategy"))
  , sortedSimplices_(0)
  , centerBoundingBoxSimplices_()
{
  // Nothing to do
}

/* Parameter constructor */
BoundingVolumeHierarchy::BoundingVolumeHierarchy(const Sample & vertices,
    const IndicesCollection & simplices,
    const UnsignedInteger binNumber,
    const String strategy)
  : EnclosingSimplexAlgorithmImplementation()
  , p_root_(0)
  , binNumber_(binNumber)
  , strategy_(strategy)
  , sortedSimplices_(0)
  , centerBoundingBoxSimplices_()
{
  if (binNumber_ < 1) throw InvalidArgumentException(HERE) << "Error: binNumber must not be null";
  if (strategy_ != "Mean" && strategy_ != "Median") throw InvalidArgumentException(HERE) << "Error: strategy " << strategy << "not available, valid values are either Mean or Median";
  setVerticesAndSimplices(vertices, simplices);
}

/* Virtual constructor */
BoundingVolumeHierarchy * BoundingVolumeHierarchy::clone() const
{
  return new BoundingVolumeHierarchy(*this);
}

BoundingVolumeHierarchy * BoundingVolumeHierarchy::emptyClone() const
{
  return new BoundingVolumeHierarchy();
}

/* Initialize BVH */
void BoundingVolumeHierarchy::setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices)
{
  EnclosingSimplexAlgorithmImplementation::setVerticesAndSimplices(vertices, simplices);

  const UnsignedInteger nrSimplices = simplices_.getSize();
  if (!nrSimplices) return;

  const UnsignedInteger dimension = vertices_.getDimension();
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    if (simplices_.cend_at(i) != simplices_.cbegin_at(i) + dimension + 1) throw InvalidArgumentException(HERE) << "All simplices must have " << (dimension + 1) << " vertices";
  }

  centerBoundingBoxSimplices_ = Sample(nrSimplices, dimension);
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    for(UnsignedInteger k = 0; k < dimension; ++k)
    {
      centerBoundingBoxSimplices_(i, k) = 0.5 * (lowerBoundingBoxSimplices_(i, k) + upperBoundingBoxSimplices_(i, k));
    }
  }

  // Recursively build tree
  UnsignedInteger sorted = 0;
  sortedSimplices_.resize(nrSimplices);
  sortedSimplices_.fill();

  p_root_ = build(sorted, 0, nrSimplices);
}

namespace
{

class PartitionSimplexBounds
{
public:
  PartitionSimplexBounds(const Sample & centerBoundingBoxSimplices, const UnsignedInteger activeDimension, const Scalar threshold)
    : centerBoundingBoxSimplices_(centerBoundingBoxSimplices)
    , activeDimension_(activeDimension)
    , threshold_(threshold)
  {}
  bool operator() (const UnsignedInteger index) const
  {
    return centerBoundingBoxSimplices_(index, activeDimension_) < threshold_;
  }
private:
  const Sample & centerBoundingBoxSimplices_;
  const UnsignedInteger activeDimension_;
  const Scalar threshold_;
};

class CompareSimplexBounds
{
public:
  CompareSimplexBounds(const Sample & centerBoundingBoxSimplices, const UnsignedInteger activeDimension)
    : centerBoundingBoxSimplices_(centerBoundingBoxSimplices), activeDimension_(activeDimension) {}
  bool operator() (const UnsignedInteger lhs, const UnsignedInteger rhs) const
  {
    return centerBoundingBoxSimplices_(lhs, activeDimension_) < centerBoundingBoxSimplices_(rhs, activeDimension_);
  }
private:
  const Sample & centerBoundingBoxSimplices_;
  const UnsignedInteger activeDimension_;
};

void update_lower_bounds(Collection<Scalar>::iterator begin, Collection<Scalar>::iterator end, Collection<Scalar>::const_iterator otherLower)
{
  for(Collection<Scalar>::iterator it = begin; it != end; ++it, ++otherLower)
  {
    *it = std::min(*it, *otherLower);
  }
}

void update_upper_bounds(Collection<Scalar>::iterator begin, Collection<Scalar>::iterator end, Collection<Scalar>::const_iterator otherUpper)
{
  for(Collection<Scalar>::iterator it = begin; it != end; ++it, ++otherUpper)
  {
    *it = std::max(*it, *otherUpper);
  }
}

} // anonymous namespace

/* Recursively build tree  */
BoundingVolumeHierarchy::Node::NodePointer BoundingVolumeHierarchy::build(
  UnsignedInteger & sorted,
  const UnsignedInteger firstIndex,
  const UnsignedInteger lastIndex)
{
  // This node contains all simplices from sortedSimplices_[firstIndex] to sortedSimplices_[lastIndex-1]
  // First compute its bounding box.
  const UnsignedInteger dimension = vertices_.getDimension();
  Point lowerBounds(dimension, SpecFunc::MaxScalar);
  Point upperBounds(dimension, - SpecFunc::MaxScalar);

  const Collection<Scalar>::const_iterator lowerData = lowerBoundingBoxSimplices_.getImplementation()->data_begin();
  const Collection<Scalar>::const_iterator upperData = upperBoundingBoxSimplices_.getImplementation()->data_begin();
  for(UnsignedInteger i = firstIndex; i < lastIndex; ++i)
  {
    update_lower_bounds(lowerBounds.begin(), lowerBounds.end(), lowerData + dimension * sortedSimplices_[i]);
    update_upper_bounds(upperBounds.begin(), upperBounds.end(), upperData + dimension * sortedSimplices_[i]);
  }
  // Create a leaf if there are few simplices
  if (lastIndex - firstIndex <= binNumber_)
  {
    sorted = lastIndex;
    return new Node(firstIndex, lastIndex - firstIndex, lowerBounds, upperBounds);
  }
  // Otherwise, we will split this node.
  // First search the minimum and maximum center.
  Point lowerMiddle(dimension, SpecFunc::MaxScalar);
  Point upperMiddle(dimension, - SpecFunc::MaxScalar);
  const Collection<Scalar>::const_iterator centerData = centerBoundingBoxSimplices_.getImplementation()->data_begin();
  for(UnsignedInteger i = firstIndex; i < lastIndex; ++i)
  {
    update_lower_bounds(lowerMiddle.begin(), lowerMiddle.end(), centerData + dimension * sortedSimplices_[i]);
    update_upper_bounds(upperMiddle.begin(), upperMiddle.end(), centerData + dimension * sortedSimplices_[i]);
  }

  // We split in the largest dimension
  UnsignedInteger activeDimension = 0;
  Scalar maxDelta = upperMiddle[0] - lowerMiddle[0];
  for(UnsignedInteger k = 1; k < dimension; ++k)
  {
    const Scalar delta = upperMiddle[k] - lowerMiddle[k];
    if (delta > maxDelta)
    {
      maxDelta = delta;
      activeDimension = k;
    }
  }
  if (upperMiddle[activeDimension] == lowerMiddle[activeDimension])
  {
    // All centers are at the same location; they cannot be distinguished,
    // thus there is no reason to split nodes
    sorted = lastIndex;
    return new Node(firstIndex, lastIndex - firstIndex, lowerBounds, upperBounds);
  }
  UnsignedInteger middleIndex = (firstIndex + lastIndex) / 2;
  Scalar valueSplit = 0.5 * (lowerMiddle[activeDimension] + upperMiddle[activeDimension]);
  if (strategy_ == "Mean")
  {
    // Sort sortedSimplices_[firstIndex]..sortedSimplices_[lastIndex-1] such that
    // all simplices from sortedSimplices_[firstIndex]..sortedSimplices_[middleIndex-1] are at
    // the left of simplices from sortedSimplices_[middleIndex]..sortedSimplices_[lastIndex-1]
    // along axis activeDimension.
    const Indices::iterator mid_ptr =
      std::partition(sortedSimplices_.begin() + firstIndex,
                     sortedSimplices_.begin() + lastIndex,
                     PartitionSimplexBounds(centerBoundingBoxSimplices_, activeDimension, valueSplit));
    middleIndex = mid_ptr - sortedSimplices_.begin();
  }
  if (middleIndex == firstIndex || middleIndex == lastIndex || strategy_ != "Mean")
  {
    middleIndex = (firstIndex + lastIndex) / 2;
    // Mean partition failed, fall back to median.
    // Sort elements in 2 parts of equal size.
    std::nth_element(sortedSimplices_.begin() + firstIndex,
                     sortedSimplices_.begin() + middleIndex,
                     sortedSimplices_.begin() + lastIndex,
                     CompareSimplexBounds(centerBoundingBoxSimplices_, activeDimension));
    valueSplit = centerBoundingBoxSimplices_(sortedSimplices_[middleIndex], activeDimension);
  }
  Node::NodePointer leftChild = build(sorted, firstIndex, middleIndex);
  Node::NodePointer rightChild = build(sorted, middleIndex, lastIndex);

  lowerBounds = leftChild->lowerBounds_;
  upperBounds = leftChild->upperBounds_;
  update_lower_bounds(lowerBounds.begin(), lowerBounds.end(), rightChild->lowerBounds_.begin());
  update_upper_bounds(upperBounds.begin(), upperBounds.end(), rightChild->upperBounds_.begin());
  return new Node(activeDimension, valueSplit, leftChild, rightChild, lowerBounds, upperBounds);
}

namespace
{
inline Bool is_point_inside_bounds(const Point & point, const Point & lowerBounds, const Point & upperBounds)
{
  for(UnsignedInteger i = 0; i < point.getSize(); ++i)
  {
    if (!(point[i] >= lowerBounds[i] && point[i] <= upperBounds[i]))
      return false;
  }
  return true;
}
} // anonymous namespace

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger BoundingVolumeHierarchy::query(const Point & point) const
{
  if (point.getDimension() != vertices_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << vertices_.getDimension() << ", got dimension=" << point.getDimension();

  // First, check against the bounding box
  const UnsignedInteger size = simplices_.getSize();
  const UnsignedInteger notFound = size;

  if (!size) return notFound;

  if (!boundingBox_.contains(point)) return notFound;

  const UnsignedInteger dimension = point.getDimension();
  std::stack<Node::NodePointer> toVisit;
  toVisit.push(p_root_);
  SquareMatrix simplexMatrix(dimension + 1);
  std::vector< std::pair<Scalar, UnsignedInteger> > pairDistanceSimplex(binNumber_);
  while(!toVisit.empty())
  {
    Node::NodePointer current = toVisit.top();
    toVisit.pop();
    if (is_point_inside_bounds(point, current->lowerBounds_, current->upperBounds_))
    {
      if (current->nrSimplices_ > 0)
      {
        // Leaf node.
        // Sort simplices according to their distance to point[activeDimension]
        // in order to (hopefully) check less simplices
        if (current->nrSimplices_ > pairDistanceSimplex.size())
          pairDistanceSimplex.resize(current->nrSimplices_);
        for(UnsignedInteger i = 0; i < current->nrSimplices_; ++i)
        {
          const UnsignedInteger simplexIndex = sortedSimplices_[(current->offset_ + i)];
          const Scalar middleValue = 0.5 * (lowerBoundingBoxSimplices_(simplexIndex, current->activeDimension_)
                                            + upperBoundingBoxSimplices_(simplexIndex, current->activeDimension_));
          pairDistanceSimplex[i] = std::make_pair(std::abs(point[current->activeDimension_] - middleValue), simplexIndex);
        }
        // First value contains distance in order to use standard comparator for std::pair
        std::sort(pairDistanceSimplex.begin(), pairDistanceSimplex.begin() + current->nrSimplices_);
        for(UnsignedInteger i = 0; i < current->nrSimplices_; ++i)
        {
          const UnsignedInteger simplexIndex = pairDistanceSimplex[i].second;
          if (checkPointInSimplex(point, simplexIndex, simplexMatrix))
          {
            return simplexIndex;
          }
        }
      }
      else
      {
        // Inner node, visit children
        if (point[current->activeDimension_] < current->value_)
        {
          // Visit left child first
          toVisit.push(current->p_right_);
          toVisit.push(current->p_left_);
        }
        else
        {
          // Visit right child first
          toVisit.push(current->p_left_);
          toVisit.push(current->p_right_);
        }
      }
    }
  }
  return notFound;
}

/* String converter */
String BoundingVolumeHierarchy::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BoundingVolumeHierarchy::GetClassName()
      << " binNumber=" << binNumber_
      << " strategy=" << strategy_;
  return oss;
}

/* String converter */
String BoundingVolumeHierarchy::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << BoundingVolumeHierarchy::GetClassName()
      << " binNumber=" << binNumber_
      << " strategy=" << strategy_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void BoundingVolumeHierarchy::save(Advocate & adv) const
{
  EnclosingSimplexAlgorithmImplementation::save(adv);
  adv.saveAttribute("binNumber_", binNumber_);
  adv.saveAttribute("strategy_", strategy_);
}


/* Method load() reloads the object from the StorageManager */
void BoundingVolumeHierarchy::load(Advocate & adv)
{
  EnclosingSimplexAlgorithmImplementation::load(adv);
  adv.loadAttribute("binNumber_", binNumber_);
  adv.loadAttribute("strategy_", strategy_);
  setVerticesAndSimplices(vertices_, simplices_);
}

END_NAMESPACE_OPENTURNS
