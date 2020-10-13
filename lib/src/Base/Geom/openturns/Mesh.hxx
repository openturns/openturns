//                                               -*- C++ -*-
/**
 *  @brief Mesh is defined as a collection of n-D vertices and simplices
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
#ifndef OPENTURNS_MESH_HXX
#define OPENTURNS_MESH_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/Indices.hxx"
#include "openturns/IndicesCollection.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Mesh
 *
 * A class that holds a mesh
 */
class OT_API Mesh
  : public PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  explicit Mesh(const UnsignedInteger dimension = 1);

  /** Parameters constructor */
  explicit Mesh(const Sample & vertices);

  /** Parameters constructor */
  Mesh(const Sample & vertices,
       const IndicesCollection & simplices,
       const Bool checkMeshValidity = ResourceMap::GetAsBool("Mesh-CheckValidity"));

  /** Virtual constructor method */
  Mesh * clone() const override;

  /** Get the numerical volume of the domain */
  Scalar getVolume() const;

  /** Check if the domain is empty, i.e if its volume is zero */
  Bool isEmpty() const;
  Bool isNumericallyEmpty() const;

  /** Get the dimension */
  UnsignedInteger getDimension() const;

  /** Description accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Get the number of vertices */
  UnsignedInteger getVerticesNumber() const;

  /** Get the number of simplices */
  UnsignedInteger getSimplicesNumber() const;

  /** Compute weights such that an integral of a function over the mesh
   * is a weighted sum of its values at the vertices
   */
  Point computeWeights() const;

  /** Comparison operator */
  Bool operator == (const Mesh & rhs) const;

  /** Check mesh validity, i.e:
      non-overlaping simplices,
      no unused vertex,
      no simplices with duplicate vertices,
      no coincident vertices */
  Bool isValid() const;

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

  /** Compute the volume of all simplices */
  Point computeSimplicesVolume() const;

  /** Compute P1 gram matrix */
  CovarianceMatrix computeP1Gram() const;

  /** Tells if the mesh is regular */
  Bool isRegular() const;

  /** Lower bound of the bounding box */
  Point getLowerBound() const;

  /** Upper bound of the bounding box */
  Point getUpperBound() const;

  /** Orientation management */
  void fixOrientation();
#ifndef SWIG
  void fixOrientation(const UnsignedInteger & index,
                      SquareMatrix & simplexMatrix);
#endif

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
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** FreeFem mesh import */
  static Mesh ImportFromMSHFile(const String & fileName);

  /** VTK export */
  String streamToVTKFormat() const;
  String streamToVTKFormat(const IndicesCollection & simplices) const;
  void exportToVTKFile(const String & fileName) const;
  void exportToVTKFile(const String & fileName,
                       const IndicesCollection & simplices) const;

protected:
  // Build the affine matrix associated with a given simplex
  void buildSimplexMatrix(const UnsignedInteger index,
                          SquareMatrix & matrix) const;

  void checkValidity() const;

  // An n-D mesh is a set of vertices with a topology described by a set of simplices
  // Spatial dimension
  UnsignedInteger dimension_;

  // Mesh might be already checked (user provide the information)
  // or we might need to check it for drawing for example.
  mutable Bool hasBeenChecked_;

  // The vertices
  Sample vertices_;

  // The simplices
  IndicesCollection simplices_;

}; /* class Mesh */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MESH_HXX */
