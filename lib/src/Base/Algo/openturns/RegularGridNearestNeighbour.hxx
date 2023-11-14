//                                               -*- C++ -*-
/**
 *  @brief Nearest neighbour index search on a RegularGrid
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
#ifndef OPENTURNS_REGULARGRIDNEARESTNEIGHBOUR_HXX
#define OPENTURNS_REGULARGRIDNEARESTNEIGHBOUR_HXX

#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RegularGridNearestNeighbour
 *
 * Organize d-dimensional points into a hierarchical tree-like structure
 */
class OT_API RegularGridNearestNeighbour
  : public NearestNeighbourAlgorithmImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  RegularGridNearestNeighbour();

  /** Parameter constructor */
  explicit RegularGridNearestNeighbour(const RegularGrid & grid);

  /** Parameter constructor */
  explicit RegularGridNearestNeighbour(const Sample & vertices);

  /** Virtual constructor */
  RegularGridNearestNeighbour * clone() const override;

#ifndef SWIG
  /** Virtual default constructor */
  RegularGridNearestNeighbour * emptyClone() const override;
#endif

  /** String converter */
  String __repr__() const override;

  /** Sample accessor */
  Sample getSample() const override;
  void setSample(const Sample & sample) override;

  /** Get the index of the nearest neighbour of the given point */
  UnsignedInteger query(const Point & x) const override;
  using NearestNeighbourAlgorithmImplementation::query;

  /** Get the index of the nearest neighbour of the given scalar */
  UnsignedInteger queryScalar(const Scalar x) const;

  /** Get the index of the nearest neighbour of the given scalars */
  Indices queryScalar(const Point & x) const;

  /** Get the indices of the k nearest neighbours of the given point */
  Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted  = false) const override;

  /** Get the indices of the k nearest neighbours of the given scalar */
  Indices queryScalarK(const Scalar x, const UnsignedInteger k, const Bool sorted  = false) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

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
