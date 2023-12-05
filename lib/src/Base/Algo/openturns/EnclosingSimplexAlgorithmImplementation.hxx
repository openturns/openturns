//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is an abstract class.
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
#ifndef OPENTURNS_ENCLOSINGSIMPLEXALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_ENCLOSINGSIMPLEXALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollection.hxx"
#include "openturns/Interval.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplexAlgorithmImplementation
 */

class OT_API EnclosingSimplexAlgorithmImplementation
  : public PersistentObject
{

  CLASSNAME

public:
  /** Default constructor without parameters */
  EnclosingSimplexAlgorithmImplementation();

  /** Parameter constructor */
  EnclosingSimplexAlgorithmImplementation(const Sample & vertices,
                                          const IndicesCollection & simplices);

  /** Virtual copy constructor */
  EnclosingSimplexAlgorithmImplementation * clone() const override;

#ifndef SWIG
  virtual EnclosingSimplexAlgorithmImplementation * emptyClone() const;
#endif

  /** Points accessor */
  Sample getVertices() const;

  /** Simplices accessor (flattened view) */
  IndicesCollection getSimplices() const;

  // Called by constructor
  virtual void setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices);

  /** Get the index of the enclosing simlex of the given point */
  virtual UnsignedInteger query(const Point & x) const;

  /** Get the indices of the enclosing simplex of the given points */
  virtual Indices query(const Sample & sample) const;

  /** Accessor to the barycentric coordinates tolerance */
  void setBarycentricCoordinatesEpsilon(const Scalar epsilon);
  Scalar getBarycentricCoordinatesEpsilon() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  // Check if the given point is in the given simplex.
  // This method is called many times for a given point, SquareMatrix is passed to avoid
  // allocating a new matrix each time.
  Bool checkPointInSimplex(const Point & point, const UnsignedInteger index, SquareMatrix & simplexMatrix) const;

  /** The vertices */
  Sample vertices_;

  /** The simplices */
  IndicesCollection simplices_;

  // Global bounding box
  Interval boundingBox_;

  // The bounding boxes of simplices
  Sample lowerBoundingBoxSimplices_;
  Sample upperBoundingBoxSimplices_;

  // The tolerance for the membreship test
  Scalar barycentricCoordinatesEpsilon_;

} ; /* class EnclosingSimplexAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ENCLOSINGSIMPLEXALGORITHMIMPLEMENTATION_HXX */
