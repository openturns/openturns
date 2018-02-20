//                                               -*- C++ -*-
/**
 *  @brief KDTree structure to speed-up queries on large samples
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

#include "openturns/KDTree.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Indices.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KDTree)

static const Factory<KDTree> Factory_KDTree;

/**
 * @class KDNearestNeighboursFinder
 *
 * A fixed-size heap to find k-nearest neighbours in a KDTree
 */
class KDNearestNeighboursFinder
{
public:

  /** Constructor */
  KDNearestNeighboursFinder(const Sample & sample, const Interval & boundingBox, const UnsignedInteger size)
    : sample_(sample)
    , boundingBox_(boundingBox)
    , capacity_(size)
    , size_(0)
    , values_(size)
    , indices_(size)
  {
    // Initialize values_[0] to a valid value
    values_[0] = SpecFunc::MaxScalar;
  }

  /** Get the indices of the k nearest neighbours of the given point */
  Indices getNearestNeighboursIndices(const KDTree::KDNode::KDNodePointer & p_node, const Point & x, const Bool sorted)
  {
    if (size_ != 0)
    {
      // Clear heap
      indices_.clear();
      values_.clear();
      size_ = 0;
      values_[0] = SpecFunc::MaxScalar;
    }
    Point lowerBoundingBox(boundingBox_.getLowerBound());
    Point upperBoundingBox(boundingBox_.getUpperBound());
    collectNearestNeighbours(p_node, x, lowerBoundingBox, upperBoundingBox, 0);
    if (sorted)
    {
      /* Sort heap in-place in ascending order.
         This breaks heap structure, but it does not matter, heap is
         rebuilt when calling collectNearestNeighbours.
       */
      const UnsignedInteger realSize = size_;
      while (size_ > 1)
      {
        // Move largest value at the end
        std::swap(values_[size_ - 1], values_[0]);
        std::swap(indices_[size_ - 1], indices_[0]);
        // Make heap believe that largest value has been removed
        --size_;
        // Move new root to a valid location
        moveNodeDown(0);
      }
      // Restore heap size
      size_ = realSize;
    }

    return indices_;
  }

private:
  /* Recursive method to find the indices of the k nearest neighbours
     Strategy:
     + for a new candidate, if there is still room just add it to the list of neighbours
     + else replace the worst candidate from the current list by the new candidate
     Complexity: O(k) at each insertion, O(log(n)) expected insertions
  */
  void collectNearestNeighbours(const KDTree::KDNode::KDNodePointer & p_node,
                                const Point & x,
                                Point & lowerBoundingBox, Point & upperBoundingBox,
                                const UnsignedInteger activeDimension)
  {
    const Scalar splitValue = sample_(p_node->index_, activeDimension);
    const Scalar delta = x[activeDimension] - splitValue;
    const KDTree::KDNode::KDNodePointer & sameSide(delta < 0.0 ? p_node->p_left_ : p_node->p_right_);
    const KDTree::KDNode::KDNodePointer & oppositeSide(delta < 0.0 ? p_node->p_right_ : p_node->p_left_);
    const UnsignedInteger dimension = sample_.getDimension();
    const UnsignedInteger nextActiveDimension = (activeDimension + 1) % dimension;
    const Scalar saveLower = lowerBoundingBox[activeDimension];
    const Scalar saveUpper = upperBoundingBox[activeDimension];
    Scalar currentGreatestValidSquaredDistance = values_[0];
    if (sameSide != 0)
    {
      // Update bounding box to match sameSide bounding box
      if (delta < 0.0)
        upperBoundingBox[activeDimension] = splitValue;
      else
        lowerBoundingBox[activeDimension] = splitValue;
      // Compute distance between x and sameSide
      Scalar squaredDistanceBoundingBox = 0.0;
      for(UnsignedInteger i = 0; i < dimension; ++i)
      {
        Scalar difference = std::max(0.0, std::max(x[i] - upperBoundingBox[i], lowerBoundingBox[i] - x[i]));
        squaredDistanceBoundingBox += difference * difference;
      }
      if (squaredDistanceBoundingBox < values_[0])
      {
        collectNearestNeighbours(sameSide, x, lowerBoundingBox, upperBoundingBox, nextActiveDimension);
        currentGreatestValidSquaredDistance = values_[0];
      }
      // Restore bounding box
      if (delta < 0.0)
        upperBoundingBox[activeDimension] = saveUpper;
      else
        lowerBoundingBox[activeDimension] = saveLower;
    }
    if (size_ == capacity_ && currentGreatestValidSquaredDistance < delta * delta) return;
    const UnsignedInteger localIndex = p_node->index_;
    // Similar to (x - sample_[localIndex]).normSquare() but it is better to avoid Point creation
    Scalar localSquaredDistance = 0.0;
    for(UnsignedInteger i = 0; i < dimension; ++i)
      localSquaredDistance += (x[i] - sample_(localIndex, i)) * (x[i] - sample_(localIndex, i));
    if (size_ != capacity_)
    {
      // Put index/value at the first free node and move it up to a valid location
      indices_[size_] = localIndex;
      values_[size_] = localSquaredDistance;
      moveNodeUp(size_);
      ++size_;
    }
    else if (localSquaredDistance < currentGreatestValidSquaredDistance)
    {
      // Heap is full, and current value is smaller than heap largest value.
      // Replace the largest value by current value and move it down to a
      // valid location.
      if (localSquaredDistance < values_[0])
      {
        indices_[0] = localIndex;
        values_[0] = localSquaredDistance;
        moveNodeDown(0);
      }
    }
    if (oppositeSide != 0)
    {
      // Update bounding box to match oppositeSide bounding box
      if (delta < 0.0)
        lowerBoundingBox[activeDimension] = splitValue;
      else
        upperBoundingBox[activeDimension] = splitValue;
      // Compute distance between x and oppositeSide
      Scalar squaredDistanceBoundingBox = 0.0;
      for(UnsignedInteger i = 0; i < dimension; ++i)
      {
        Scalar difference = std::max(0.0, std::max(x[i] - upperBoundingBox[i], lowerBoundingBox[i] - x[i]));
        squaredDistanceBoundingBox += difference * difference;
      }
      if (squaredDistanceBoundingBox < values_[0])
        collectNearestNeighbours(oppositeSide, x, lowerBoundingBox, upperBoundingBox, nextActiveDimension);
      // Restore bounding box
      if (delta < 0.0)
        lowerBoundingBox[activeDimension] = saveLower;
      else
        upperBoundingBox[activeDimension] = saveUpper;
    }
  }

  /** Move node down to its final location */
  void moveNodeDown(const UnsignedInteger index)
  {
    const UnsignedInteger left = (index << 1) + 1;
    const UnsignedInteger right = left + 1;
    UnsignedInteger maxValueIndex = index;
    if (left < size_ && values_[left] > values_[maxValueIndex])
    {
      maxValueIndex = left;
    }
    if (right < size_ && values_[right] > values_[maxValueIndex])
    {
      maxValueIndex = right;
    }
    if (index != maxValueIndex)
    {
      std::swap(values_[index], values_[maxValueIndex]);
      std::swap(indices_[index], indices_[maxValueIndex]);
      moveNodeDown(maxValueIndex);
    }
  }

  /** Move node up to its final location */
  void moveNodeUp(const UnsignedInteger index)
  {
    if (index == 0) return;
    const UnsignedInteger parent = (index - 1) >> 1;
    if (values_[index] > values_[parent])
    {
      std::swap(values_[index], values_[parent]);
      std::swap(indices_[index], indices_[parent]);
      moveNodeUp(parent);
    }
  }

  // Points contained in the tree
  const Sample sample_;
  // Points bounding box
  const Interval boundingBox_;
  // Maximum heap size
  const UnsignedInteger capacity_;
  // Number of used buckets
  UnsignedInteger size_;
  // Array containing values
  Collection<Scalar> values_;
  // Array containing point indices
  Indices indices_;

}; /* class KDNearestNeighboursFinder */

/**
 * @class KDTree
 *
 * Organize d-dimensional points into a hierarchical tree-like structure
 */

/* Default constructor */
KDTree::KDTree()
  : NearestNeighbourAlgorithmImplementation()
  , points_(0, 0)
  , boundingBox_()
  , p_root_(0)
{
  // Nothing to do
}

/* Parameters constructor */
KDTree::KDTree(const Sample & points)
  : NearestNeighbourAlgorithmImplementation()
  , points_(0, 0)
  , boundingBox_()
  , p_root_(0)
{
  // Build the tree
  setSample(points);
}

/* Sample accessor */
Sample KDTree::getSample() const
{
  return points_;
}

void KDTree::setSample(const Sample & points)
{
  if (points == points_) return;

  // Reset tree if it contains old points
  if (!p_root_.isNull())
  {
    p_root_.reset();
  }

  points_ = points;
  boundingBox_ = Interval(points_.getDimension());

  // Scramble the order in which the points are inserted in the tree in order to improve its balancing
  const UnsignedInteger size = points_.getSize();
  Indices buffer(size);
  buffer.fill();
  SobolSequence sequence(1);
  for (UnsignedInteger i = 0; i < points_.getSize(); ++ i)
  {
    const UnsignedInteger index = i + static_cast< UnsignedInteger >((size - i) * sequence.generate()[0]);
    insert(p_root_, buffer[index], 0);
    buffer[index] = buffer[i];
  }
  boundingBox_.setLowerBound(points_.getMin());
  boundingBox_.setUpperBound(points_.getMax());
}

/* Virtual constructor */
KDTree * KDTree::clone() const
{
  return new KDTree( *this );
}

/* Virtual default constructor */
KDTree * KDTree::emptyClone() const
{
  return new KDTree();
}

/* String converter */
String KDTree::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " root=" << (p_root_ ? p_root_->__repr__() : "NULL");
}

String KDTree::__str__() const
{
  return OSS(false) << "class=" << GetClassName()
         << " root=" << (p_root_ ? p_root_->__repr__() : "NULL");
}

/* Insert the point at given index into the tree */
void KDTree::insert(KDNode::KDNodePointer & p_node,
                    const UnsignedInteger index,
                    const UnsignedInteger activeDimension)
{
  if (index >= points_.getSize()) throw InvalidArgumentException(HERE) << "Error: expected an index less than " << points_.getSize() << ", got " << index;
  // We are on a leaf
  if (p_node == 0) p_node = new KDNode(index);
  else if (points_(index, activeDimension) < points_(p_node->index_, activeDimension)) insert(p_node->p_left_, index, (activeDimension + 1) % points_.getDimension());
  else insert(p_node->p_right_, index, (activeDimension + 1) % points_.getDimension());
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger KDTree::query(const Point & x) const
{
  if (points_.getSize() == 1) return 0;
  Scalar smallestDistance = SpecFunc::MaxScalar;
  Point lowerBoundingBox(boundingBox_.getLowerBound());
  Point upperBoundingBox(boundingBox_.getUpperBound());
  return getNearestNeighbourIndex(p_root_, x, smallestDistance, lowerBoundingBox, upperBoundingBox, 0);
}

UnsignedInteger KDTree::getNearestNeighbourIndex(const KDNode::KDNodePointer & p_node,
    const Point & x,
    Scalar & bestSquaredDistance,
    Point & lowerBoundingBox,
    Point & upperBoundingBox,
    const UnsignedInteger activeDimension) const
{
  if (p_node == 0) throw NotDefinedException(HERE) << "Error: cannot find a nearest neighbour in an emty tree";
  // Set delta = x[activeDimension] - points_(p_node->index_, activeDimension)
  // sameSide = p_node->p_left_ if delta < 0, p_node->p_right_ else
  // oppositeSide = p_node->p_right_ if delta < 0, p_node->p_left_ else
  // Possibilities:
  // 1) sameSide != 0
  // 1.1) Go on the same side. On return, the index is the best candidate index. If the associated distance is less than the current best index, update the current best index and the associated squared distance.
  // 2) Check if the current best squared distance is less than delta^2
  // 2.1*) If yes, the points associated to p_node or to its opposite side can't be better than the current best candidate so return it and the associated squared distance to the upper level
  // 2.2) If no, check the point associated to the current node and update the current best index and the associated squared distance
  // 2.3) oppositeSide != 0
  // 2.4) Go on the opposite side. On return, check if the returned squared distance is less than the current best squared distance and update the current best index and the associated squared distance.
  // 3*) Return the current best index and the associated squared distance to the upper level

  const Scalar splitValue = points_(p_node->index_, activeDimension);
  const Scalar delta = x[activeDimension] - splitValue;
  const KDNode::KDNodePointer & sameSide(delta < 0.0 ? p_node->p_left_ : p_node->p_right_);
  const KDNode::KDNodePointer & oppositeSide(delta < 0.0 ? p_node->p_right_ : p_node->p_left_);
  UnsignedInteger currentBestIndex = points_.getSize();
  Scalar currentBestSquaredDistance = bestSquaredDistance;
  const UnsignedInteger dimension = points_.getDimension();
  const UnsignedInteger nextActiveDimension = (activeDimension + 1) % dimension;
  const Scalar saveLower = lowerBoundingBox[activeDimension];
  const Scalar saveUpper = upperBoundingBox[activeDimension];
  // 1)
  if (sameSide != 0)
  {
    // 1.1)
    // Update bounding box to match sameSide bounding box
    if (delta < 0.0)
      upperBoundingBox[activeDimension] = splitValue;
    else
      lowerBoundingBox[activeDimension] = splitValue;
    // Compute distance between x and sameSide
    Scalar squaredDistanceBoundingBox = 0.0;
    for(UnsignedInteger i = 0; i < dimension; ++i)
    {
      Scalar difference = std::max(0.0, std::max(x[i] - upperBoundingBox[i], lowerBoundingBox[i] - x[i]));
      squaredDistanceBoundingBox += difference * difference;
    }
    if (squaredDistanceBoundingBox < currentBestSquaredDistance)
    {
      UnsignedInteger candidateBestIndex = getNearestNeighbourIndex(sameSide, x, bestSquaredDistance, lowerBoundingBox, upperBoundingBox, nextActiveDimension);
      if (bestSquaredDistance < currentBestSquaredDistance)
      {
        currentBestSquaredDistance = bestSquaredDistance;
        currentBestIndex = candidateBestIndex;
      }
    }
    // Restore bounding box
    if (delta < 0.0)
      upperBoundingBox[activeDimension] = saveUpper;
    else
      lowerBoundingBox[activeDimension] = saveLower;
  } // sameSide != 0
  // 2)
  if (currentBestSquaredDistance < delta * delta)
  {
    // 2.1)
    bestSquaredDistance = currentBestSquaredDistance;
    return currentBestIndex;
  }
  // 2.2)
  const UnsignedInteger localIndex = p_node->index_;
  // Similar to (x - points_[localIndex]).normSquare() but it is better to avoid Point creation
  Scalar localSquaredDistance = 0.0;
  for(UnsignedInteger i = 0; i < dimension; ++i)
    localSquaredDistance += (x[i] - points_(localIndex, i)) * (x[i] - points_(localIndex, i));
  if (localSquaredDistance < currentBestSquaredDistance)
  {
    currentBestSquaredDistance = localSquaredDistance;
    // To send the current best squared distance to the lower levels
    bestSquaredDistance = localSquaredDistance;
    currentBestIndex = localIndex;
  }
  // 2.3)
  if (oppositeSide != 0)
  {
    // Update bounding box to match oppositeSide bounding box
    if (delta < 0.0)
      lowerBoundingBox[activeDimension] = splitValue;
    else
      upperBoundingBox[activeDimension] = splitValue;
    // Compute distance between x and oppositeSide
    Scalar squaredDistanceBoundingBox = 0.0;
    for(UnsignedInteger i = 0; i < dimension; ++i)
    {
      Scalar difference = std::max(0.0, std::max(x[i] - upperBoundingBox[i], lowerBoundingBox[i] - x[i]));
      squaredDistanceBoundingBox += difference * difference;
    }
    // 2.4)
    if (squaredDistanceBoundingBox < currentBestSquaredDistance)
    {
      UnsignedInteger candidateBestIndex = getNearestNeighbourIndex(oppositeSide, x, bestSquaredDistance, lowerBoundingBox, upperBoundingBox, nextActiveDimension);
      if (bestSquaredDistance < currentBestSquaredDistance)
      {
        currentBestSquaredDistance = bestSquaredDistance;
        currentBestIndex = candidateBestIndex;
      }
    }
    // Restore bounding box
    if (delta < 0.0)
      lowerBoundingBox[activeDimension] = saveLower;
    else
      upperBoundingBox[activeDimension] = saveUpper;
  } // oppositeSide != 0
  // 3)
  bestSquaredDistance = currentBestSquaredDistance;
  return currentBestIndex;
}

/* Get the indices of the k nearest neighbours of the given point */
Indices KDTree::query_k(const Point & x, const UnsignedInteger k, const Bool sorted) const
{
  if (k > points_.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot return more neighbours than points in the database!";
  Indices result(k);
  // If we need as many neighbours as points in the sample, just return all the possible indices
  if (k == points_.getSize() && !sorted)
  {
    result.fill();
  }
  else
  {
    KDNearestNeighboursFinder heap(points_, boundingBox_, k);
    result = heap.getNearestNeighboursIndices(p_root_, x, sorted);
  }
  return result;
}

/** Method save() stores the object through the StorageManager */
void KDTree::save(Advocate & adv) const
{
  NearestNeighbourAlgorithmImplementation::save(adv);
  adv.saveAttribute("points_", points_);
}

/** Method load() reloads the object from the StorageManager */
void KDTree::load(Advocate & adv)
{
  NearestNeighbourAlgorithmImplementation::load(adv);
  Sample points;
  adv.loadAttribute("points_", points);
  if (points.getSize() > 0) setSample(points);

}

END_NAMESPACE_OPENTURNS
