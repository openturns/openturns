//                                               -*- C++ -*-
/**
 *  @brief Mesh is defined as a collection of n-D vertices and simplices
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
#ifndef OPENTURNS_MESHIMPLEMENTATION_HXX
#define OPENTURNS_MESHIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/Indices.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Graph.hxx"
#include "openturns/TBB.hxx"
#include "openturns/KDTree.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MeshImplementation
 *
 * A class that holds a mesh
 */
class OT_API MeshImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection< Indices >           IndicesCollection;
  typedef PersistentCollection< Indices > IndicesPersistentCollection;

  /** Default constructor */
  explicit MeshImplementation(const UnsignedInteger dimension = 1);

  /** Parameters constructor */
  explicit MeshImplementation(const Sample & vertices);

  /** Parameters constructor */
  MeshImplementation(const Sample & vertices,
                     const IndicesCollection & simplices);

  /** Virtual constructor method */
  virtual MeshImplementation * clone() const;

  /** Get the spatial dimension */
  UnsignedInteger getDimension() const;

  /** Check if the given point is inside of the closed mesh */
  Bool contains(const Point & point) const;

  /** Get the description of the vertices */
  Description getDescription() const;

  /** Get the number of vertices */
  UnsignedInteger getVerticesNumber() const;

  /** Get the number of simplices */
  UnsignedInteger getSimplicesNumber() const;

  /** Add a KDTree to speed-up nearest vertex searches */
  virtual void computeKDTree();

  /** Get the index of the nearest vertex */
  virtual UnsignedInteger getNearestVertexIndex(const Point & point) const;

  /** Get the index of the nearest vertex and the index of the containing simplex if any */
  Indices getNearestVertexAndSimplexIndicesWithCoordinates(const Point & point,
      Point & coordinatesOut) const;

  /** Get the nearest vertex */
  Point getNearestVertex(const Point & point) const;

  /** Get the index of the nearest vertex for a set of points */
  Indices getNearestVertexIndex(const Sample & points) const;

  /** Get the nearest vertex for a set of points */
  Sample getNearestVertex(const Sample & points) const;

  /** Get the map between vertices and simplices: for each vertex, list the vertices indices it belongs to */
  IndicesCollection getVerticesToSimplicesMap() const;

  /** Compute weights such that an integral of a function over the mesh
   * is a weighted sum of its values at the vertices
   */
  Point computeWeights() const;

  /** Comparison operator */
  Bool operator == (const MeshImplementation & rhs) const;

  /** Check mesh validity, i.e:
      non-overlaping simplices,
      no unused vertex,
      no simplices with duplicate vertices,
      no coincident vertices */
  Bool isValid() const;

  /** Check if the given point is in the given simplex */
  Bool checkPointInSimplex(const Point & point,
                           const UnsignedInteger index) const;

  /** Check if the given point is in the given simplex and returns its barycentric coordinates */
  Bool checkPointInSimplexWithCoordinates(const Point & point,
                                          const UnsignedInteger index,
                                          Point & coordinatesOut) const;

  /** Vertices accessor */
  Sample getVertices() const;
  void setVertices(const Sample & vertices);

  /** Vertex accessor */
  Point getVertex(const UnsignedInteger index) const;
  void setVertex(const UnsignedInteger index,
                 const Point & vertex);

  /** Simplices accessor */
  IndicesCollection getSimplices() const;
  void setSimplices(const IndicesCollection & simplices);

  /** Simplex accessor */
  Indices getSimplex(const UnsignedInteger index) const;

  /** Compute the volume of a given simplex */
  Scalar computeSimplexVolume(const UnsignedInteger index) const;

  /** Compute P1 gram matrix */
  CovarianceMatrix computeP1Gram() const;

  /** Tells if the mesh is regular */
  Bool isRegular() const;

  /** Lower bound of the bounding box */
  Point getLowerBound() const;

  /** Upper bound of the bounding box */
  Point getUpperBound() const;

  /** Drawing method */
  Graph draw() const;
  Graph draw1D() const;
  Graph draw2D() const;
  Graph draw3D(const Bool drawEdge = true,
               const Scalar thetaX = 0.0,
               const Scalar thetaY = 0.0,
               const Scalar thetaZ = 0.0,
               const Bool shading = false,
               const Scalar rho = 1.0) const;
  Graph draw3D(const Bool drawEdge,
               const SquareMatrix & rotation,
               const Bool shading,
               const Scalar rho) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** FreeFem mesh import */
  static MeshImplementation ImportFromMSHFile(const String & fileName);

  /** VTK export */
  String streamToVTKFormat() const;
  void exportToVTKFile(const String & fileName) const;

protected:
  // Build the affine matrix associated with a given simplex
  SquareMatrix buildSimplexMatrix(const UnsignedInteger index) const;

  void checkValidity() const;

  // An n-D mesh is a set of vertices with a topology described by a set of simplices
  // The vertices
  Sample vertices_;

  // The simplices
  IndicesPersistentCollection simplices_;

  // The kd-tree associated to the vertices
  KDTree tree_;

  // The vertices to simplices map
  mutable IndicesPersistentCollection verticesToSimplices_;

}; /* class MeshImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MESHIMPLEMENTATION_HXX */
