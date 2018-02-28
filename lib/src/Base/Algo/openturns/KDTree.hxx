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
#ifndef OPENTURNS_KDTREE_HXX
#define OPENTURNS_KDTREE_HXX

#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

class KDNearestNeighboursFinder;

/**
 * @class KDTree
 *
 * Organize d-dimensional points into a hierarchical tree-like structure
 */
class OT_API KDTree
  : public NearestNeighbourAlgorithmImplementation
{
  CLASSNAME

  friend class KDNearestNeighboursFinder;

public:

  /** Default constructor */
  KDTree();

  /** Parameters constructor */
  explicit KDTree(const Sample & sample);

  /** Virtual constructor */
  virtual KDTree * clone() const;

#ifndef SWIG
  /** Virtual default constructor */
  virtual KDTree * emptyClone() const;
#endif

  /** Sample accessor */
  virtual Sample getSample() const;
  virtual void setSample(const Sample & sample);

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Get the index of the nearest neighbour of the given point */
  UnsignedInteger query(const Point & x) const;
  using NearestNeighbourAlgorithmImplementation::query;

  /** Get the indices of the k nearest neighbours of the given point */
  virtual Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted  = false) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  /**
   * @class KDNode
   *
   * A node in a KDTree
   */
  struct KDNode
  {
    typedef Pointer< KDNode> KDNodePointer;

    /** Parameter constructor */
    explicit KDNode(const UnsignedInteger index)
      : index_(index)
      , p_left_(0)
      , p_right_(0)
    {
      // Nothing to do
    }

    /** String converter */
    String __repr__() const
    {
      return OSS() << "class=KDNode"
             << " index=" << index_
             << " left=" << (p_left_ ? p_left_->__repr__() : "NULL")
             << " right=" << (p_right_ ? p_right_->__repr__() : "NULL");
    }

    /* Index of the nodal point */
    UnsignedInteger index_;

    /* Children */
    KDNodePointer p_left_;
    KDNodePointer p_right_;

  }; /* class KDNode */

  /** Insert the point of the database at index i in the tree */
  void insert(KDNode::KDNodePointer & p_node,
              const UnsignedInteger index,
              const UnsignedInteger activeDimension);

  /** Get the index of the nearest neighbour of the given point */
  virtual UnsignedInteger getNearestNeighbourIndex(const KDNode::KDNodePointer & p_node,
      const Point & x,
      Scalar & bestSquaredDistance,
      Point & lowerBoundingBox,
      Point & upperBoundingBox,
      const UnsignedInteger activeDimension) const;

  /** The data organized by the tree */
  Sample points_;

  /** Global bounding box */
  Interval boundingBox_;

  /** The root of the tree */
  KDNode::KDNodePointer p_root_;

}; /* class KDTree */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KDTREE_HXX */
