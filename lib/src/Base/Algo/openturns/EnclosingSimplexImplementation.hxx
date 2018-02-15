//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is an abstract class.
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
#ifndef OPENTURNS_ENCLOSINGSIMPLEXIMPLEMENTATION_HXX
#define OPENTURNS_ENCLOSINGSIMPLEXIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplexImplementation
 */

class OT_API EnclosingSimplexImplementation
  : public PersistentObject
{

  CLASSNAME

public:
  typedef Collection<Indices>           IndicesCollection;
  typedef PersistentCollection<Indices> IndicesPersistentCollection;

  /** Default constructor without parameters */
  EnclosingSimplexImplementation();

  /** Parameter constructor */
  EnclosingSimplexImplementation(const Sample & vertices, const IndicesCollection & simplices);

  /** Virtual copy constructor */
  virtual EnclosingSimplexImplementation * clone() const;

  /** Points accessor */
  Sample getVertices() const;

  /** Simplices accessor */
  IndicesCollection getSimplices() const;

  /** Simplices accessor (flattened view) */
  IndicesCollection getFlattenedSimplices() const;

  /** Get the index of the enclosing simlex of the given point */
  virtual UnsignedInteger getEnclosingSimplexIndex(const Point & x) const;

  /** Get the indices of the enclosing simplex of the given points */
  virtual Indices getEnclosingSimplexIndex(const Sample & sample) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  // Called by constructor
  void initialize();

protected:
  // Check if the given point is in the given simplex.
  // This method is called many times for a given point, SquareMatrix is passed to avoid
  // allocating a new matrix each time.
  Bool checkPointInSimplex(const Point & point, const UnsignedInteger index, SquareMatrix & simplexMatrix) const;

  /** The vertices */
  Sample vertices_;

  /** The simplices:
      + flatSimplexIndices_ is a flattened list of vertex indices
      + offsetSimplexIndices_ is of size (nrSimplices+1) and gives the position
                              of the first vertex index in flatSimplexIndices_ */
  Indices flatSimplexIndices_;
  Indices offsetSimplexIndices_;

  // Global bounding box
  Interval boundingBox_;

  // The bounding boxes of simplices
  Sample lowerBoundingBoxSimplices_;
  Sample upperBoundingBoxSimplices_;

} ; /* class EnclosingSimplexImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ENCLOSINGSIMPLEXIMPLEMENTATION_HXX */
