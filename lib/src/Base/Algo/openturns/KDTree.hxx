//                                               -*- C++ -*-
/**
 *  @brief KDTree structure to speed-up queries on large samples
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  KDTree * clone() const override;

#ifndef SWIG
  /** Virtual default constructor */
  KDTree * emptyClone() const override;
#endif

  /** Sample accessor */
  Sample getSample() const override;
  void setSample(const Sample & sample) override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Get the index of the nearest neighbour of the given point */
  UnsignedInteger query(const Point & x) const override;
  using NearestNeighbourAlgorithmImplementation::query;

  /** Get the indices of the k nearest neighbours of the given point */
  Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted  = false) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  String printNode(const UnsignedInteger inode) const;

  /** Insert the point of the database at index in the tree */
  void insert(UnsignedInteger & inode,
              UnsignedInteger & currentSize,
              const UnsignedInteger index,
              const UnsignedInteger activeDimension);

  /** Get the index of the nearest neighbour of the given point */
  virtual UnsignedInteger getNearestNeighbourIndex(const UnsignedInteger inode,
      const Point & x,
      Scalar & bestSquaredDistance,
      Point & lowerBoundingBox,
      Point & upperBoundingBox,
      const UnsignedInteger activeDimension) const;

  /** The data organized by the tree */
  Sample points_;

  /** Global bounding box */
  Interval boundingBox_;

  /** The tree, stored as a list of tuples (index, leftNode, rightNode) */
  Indices tree_;

}; /* class KDTree */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KDTREE_HXX */
