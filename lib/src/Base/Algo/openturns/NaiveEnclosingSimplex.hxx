//                                               -*- C++ -*-
/**
 *  @brief  This class provides a naive implementation of search
 *  of neighbours.  It is only meant for debugging purpose.
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
#ifndef OPENTURNS_NAIVEENCLOSINGSIMPLEX_HXX
#define OPENTURNS_NAIVEENCLOSINGSIMPLEX_HXX

#include "openturns/EnclosingSimplexImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/NearestNeighbour.hxx"
#include "openturns/Interval.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NaiveEnclosingSimplex
 */

class OT_API NaiveEnclosingSimplex
  : public EnclosingSimplexImplementation
{

  CLASSNAME

public:
  typedef EnclosingSimplexImplementation::IndicesCollection            IndicesCollection;
  typedef EnclosingSimplexImplementation::IndicesPersistentCollection  IndicesPersistentCollection;

  /** Default constructor without parameters */
  NaiveEnclosingSimplex();

  /** Parameter constructor */
  explicit NaiveEnclosingSimplex(const Mesh & mesh);

  /** Parameter constructor */
  NaiveEnclosingSimplex(const Sample & vertices, const IndicesCollection & simplices);

  /** Virtual copy constructor */
  virtual NaiveEnclosingSimplex * clone() const;

  /** Get the index of the enclosing simlex of the given point */
  virtual UnsignedInteger getEnclosingSimplexIndex(const Point & x) const;
  using EnclosingSimplexImplementation::getEnclosingSimplexIndex;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  void initialize();

  /** Algorithm to find the nearest neighbour of a point */
  NearestNeighbour nearestNeighbour_;

  // The vertices to simplices map
  IndicesPersistentCollection verticesToSimplices_;

} ; /* class NaiveEnclosingSimplex */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAIVEENCLOSINGSIMPLEX_HXX */
