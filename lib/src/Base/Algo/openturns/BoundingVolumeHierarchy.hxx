//                                               -*- C++ -*-
/**
 *  @brief  This class implements Bounding Volume Hierarchy (BVH) to speed-up point location.
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
#ifndef OPENTURNS_BOUNDINGVOLUMEHIERARCHY_HXX
#define OPENTURNS_BOUNDINGVOLUMEHIERARCHY_HXX

#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoundingVolumeHierarchy
 */

class OT_API BoundingVolumeHierarchy
  : public EnclosingSimplexAlgorithmImplementation
{

  CLASSNAME

public:
  /** Default constructor without parameters */
  BoundingVolumeHierarchy();

  /** Parameter constructor */
  BoundingVolumeHierarchy(const Sample & vertices,
                          const IndicesCollection & simplices,
                          const UnsignedInteger binNumber = ResourceMap::GetAsUnsignedInteger("BoundingVolumeHierarchy-BinNumber"),
                          const String strategy = ResourceMap::GetAsString("BoundingVolumeHierarchy-Strategy"));

  /** Virtual copy constructor */
  BoundingVolumeHierarchy * clone() const override;
#ifndef SWIG
  BoundingVolumeHierarchy * emptyClone() const override;
#endif

  void setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices) override;

  /** Get the index of the enclosing simlex of the given point */
  UnsignedInteger query(const Point & x) const override;
  using EnclosingSimplexAlgorithmImplementation::query;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  struct Node
  {
    typedef Pointer<Node> NodePointer;

    /** Internal node */
    Node(const UnsignedInteger activeDimension,
         const Scalar value,
         const NodePointer & p_left,
         const NodePointer & p_right,
         const Point & lowerBounds,
         const Point & upperBounds)
      : lowerBounds_(lowerBounds)
      , upperBounds_(upperBounds)
      , activeDimension_(activeDimension)
      , value_(value)
      , p_left_(p_left)
      , p_right_(p_right)
      , offset_(0)
      , nrSimplices_(0)
    {
      // Nothing to do
    }
    /** Leaf node */
    Node(const UnsignedInteger offset,
         const UnsignedInteger nrSimplices,
         const Point & lowerBounds,
         const Point & upperBounds)
      : lowerBounds_(lowerBounds)
      , upperBounds_(upperBounds)
      , activeDimension_(0)
      , value_(0)
      , p_left_(0)
      , p_right_(0)
      , offset_(offset)
      , nrSimplices_(nrSimplices)
    {
      // Nothing to do
    }
    const Point lowerBounds_;
    const Point upperBounds_;
    // 1. For inner nodes
    //    Split dimension
    const UnsignedInteger activeDimension_;
    //    Split value
    const Scalar value_;
    //    Children
    const NodePointer p_left_;
    const NodePointer p_right_;
    // 2. For leaf nodes
    //    Offset of the first simplex
    const UnsignedInteger offset_;
    //    Number of simplices enclosed in leaf
    const UnsignedInteger nrSimplices_;

  }; /* struct Node */

  /** Recursively build tree */
  Node::NodePointer build(UnsignedInteger & sorted,
                          const UnsignedInteger firstIndex,
                          const UnsignedInteger lastIndex);

  /** The root of the tree */
  Node::NodePointer p_root_;

  /** Maximum number of simplices in a node */
  UnsignedInteger binNumber_;

  /** Splitting strategy, may be either "mean" or "median" */
  String strategy_;

  /** Collection of sorted simplices */
  Indices sortedSimplices_;

  // The center of bounding boxes of simplices
  Sample centerBoundingBoxSimplices_;
} ; /* class BoundingVolumeHierarchy */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BOUNDINGVOLUMEHIERARCHY_HXX */
