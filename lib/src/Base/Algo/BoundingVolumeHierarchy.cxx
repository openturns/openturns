//                                               -*- C++ -*-
/**
 *  @brief  This class implements Bounding Volume Hierarchy (BVH) to speed-up point location.
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
  : EnclosingSimplexImplementation()
  , p_root_(0)
  , sortedSimplices_(0)
{
  // Nothing to do
}

/* Parameter constructor */
BoundingVolumeHierarchy::BoundingVolumeHierarchy(const Sample & vertices,
                                                 const IndicesCollection & simplices,
                                                 const UnsignedInteger binNumber,
                                                 const String strategy)
  : EnclosingSimplexImplementation(vertices, simplices)
  , p_root_(0)
  , binNumber_(binNumber)
  , strategy_(strategy)
  , sortedSimplices_(0)
{
  if (binNumber_ < 1) throw InvalidArgumentException(HERE) << "Error: binNumber must not be null";
  if (strategy_ != "Mean" && strategy_ != "Median") throw InvalidArgumentException(HERE) << "Error: strategy " << strategy << "not available, valid values are either Mean or Median";
  initialize();
}

/* Virtual constructor */
BoundingVolumeHierarchy * BoundingVolumeHierarchy::clone() const
{
  return new BoundingVolumeHierarchy(*this);
}

/* Initialize BVH */
void BoundingVolumeHierarchy::initialize()
{
  if (offsetSimplexIndices_.getSize() <= 1) return;

  const UnsignedInteger dimension = vertices_.getDimension();
  const UnsignedInteger nrSimplices = offsetSimplexIndices_.getSize() - 1;
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    if (offsetSimplexIndices_[i + 1] != offsetSimplexIndices_[i] + dimension + 1) throw InvalidArgumentException(HERE) << "All simplices must have " << (dimension + 1) << " vertices";
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
  Indices simplexIndices(nrSimplices);
  simplexIndices.fill();

  p_root_ = build(simplexIndices, 0, nrSimplices);
}

namespace {

class partitionSimplexBounds
{
public:
    partitionSimplexBounds(const Sample & centerBoundingBoxSimplices, const UnsignedInteger activeDimension, const Scalar threshold)
      : centerBoundingBoxSimplices_(centerBoundingBoxSimplices)
      , activeDimension_(activeDimension)
      , threshold_(threshold)
      {}
    bool operator() (const UnsignedInteger index) const
    {
      return centerBoundingBoxSimplices_(index, activeDimension_) < threshold_;
    }
private:
    const Sample centerBoundingBoxSimplices_;
    const UnsignedInteger activeDimension_;
    const Scalar threshold_;
};

class compareSimplexBounds
{
public:
    compareSimplexBounds(const Sample & centerBoundingBoxSimplices, const UnsignedInteger activeDimension)
      : centerBoundingBoxSimplices_(centerBoundingBoxSimplices), activeDimension_(activeDimension) {}
    bool operator() (const UnsignedInteger lhs, const UnsignedInteger rhs) const
    {
      return centerBoundingBoxSimplices_(lhs, activeDimension_) < centerBoundingBoxSimplices_(rhs, activeDimension_);
    }
private:
    const Sample centerBoundingBoxSimplices_;
    const UnsignedInteger activeDimension_;
};

void update_lower_bounds(Collection<Scalar>::iterator begin, Collection<Scalar>::iterator end, Collection<Scalar>::const_iterator otherLower)
{
  for(Collection<Scalar>::iterator it = begin; it != end; ++it, ++otherLower)
  {
    if (*it > *otherLower)
      *it = *otherLower;
  }
}

void update_upper_bounds(Collection<Scalar>::iterator begin, Collection<Scalar>::iterator end, Collection<Scalar>::const_iterator otherUpper)
{
  for(Collection<Scalar>::iterator it = begin; it != end; ++it, ++otherUpper)
  {
    if (*it < *otherUpper)
      *it = *otherUpper;
  }
}

} // anonymous namespace

BoundingVolumeHierarchy::Node::NodePointer BoundingVolumeHierarchy::build(
    Indices & simplexIndices,
    const UnsignedInteger firstIndex,
    const UnsignedInteger lastIndex)
{
  const UnsignedInteger dimension = vertices_.getDimension();
  Point lowerBounds(dimension, SpecFunc::MaxScalar);
  Point upperBounds(dimension, - SpecFunc::MaxScalar);

  const Collection<Scalar>::const_iterator lowerData = lowerBoundingBoxSimplices_.getImplementation()->data_begin();
  const Collection<Scalar>::const_iterator upperData = upperBoundingBoxSimplices_.getImplementation()->data_begin();
  for(UnsignedInteger i = firstIndex; i < lastIndex; ++i)
  {
    update_lower_bounds(lowerBounds.begin(), lowerBounds.end(), lowerData + dimension * simplexIndices[i]);
    update_upper_bounds(upperBounds.begin(), upperBounds.end(), upperData + dimension * simplexIndices[i]);
  }
  // Create a leaf if there are few simplices
  if (lastIndex - firstIndex <= binNumber_)
  {
    const UnsignedInteger offset = sortedSimplices_.getSize();
    for (UnsignedInteger i = firstIndex; i < lastIndex; ++i)
    {
      sortedSimplices_.add(simplexIndices[i]);
    }
    return new Node(offset, lastIndex - firstIndex, lowerBounds, upperBounds);
  }
  // Otherwise, we will split this node.
  // First search the minimum and maximum center.
  Point lowerMiddle(dimension, SpecFunc::MaxScalar);
  Point upperMiddle(dimension, - SpecFunc::MaxScalar);
  const Collection<Scalar>::const_iterator centerData = centerBoundingBoxSimplices_.getImplementation()->data_begin();
  for(UnsignedInteger i = firstIndex; i < lastIndex; ++i)
  {
    update_lower_bounds(lowerMiddle.begin(), lowerMiddle.end(), centerData + dimension * simplexIndices[i]);
    update_upper_bounds(upperMiddle.begin(), upperMiddle.end(), centerData + dimension * simplexIndices[i]);
  }

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
    const UnsignedInteger offset = sortedSimplices_.getSize();
    for (UnsignedInteger i = firstIndex; i < lastIndex; ++i)
    {
      sortedSimplices_.add(simplexIndices[i]);
    }
    return new Node(offset, lastIndex - firstIndex, lowerBounds, upperBounds);
  }
  UnsignedInteger middleIndex = (firstIndex + lastIndex) / 2;
  Scalar valueSplit = 0.5 * (lowerMiddle[activeDimension] + upperMiddle[activeDimension]);
  if (strategy_ == "Mean")
  {
    const Indices::iterator mid_ptr =
      std::partition(simplexIndices.begin() + firstIndex,
                     simplexIndices.begin() + lastIndex,
                     partitionSimplexBounds(centerBoundingBoxSimplices_, activeDimension, valueSplit));
    middleIndex = mid_ptr - simplexIndices.begin();
  }
  if (middleIndex == firstIndex || middleIndex == lastIndex || strategy_ == "median")
  {
    middleIndex = (firstIndex + lastIndex) / 2;
    // Mean partition failed, fall back to median
    // Sort elements in 2 parts of equal size
    std::nth_element(simplexIndices.begin() + firstIndex,
                     simplexIndices.begin() + middleIndex,
                     simplexIndices.begin() + lastIndex,
                     compareSimplexBounds(centerBoundingBoxSimplices_, activeDimension));
    valueSplit = centerBoundingBoxSimplices_(simplexIndices[middleIndex], activeDimension);
  }
  Node::NodePointer leftChild = build(simplexIndices, firstIndex, middleIndex);
  Node::NodePointer rightChild = build(simplexIndices, middleIndex, lastIndex);

  lowerBounds = leftChild->lowerBounds_;
  upperBounds = leftChild->upperBounds_;
  update_lower_bounds(lowerBounds.begin(), lowerBounds.end(), rightChild->lowerBounds_.begin());
  update_upper_bounds(upperBounds.begin(), upperBounds.end(), rightChild->upperBounds_.begin());
  return new Node(activeDimension, valueSplit, leftChild, rightChild, lowerBounds, upperBounds);
}

namespace {
inline Bool is_point_inside_bounds(const Point & point, const Point & lowerBounds, const Point & upperBounds)
{
  for(UnsignedInteger i = 0; i < point.getSize(); ++i)
  {
    if (point[i] < lowerBounds[i] || point[i] > upperBounds[i])
      return false;
  }
  return true;
}
} // anonymous namespace

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger BoundingVolumeHierarchy::getEnclosingSimplexIndex(const Point & point) const
{
  if (point.getDimension() != vertices_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << vertices_.getDimension() << ", got dimension=" << point.getDimension();

  // First, check against the bounding box
  const UnsignedInteger notFound = offsetSimplexIndices_.getSize() - 1;
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
          pairDistanceSimplex.reserve(current->nrSimplices_);
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
String BoundingVolumeHierarchy::__str__(const String & offset) const
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
  EnclosingSimplexImplementation::save(adv);
  adv.saveAttribute("binNumber_", binNumber_);
  adv.saveAttribute("strategy_", strategy_);
}


/* Method load() reloads the object from the StorageManager */
void BoundingVolumeHierarchy::load(Advocate & adv)
{
  EnclosingSimplexImplementation::load(adv);
  adv.loadAttribute("binNumber_", binNumber_);
  adv.loadAttribute("strategy_", strategy_);
  initialize();
}

END_NAMESPACE_OPENTURNS
