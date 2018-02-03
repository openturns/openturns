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
#include <fstream>

#include "openturns/Mesh.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PlatformInfo.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Mesh)

/* Default constructor */
Mesh::Mesh(const UnsignedInteger dimension)
  : TypedInterfaceObject<MeshImplementation>(new MeshImplementation())
{
  // Nothing to do
}

/* Parameters constructor, simplified interface for 1D case */
Mesh::Mesh(const Sample & vertices)
  : TypedInterfaceObject<MeshImplementation>(new MeshImplementation(vertices))
{
  // Nothing to do
}

/* Parameters constructor, simplified interface for 1D case */
Mesh::Mesh(const Sample & vertices,
           const IndicesCollection & simplices)
  : TypedInterfaceObject<MeshImplementation>(new MeshImplementation(vertices, simplices))
{
  // Nothing to do
}

/* Constructor with implementation */
Mesh::Mesh(const Implementation & i)
  : TypedInterfaceObject<MeshImplementation>(i)
{
  // Nothing to do
}

/* Constructor with implementation */
Mesh::Mesh(const MeshImplementation & i)
  : TypedInterfaceObject<MeshImplementation>(i.clone())
{
  // Nothing to do
}

/* String converter */
String Mesh::__repr__() const
{
  OSS oss;
  oss << "class=" << Mesh::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* Dimension accessor */
UnsignedInteger Mesh::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Description of the vertices accessor */
Description Mesh::getDescription() const
{
  return getImplementation()->getDescription();
}

/* Vertices accessor */
Sample Mesh::getVertices() const
{
  return getImplementation()->getVertices();
}

void Mesh::setVertices(const Sample & vertices)
{
  copyOnWrite();
  getImplementation()->setVertices(vertices);
}

/* Compute KDTree to speed-up searches */
void Mesh::computeKDTree()
{
  copyOnWrite();
  getImplementation()->computeKDTree();
}

/* Vertex accessor */
Point Mesh::getVertex(const UnsignedInteger index) const
{
  return getImplementation()->getVertex(index);
}

void Mesh::setVertex(const UnsignedInteger index,
                     const Point & vertex)
{
  copyOnWrite();
  getImplementation()->setVertex(index, vertex);
}

/* Simplices accessor */
Mesh::IndicesCollection Mesh::getSimplices() const
{
  return getImplementation()->getSimplices();
}

void Mesh::setSimplices(const IndicesCollection & simplices)
{
  copyOnWrite();
  getImplementation()->setSimplices(simplices);
}

/* Simplex accessor */
Indices Mesh::getSimplex(const UnsignedInteger index) const
{
  return getImplementation()->getSimplex(index);
}

Bool Mesh::isValid() const
{
  return getImplementation()->isValid();
}

/* Check if the given point is in the mesh */
Bool Mesh::contains(const Point & point) const
{
  return getImplementation()->contains(point);
}

/* Check if the given point is in the given simplex */
Bool Mesh::checkPointInSimplex(const Point & point,
                               const UnsignedInteger index) const
{
  return getImplementation()->checkPointInSimplex(point, index);
}

/* Check if the given point is in the given simplex and returns its barycentric coordinates */
Bool Mesh::checkPointInSimplexWithCoordinates(const Point & point,
    const UnsignedInteger index,
    Point & coordinates) const
{
  return getImplementation()->checkPointInSimplexWithCoordinates(point, index, coordinates);
}

/* Get the number of vertices */
UnsignedInteger Mesh::getVerticesNumber() const
{
  return getImplementation()->getVerticesNumber();
}

/* Get the number of simplices */
UnsignedInteger Mesh::getSimplicesNumber() const
{
  return getImplementation()->getSimplicesNumber();
}

/* Get the index of the nearest vertex */
UnsignedInteger Mesh::getNearestVertexIndex(const Point & point) const
{
  return getImplementation()->getNearestVertexIndex(point);
}

/* Get the index of the nearest vertex and the index of the containing simplex if any */
Indices Mesh::getNearestVertexAndSimplexIndicesWithCoordinates(const Point & point,
    Point & coordinates) const
{
  return getImplementation()->getNearestVertexAndSimplexIndicesWithCoordinates(point, coordinates);
}

/* Get the nearest vertex */
Point Mesh::getNearestVertex(const Point & point) const
{
  return getImplementation()->getNearestVertex(point);
}

/* Get the index of the nearest vertex for a set of points */
Indices Mesh::getNearestVertexIndex(const Sample & points) const
{
  return getImplementation()->getNearestVertexIndex(points);
}

/* Get the nearest vertex for a set of points */
Sample Mesh::getNearestVertex(const Sample & points) const
{
  return getImplementation()->getNearestVertex(points);
}

/* Compute the volume of a given simplex */
Scalar Mesh::computeSimplexVolume(const UnsignedInteger index) const
{
  return getImplementation()->computeSimplexVolume(index);
}

/* Compute P1 gram matrix */
CovarianceMatrix Mesh::computeP1Gram() const
{
  return getImplementation()->computeP1Gram();
}

/* Tells if the mesh is regular */
Bool Mesh::isRegular() const
{
  return getImplementation()->isRegular();
}

/* Lower bound of the bounding box */
Point Mesh::getLowerBound() const
{
  return getImplementation()->getLowerBound();
}

/* Upper bound of the bounding box */
Point Mesh::getUpperBound() const
{
  return getImplementation()->getUpperBound();
}


/* Get the map between vertices and simplices: for each vertex, list the vertices indices it belongs to */
Mesh::IndicesCollection Mesh::getVerticesToSimplicesMap() const
{
  return getImplementation()->getVerticesToSimplicesMap();
}

/* Compute weights such that an integral of a function over the mesh
 * is a weighted sum of its values at the vertices
 */
Point Mesh::computeWeights() const
{
  return getImplementation()->computeWeights();
}

/* Comparison operator */
Bool Mesh::operator == (const Mesh & other) const
{
  if (this == &other) return true;
  return (*(getImplementation()) == *(other.getImplementation()));
}

/* Drawing method */
Graph Mesh::draw() const
{
  return getImplementation()->draw();
}

Graph Mesh::draw1D() const
{
  return getImplementation()->draw1D();
}

Graph Mesh::draw2D() const
{
  return getImplementation()->draw2D();
}

Graph Mesh::draw3D(const Bool drawEdge,
                   const Scalar thetaX,
                   const Scalar thetaY,
                   const Scalar thetaZ,
                   const Bool shading,
                   const Scalar rho) const
{
  return getImplementation()->draw3D(drawEdge, thetaX, thetaY, thetaZ, shading, rho);
}

Graph Mesh::draw3D(const Bool drawEdge,
                   const SquareMatrix & rotation,
                   const Bool shading,
                   const Scalar rho) const
{
  return getImplementation()->draw3D(drawEdge, rotation, shading, rho);
}

/* Import mesh from FreeFem 2D mesh files */
Mesh Mesh::ImportFromMSHFile(const String & fileName)
{
  MeshImplementation result(MeshImplementation::ImportFromMSHFile(fileName));
  return result;
}

/* VTK export */
String Mesh::streamToVTKFormat() const
{
  return getImplementation()->streamToVTKFormat();
}

void Mesh::exportToVTKFile(const String & fileName) const
{
  return getImplementation()->exportToVTKFile(fileName);
}

END_NAMESPACE_OPENTURNS
