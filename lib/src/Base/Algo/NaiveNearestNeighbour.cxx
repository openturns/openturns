//                                               -*- C++ -*-
/**
 *  @brief Brute force algorithm for nearest-neighbour lookup
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

#include "openturns/NaiveNearestNeighbour.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Indices.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NaiveNearestNeighbour)

static const Factory<NaiveNearestNeighbour> Factory_NaiveNearestNeighbour;

/**
 * @class NaiveNearestNeighboursFinder
 *
 * A fixed-size heap to find k-nearest neighbours in a NaiveNearestNeighbour
 */
class NaiveNearestNeighboursFinder
{
public:

  /** Constructor */
  NaiveNearestNeighboursFinder(const Sample & sample, const UnsignedInteger size)
    : points_(sample)
    , capacity_(size)
    , size_(0)
    , values_(size)
    , indices_(size)
  {
    // Initialize values_[0] to a valid value
    values_[0] = SpecFunc::MaxScalar;
  }

  /** Get the indices of the k nearest neighbours of the given point */
  Indices getNearestNeighboursIndices(const Point & x, const Bool sorted)
  {
    if (size_ != 0)
    {
      // Clear heap
      indices_.clear();
      values_.clear();
      size_ = 0;
      values_[0] = SpecFunc::MaxScalar;
    }
    const UnsignedInteger dimension = points_.getDimension();
    const UnsignedInteger sampleSize = points_.getSize();
    Scalar maxHeap = values_[0];
    for(UnsignedInteger i = 0; i < sampleSize; ++i)
    {
      Scalar localSquaredDistance = 0.0;
      for(UnsignedInteger j = 0; j < dimension; ++j)
        localSquaredDistance += (x[j] - points_(i, j)) * (x[j] - points_(i, j));
      // Insert value into heap
      if (size_ != capacity_)
      {
        // Put index/value at the first free node and move it up to a valid location
        indices_[size_] = i;
        values_[size_] = localSquaredDistance;
        moveNodeUp(size_);
        ++size_;
        maxHeap = values_[0];
      }
      else if (localSquaredDistance < maxHeap)
      {
        // Heap is full, and current value is smaller than heap largest value.
        // Replace the largest value by current value and move it down to a
        // valid location.
        indices_[0] = i;
        values_[0] = localSquaredDistance;
        moveNodeDown(0);
        maxHeap = values_[0];
      }
    }
    if (sorted)
    {
      // Sort heap in-place in ascending order.
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
  const Sample points_;
  // Maximum heap size
  const UnsignedInteger capacity_;
  // Number of used buckets
  UnsignedInteger size_;
  // Array containing values
  Collection<Scalar> values_;
  // Array containing point indices
  Indices indices_;

}; /* class NaiveNearestNeighboursFinder */

/**
 * @class NaiveNearestNeighbour
 *
 * Organize d-dimensional points into a hierarchical tree-like structure
 */

/* Default constructor */
NaiveNearestNeighbour::NaiveNearestNeighbour()
  : NearestNeighbourAlgorithmImplementation()
  , points_(0, 0)
{
  // Nothing to do
}

/* Parameters constructor */
NaiveNearestNeighbour::NaiveNearestNeighbour(const Sample & points)
  : NearestNeighbourAlgorithmImplementation()
  , points_(points)
{
  // Nothing to do
}

/* Virtual constructor */
NaiveNearestNeighbour * NaiveNearestNeighbour::clone() const
{
  return new NaiveNearestNeighbour( *this );
}

/* Virtual default constructor */
NaiveNearestNeighbour * NaiveNearestNeighbour::emptyClone() const
{
  return new NaiveNearestNeighbour();
}

/* Sample accessor */
Sample NaiveNearestNeighbour::getSample() const
{
  return points_;
}

void NaiveNearestNeighbour::setSample(const Sample & points)
{
  points_ = points;
}

/* String converter */
String NaiveNearestNeighbour::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " sample=" << points_;
}

String NaiveNearestNeighbour::__str__(const String & ) const
{
  return OSS(false) << "class=" << GetClassName()
         << " sample=" << points_;
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger NaiveNearestNeighbour::query(const Point & x) const
{
  const UnsignedInteger size = points_.getSize();
  const UnsignedInteger dimension = points_.getDimension();
  Scalar smallestDistance = SpecFunc::MaxScalar;
  UnsignedInteger bestIndex = size;
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    Scalar localSquaredDistance = 0.0;
    for(UnsignedInteger j = 0; j < dimension; ++j)
      localSquaredDistance += (x[j] - points_(i, j)) * (x[j] - points_(i, j));
    if (localSquaredDistance < smallestDistance)
    {
      smallestDistance = localSquaredDistance;
      bestIndex = i;
    }
  }
  return bestIndex;
}

/* Get the indices of the k nearest neighbours of the given point */
Indices NaiveNearestNeighbour::queryK(const Point & x, const UnsignedInteger k, const Bool sorted) const
{
  if (!(k <= points_.getSize())) throw InvalidArgumentException(HERE) << "Error: cannot return more neighbours (" << k << ") than points (" << points_.getSize() << ") in the database!";
  Indices result(k);
  // If we need as many neighbours as points in the sample, just return all the possible indices
  if (k == points_.getSize() && !sorted)
  {
    result.fill();
  }
  else
  {
    NaiveNearestNeighboursFinder heap(points_, k);
    result = heap.getNearestNeighboursIndices(x, sorted);
  }
  return result;
}

/** Method save() stores the object through the StorageManager */
void NaiveNearestNeighbour::save(Advocate & adv) const
{
  NearestNeighbourAlgorithmImplementation::save(adv);
  adv.saveAttribute("points_", points_);
}

/** Method load() reloads the object from the StorageManager */
void NaiveNearestNeighbour::load(Advocate & adv)
{
  NearestNeighbourAlgorithmImplementation::load(adv);
  adv.loadAttribute("points_", points_);
}

END_NAMESPACE_OPENTURNS
