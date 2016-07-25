//                                               -*- C++ -*-
/**
 *  @brief Mesh is defined as a collection of n-D vertices and simplices
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MESH_HXX
#define OPENTURNS_MESH_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Indices.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/DomainImplementation.hxx"
#include "openturns/Graph.hxx"
#include "openturns/TBB.hxx"
#include "openturns/KDTree.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Mesh
 *
 * A class that holds a mesh
 */
class OT_API Mesh
  : public DomainImplementation
{
  CLASSNAME;

public:
  typedef Collection< Indices >           IndicesCollection;
  typedef PersistentCollection< Indices > IndicesPersistentCollection;

  /** Default constructor */
  explicit Mesh(const UnsignedInteger dimension = 1);

  /** Parameters constructor */
  explicit Mesh(const NumericalSample & vertices);

  /** Parameters constructor */
  Mesh(const NumericalSample & vertices,
       const IndicesCollection & simplices);

  /** Virtual constructor method */
  virtual Mesh * clone() const;

  /** Check if the given point is inside of the closed mesh */
  Bool contains(const NumericalPoint & point) const;

  /** Get the description of the vertices */
  Description getDescription() const;

  /** Get the number of vertices */
  UnsignedInteger getVerticesNumber() const;

  /** Get the number of simplices */
  UnsignedInteger getSimplicesNumber() const;

  /** Get the index of the nearest vertex */
  UnsignedInteger getNearestVertexIndex(const NumericalPoint & point) const;

  /** Get the nearest vertex */
  NumericalPoint getNearestVertex(const NumericalPoint & point) const;

  /** Get the index of the nearest vertex for a set of points */
  Indices getNearestVertexIndex(const NumericalSample & points) const;

  /** Get the nearest vertex for a set of points */
  NumericalSample getNearestVertex(const NumericalSample & points) const;

  /** Get the map between vertices and simplices: for each vertex, list the vertices indices it belongs to */
  IndicesCollection getVerticesToSimplicesMap() const;

  /** Comparison operator */
  Bool operator == (const Mesh & rhs) const;

  /** Check mesh validity, i.e:
      non-overlaping simplices,
      no unused vertex,
      no simplices with duplicate vertices,
      no coincident vertices */
  Bool isValid() const;

  /** Check if the given point is in the given simplex */
  Bool checkPointInSimplex(const NumericalPoint & point,
                           const UnsignedInteger index) const;

  /** Check if the given point is in the given simplex and returns its barycentric coordinates */
  Bool checkPointInSimplexWithCoordinates(const NumericalPoint & point,
					  const UnsignedInteger index,
					  NumericalPoint & coordinates) const;

  /** Vertices accessor */
  const NumericalSample & getVertices() const;
  void setVertices(const NumericalSample & vertices);

  /** Vertex accessor */
  NumericalPoint getVertex(const UnsignedInteger index) const;
  void setVertex(const UnsignedInteger index,
                 const NumericalPoint & vertex);

  /** Simplices accessor */
  const IndicesCollection & getSimplices() const;
  void setSimplices(const IndicesCollection & simplices);

  /** Simplex accessor */
  Indices getSimplex(const UnsignedInteger index) const;

  /** Compute the volume of a given simplex */
  NumericalScalar computeSimplexVolume(const UnsignedInteger index) const;

  /** Compute P1 gram matrix */
  CovarianceMatrix computeP1Gram() const;

  /** Tells if the mesh is regular */
  Bool isRegular() const;

  /** Lower bound of the bounding box */
  NumericalPoint getLowerBound() const;

  /** Upper bound of the bounding box */
  NumericalPoint getUpperBound() const;

  /** Drawing method */
  Graph draw() const;
  Graph draw1D() const;
  Graph draw2D() const;
  Graph draw3D(const Bool drawEdge = true,
               const NumericalScalar thetaX = 0.0,
               const NumericalScalar thetaY = 0.0,
               const NumericalScalar thetaZ = 0.0,
               const Bool shading = false,
               const NumericalScalar rho = 1.0) const;
  Graph draw3D(const Bool drawEdge,
               const SquareMatrix & rotation,
               const Bool shading,
               const NumericalScalar rho) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  /** FreeFem mesh import */
  static Mesh ImportFromMSHFile(const String & fileName);

  /** VTK export */
  String streamToVTKFormat() const;
  void exportToVTKFile(const String & fileName) const;

protected:
  // Build the affine matrix associated with a given simplex
  SquareMatrix buildSimplexMatrix(const UnsignedInteger index) const;

  // Compute the total volume of the mesh
  void computeVolume() const;

  void checkValidity() const;

  // An n-D mesh is a set of vertices with a topology described by a set of simplices
  // The vertices
  NumericalSample vertices_;

  // The simplices
  IndicesPersistentCollection simplices_;

  // The kd-tree associated to the vertices
  KDTree tree_;
}; /* class Mesh */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MESH_HXX */
