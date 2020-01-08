//                                               -*- C++ -*-
/**
 *  @brief  This class provides a naive implementation of search
 *  of neighbours.  It is only meant for debugging purpose.
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
#ifndef OPENTURNS_NAIVEENCLOSINGSIMPLEX_HXX
#define OPENTURNS_NAIVEENCLOSINGSIMPLEX_HXX

#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollection.hxx"
#include "openturns/NearestNeighbourAlgorithm.hxx"
#include "openturns/Interval.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NaiveEnclosingSimplex
 */

class OT_API NaiveEnclosingSimplex
  : public EnclosingSimplexAlgorithmImplementation
{

  CLASSNAME

public:
  /** Default constructor without parameters */
  NaiveEnclosingSimplex();

  /** Parameter constructor */
  NaiveEnclosingSimplex(const Sample & vertices, const IndicesCollection & simplices);

  /** Virtual copy constructor */
  virtual NaiveEnclosingSimplex * clone() const;
#ifndef SWIG
  virtual NaiveEnclosingSimplex * emptyClone() const;
#endif

  void setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices);

  /** Nearest neighbour algorithm accessor */
  virtual NearestNeighbourAlgorithm getNearestNeighbourAlgorithm() const;
  virtual void setNearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & nearestNeighbour);

  /** Get the index of the enclosing simlex of the given point */
  virtual UnsignedInteger query(const Point & x) const;
  using EnclosingSimplexAlgorithmImplementation::query;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Algorithm to find the nearest neighbour of a point */
  NearestNeighbourAlgorithm nearestNeighbour_;

  // The vertices to simplices map
  IndicesCollection verticesToSimplices_;

} ; /* class NaiveEnclosingSimplex */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAIVEENCLOSINGSIMPLEX_HXX */
