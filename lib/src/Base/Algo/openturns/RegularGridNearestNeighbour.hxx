//                                               -*- C++ -*-
/**
 *  @brief Nearest neighbour index search on a RegularGrid
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
#ifndef OPENTURNS_REGULARGRIDNEARESTNEIGHBOUR_HXX
#define OPENTURNS_REGULARGRIDNEARESTNEIGHBOUR_HXX

#include "openturns/NearestNeighbourImplementation.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RegularGridNearestNeighbour
 *
 * Organize d-dimensional points into a hierarchical tree-like structure
 */
class OT_API RegularGridNearestNeighbour
  : public NearestNeighbourImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  RegularGridNearestNeighbour();

  /** Parameter constructor */
  explicit RegularGridNearestNeighbour(const RegularGrid & grid);

  /** Virtual constructor */
  virtual RegularGridNearestNeighbour * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Get the index of the nearest neighbour of the given point */
  UnsignedInteger getNearestNeighbourIndex(const Point & x) const;
  using NearestNeighbourImplementation::getNearestNeighbourIndex;

  /** Get the nearest neighbour of the given point */
  Point getNearestNeighbour(const Point & x) const;
  using NearestNeighbourImplementation::getNearestNeighbour;

  /** Get the indices of the k nearest neighbours of the given point */
  Indices getNearestNeighboursIndices(const Point & x,
                                      const UnsignedInteger k,
                                      const Bool sorted  = false) const;

  /** Get the k nearest neighbours of the given point */
  Sample getNearestNeighbours(const Point & x,
                              const UnsignedInteger k,
                              const Bool sorted = false) const;

  /** Get the index of the nearest neighbour of the given scalar */
  UnsignedInteger getNearestScalarNeighbourIndex(const Scalar x) const;

  /** Get the index of the nearest neighbour of the given scalars */
  Indices getNearestScalarNeighbourIndex(const Point & x) const;

  /** Get the nearest neighbour of the given scalar */
  Scalar getNearestScalarNeighbour(const Scalar x) const;

  /** Get the nearest neighbour of the given scalars */
  Point getNearestScalarNeighbour(const Point & x) const;

  /** Get the indices of the k nearest neighbours of the given scalar */
  Indices getNearestScalarNeighboursIndices(const Scalar x,
                                            const UnsignedInteger k,
                                            const Bool sorted  = false) const;

  /** Get the k nearest neighbours of the given scalar */
  Point getNearestScalarNeighbours(const Scalar x,
                                   const UnsignedInteger k,
                                   const Bool sorted = false) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  /** The data organized by the tree */
  RegularGrid grid_;

  /** Cached values taken from grid_ */
  Scalar start_;
  UnsignedInteger N_;
  Scalar step_;
  Point values_;

}; /* class RegularGridNearestNeighbour */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_REGULARGRIDNEARESTNEIGHBOUR_HXX */
