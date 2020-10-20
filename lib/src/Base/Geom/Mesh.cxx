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
#include <fstream>
#include <algorithm>

#include "openturns/Mesh.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/PolygonArray.hxx"
#include "openturns/Collection.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PlatformInfo.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Mesh)

static const Factory<Mesh> Factory_Mesh;

/* Default constructor */
Mesh::Mesh(const UnsignedInteger dimension)
  : PersistentObject()
  , dimension_(dimension)
  , hasBeenChecked_(false)
  , vertices_(1, dimension) // At least one point
  , simplices_()
{
  // Nothing to do
  if (vertices_.getDescription().isBlank()) vertices_.setDescription(Description::BuildDefault(dimension, "t"));
}

/* Parameters constructor, simplified interface for 1D case */
Mesh::Mesh(const Sample & vertices)
  : PersistentObject()
  , dimension_(vertices.getDimension())
  , hasBeenChecked_(false)
  , vertices_(0, vertices.getDimension())
  , simplices_()
{
  // Use the vertices accessor to initialize the kd-tree
  setVertices(vertices);
}

/* Parameters constructor, simplified interface for 1D case */
Mesh::Mesh(const Sample & vertices,
           const IndicesCollection & simplices,
           const Bool checkMeshValidity)
  : PersistentObject()
  , dimension_(vertices.getDimension())
  , hasBeenChecked_(false)
  , vertices_(0, vertices.getDimension())
  , simplices_(simplices)
{
  // Use the vertices accessor to initialize the kd-tree
  setVertices(vertices);
  if (checkMeshValidity)
    checkValidity();
}

/* Clone method */
Mesh * Mesh::clone() const
{
  return new Mesh(*this);
}

/* Dimension accessor */
UnsignedInteger Mesh::getDimension() const
{
  return dimension_;
}

/* Description of the vertices accessor */
void Mesh::setDescription(const Description & description)
{
  vertices_.setDescription(description);
}

Description Mesh::getDescription() const
{
  return vertices_.getDescription();
}

/* Vertices accessor */
Sample Mesh::getVertices() const
{
  return vertices_;
}

void Mesh::setVertices(const Sample & vertices)
{
  vertices_ = vertices;
  if (vertices_.getDescription().isBlank()) vertices_.setDescription(Description::BuildDefault(vertices_.getDimension(), "t"));
  hasBeenChecked_ = false;
}

/* Vertex accessor */
Point Mesh::getVertex(const UnsignedInteger index) const
{
  if (index >= getVerticesNumber()) throw InvalidArgumentException(HERE) << "Error: the vertex index=" << index << " must be less than the number of vertices=" << getVerticesNumber();
  return vertices_[index];
}

void Mesh::setVertex(const UnsignedInteger index,
                     const Point & vertex)
{
  vertices_[index] = vertex;
  hasBeenChecked_ = false;
}

/* Simplices accessor */
IndicesCollection Mesh::getSimplices() const
{
  return simplices_;
}

void Mesh::setSimplices(const IndicesCollection & simplices)
{
  if (!(simplices == simplices_))
  {
    simplices_ = simplices;
  }
  hasBeenChecked_ = false;
}

/* Simplex accessor */
Indices Mesh::getSimplex(const UnsignedInteger index) const
{
  if (index >= getSimplicesNumber()) throw InvalidArgumentException(HERE) << "Error: the simplex index=" << index << " must be less than the number of simplices=" << getSimplicesNumber();
  return Indices(simplices_.cbegin_at(index), simplices_.cend_at(index));
}

/* Check the mesh validity */
void Mesh::checkValidity() const
{

  if (hasBeenChecked_) return;
  // Check the vertices: no duplicate, no unused vertex
  // Check the simplices: no simplex with duplicate vertices, no simplex with unknown vertex, no simplex with a number of vertices different from dimension+1
  for (UnsignedInteger i = 0; i < getSimplicesNumber(); ++ i)
  {
    Indices simplex(getSimplex(i));
    if (simplex.getSize() != getDimension() + 1)
      throw InvalidArgumentException(HERE) << "Error: mesh has dimension " << getDimension() << " but simplex #" << i << " has size" << simplex.getSize();

    if (!simplex.check(getVerticesNumber()))
      throw InvalidArgumentException(HERE) << "Error: mesh has " << getVerticesNumber() << " vertices but simplex #" << i << " refers to an unknown vertex";
  }
  // Check that no ball can be included into the intersection of two simplices
  // One it has been checked everything is ok
  hasBeenChecked_ = true;
}

Bool Mesh::isValid() const
{
  Bool result = true;
  try
  {
    checkValidity();
  }
  catch (Exception &)
  {
    result = false;
  }
  return result;
}

/* Build the affine matrix associated to the simplex at the given index*/
void Mesh::buildSimplexMatrix(const UnsignedInteger index,
                              SquareMatrix & matrix) const
{
  if (index >= getSimplicesNumber()) throw InvalidArgumentException(HERE) << "Error: the simplex index=" << index << " must be less than the number of simplices=" << getSimplicesNumber();
  if (matrix.getDimension() != dimension_ + 1)
    matrix = SquareMatrix(dimension_ + 1);
  // Loop over the vertices of the simplex
  for (UnsignedInteger j = 0; j <= dimension_; ++j)
  {
    const UnsignedInteger vertexJ = simplices_(index, j);
    for (UnsignedInteger i = 0; i < dimension_; ++i) matrix(i, j) = vertices_(vertexJ, i);
    matrix(dimension_, j) = 1.0;
  }
}

/* Check if the given point is in the given simplex and returns its barycentric coordinates */
Bool Mesh::checkPointInSimplexWithCoordinates(const Point & point,
    const UnsignedInteger index,
    Point & coordinates) const
{
  if (index >= getSimplicesNumber()) return false;
  const Scalar epsilon = ResourceMap::GetAsScalar("Mesh-VertexEpsilon");
  if (dimension_ == 1)
  {
    const Scalar x = point[0];
    const Scalar x0 = vertices_(simplices_(index, 0), 0);
    const Scalar x1 = vertices_(simplices_(index, 1), 0);
    if ((x - x0) * (x - x1) > epsilon)
      return false;
    coordinates = Point(2);
    if (x0 == x1)
    {
      // x, x0 and x1 are amost at the same position, any value would work.
      coordinates[0] = 1.0;
      coordinates[1] = 0.0;
    }
    else
    {
      const Scalar alpha = (x1 - x) / (x1 - x0);
      coordinates[0] = alpha;
      coordinates[1] = 1.0 - alpha;
    }
    return true;
  }
  else if (dimension_ == 2)
  {
    const Scalar x0 = vertices_(simplices_(index, 0), 0);
    const Scalar y0 = vertices_(simplices_(index, 0), 1);
    const Scalar x01 = vertices_(simplices_(index, 1), 0) - x0;
    const Scalar y01 = vertices_(simplices_(index, 1), 1) - y0;
    const Scalar x02 = vertices_(simplices_(index, 2), 0) - x0;
    const Scalar y02 = vertices_(simplices_(index, 2), 1) - y0;
    const Scalar det = (x02 * y01 - y02 * x01);
    if (det == 0.0)
    {
      return false;
    }
    const Scalar x = point[0] - x0;
    const Scalar y = point[1] - y0;
    coordinates = Point(3);
    coordinates[1] = (x02 * y - y02 * x) / det;
    coordinates[2] = (x * y01 - y * x01) / det;
    coordinates[0] = 0.5 + (0.5 - coordinates[1] - coordinates[2]);
    return coordinates[0] >= -epsilon && coordinates[0] <= 1.0 + epsilon && coordinates[1] >= -epsilon && coordinates[1] <= 1.0 + epsilon && coordinates[2] >= -epsilon && coordinates[2] <= 1.0 + epsilon;
  }
  SquareMatrix matrix(dimension_ + 1);
  buildSimplexMatrix(index, matrix);
  Point v(point);
  v.add(1.0);
  coordinates = matrix.solveLinearSystem(v, false);
  for (UnsignedInteger i = 0; i <= dimension_; ++i) if ((coordinates[i] < -epsilon) || (coordinates[i] > 1.0 + epsilon)) return false;
  return true;
}

/* Get the number of vertices */
UnsignedInteger Mesh::getVerticesNumber() const
{
  return vertices_.getSize();
}

/* Get the number of simplices */
UnsignedInteger Mesh::getSimplicesNumber() const
{
  return simplices_.getSize();
}

Point Mesh::computeSimplicesVolume() const
{
  const UnsignedInteger nrSimplices = getSimplicesNumber();
  Point result(nrSimplices);
  if (nrSimplices == 0) return result;
  if (getDimension() == 1)
  {
    for (UnsignedInteger index = 0; index < nrSimplices; ++index)
    {
      const Scalar x0 = vertices_(simplices_(index, 0), 0);
      const Scalar x1 = vertices_(simplices_(index, 1), 0);
      result[index] = std::abs(x1 - x0);
    }
  }
  else if (getDimension() == 2)
  {
    for (UnsignedInteger index = 0; index < nrSimplices; ++index)
    {
      const Scalar x0 = vertices_(simplices_(index, 0), 0);
      const Scalar y0 = vertices_(simplices_(index, 0), 1);
      const Scalar x01 = vertices_(simplices_(index, 1), 0) - x0;
      const Scalar y01 = vertices_(simplices_(index, 1), 1) - y0;
      const Scalar x02 = vertices_(simplices_(index, 2), 0) - x0;
      const Scalar y02 = vertices_(simplices_(index, 2), 1) - y0;
      result[index] = 0.5 * std::abs(x02 * y01 - x01 * y02);
    }
  }
  else if (getDimension() == 3)
  {
    for (UnsignedInteger index = 0; index < nrSimplices; ++index)
    {
      const Scalar x0 = vertices_(simplices_(index, 0), 0);
      const Scalar y0 = vertices_(simplices_(index, 0), 1);
      const Scalar z0 = vertices_(simplices_(index, 0), 2);
      const Scalar x01 = vertices_(simplices_(index, 1), 0) - x0;
      const Scalar y01 = vertices_(simplices_(index, 1), 1) - y0;
      const Scalar z01 = vertices_(simplices_(index, 1), 2) - z0;
      const Scalar x02 = vertices_(simplices_(index, 2), 0) - x0;
      const Scalar y02 = vertices_(simplices_(index, 2), 1) - y0;
      const Scalar z02 = vertices_(simplices_(index, 2), 2) - z0;
      const Scalar x03 = vertices_(simplices_(index, 3), 0) - x0;
      const Scalar y03 = vertices_(simplices_(index, 3), 1) - y0;
      const Scalar z03 = vertices_(simplices_(index, 3), 2) - z0;
      result[index] = std::abs(x01 * (y02 * z03 - z02 * y03) + y01 * (z02 * x03 - x02 * z03) + z01 * (x02 * y03 - y02 * x03)) / 6.0;
    }
  }
  else
  {
    // General case
    SquareMatrix matrix(getDimension() + 1);
    Scalar sign = 0.0;
    const Scalar logGamma = SpecFunc::LogGamma(getDimension() + 1);
    for (UnsignedInteger index = 0; index < nrSimplices; ++index)
    {
      buildSimplexMatrix(index, matrix);
      result[index] = exp(matrix.computeLogAbsoluteDeterminant(sign, false) - logGamma);
    }
  }
  return result;
}

/* Compute P1 gram matrix */
CovarianceMatrix Mesh::computeP1Gram() const
{
  // If no simplex, the P1 gram matrix is null
  if (simplices_.getSize() == 0) return CovarianceMatrix(0);
  const UnsignedInteger simplexSize = getVertices().getDimension() + 1;
  SquareMatrix elementaryGram(simplexSize, Point(simplexSize * simplexSize, 1.0 / SpecFunc::Gamma(simplexSize + 2.0)));
  for (UnsignedInteger i = 0; i < simplexSize; ++i) elementaryGram(i, i) *= 2.0;
  const UnsignedInteger verticesSize = vertices_.getSize();
  const UnsignedInteger simplicesSize = simplices_.getSize();
  const Point simplexVolume(computeSimplicesVolume());
  SquareMatrix gram(verticesSize);
  for (UnsignedInteger i = 0; i < simplicesSize; ++i)
  {
    const Indices simplex(getSimplex(i));
    const Scalar delta = simplexVolume[i];
    for (UnsignedInteger j = 0; j < simplexSize; ++j)
    {
      const UnsignedInteger newJ = simplex[j];
      for (UnsignedInteger k = 0; k < simplexSize; ++k)
      {
        const UnsignedInteger newK = simplex[k];
        gram(newJ, newK) += delta * elementaryGram(j, k);
      } // Loop over second vertex
    } // Loop over first vertex
  } // Loop over simplices
  return CovarianceMatrix(gram.getImplementation());
}

/* Get the numerical volume of the domain */
Scalar Mesh::getVolume() const
{
  const Point volumes(computeSimplicesVolume());
  return volumes.norm1();
}

/* Check if the domain is empty, i.e if its numerical volume is zero */
Bool Mesh::isEmpty() const
{
  return isNumericallyEmpty();
}

Bool Mesh::isNumericallyEmpty() const
{
  return getVolume() <= ResourceMap::GetAsScalar("Domain-SmallVolume");
}

/* Tells if the mesh is regular */
Bool Mesh::isRegular() const
{
  // For now, only 1D regular meshes are considered
  if (getDimension() != 1) return false;
  const UnsignedInteger size = getSimplicesNumber();
  if (size <= 1) return true;
  Bool regular = true;
  const Scalar epsilon = ResourceMap::GetAsScalar("Mesh-VertexEpsilon");
  const Scalar step = vertices_(simplices_(0, 1), 0) - vertices_(simplices_(0, 0), 0);
  const Scalar absStep = std::abs(step);
  for (UnsignedInteger i = 1; (i < size) && regular; ++ i)
  {
    const Scalar delta = vertices_(simplices_(i, 1), 0) - vertices_(simplices_(i, 0), 0);
    regular = (std::abs(delta - step) <= absStep * epsilon);
  }
  return regular;
}

/* Lower bound of the bounding box */
Point Mesh::getLowerBound() const
{
  return vertices_.getMin();
}

/* Upper bound of the bounding box */
Point Mesh::getUpperBound() const
{
  return vertices_.getMax();
}

/* Orientation management */
void Mesh::fixOrientation()
{
  SquareMatrix matrix(dimension_ + 1);
  const UnsignedInteger size = getSimplicesNumber();
  for (UnsignedInteger i = 0; i < size; ++i)
    fixOrientation(i, matrix);
}

void Mesh::fixOrientation(const UnsignedInteger & index,
                          SquareMatrix & matrix)
{
  if (getDimension() == 1)
  {
    IndicesCollection::iterator cit = simplices_.begin_at(index);
    if (vertices_(*(cit + 1), 0) < vertices_(*cit, 0)) std::swap(*cit, *(cit + 1));
    return;
  }
  if (getDimension() == 2)
  {
    IndicesCollection::iterator cit = simplices_.begin_at(index);
    const Scalar v1x = vertices_(*cit, 0);
    const Scalar v1y = vertices_(*cit, 1);
    const Scalar v2x = vertices_(*(cit + 1), 0);
    const Scalar v2y = vertices_(*(cit + 1), 1);
    const Scalar v3x = vertices_(*(cit + 2), 0);
    const Scalar v3y = vertices_(*(cit + 2), 1);
    if ((v3x - v2x) * (v1y - v2y) < (v1x - v2x) * (v3y - v2y)) std::swap(*cit, *(cit + 1));
    return;
  }
  if (getDimension() == 3)
  {
    IndicesCollection::iterator cit = simplices_.begin_at(index);
    const Scalar v1x = vertices_(*cit, 0);
    const Scalar v1y = vertices_(*cit, 1);
    const Scalar v1z = vertices_(*cit, 2);
    const Scalar v2x = vertices_(*(cit + 1), 0);
    const Scalar v2y = vertices_(*(cit + 1), 1);
    const Scalar v2z = vertices_(*(cit + 1), 2);
    const Scalar v3x = vertices_(*(cit + 2), 0);
    const Scalar v3y = vertices_(*(cit + 2), 1);
    const Scalar v3z = vertices_(*(cit + 2), 2);
    const Scalar v4x = vertices_(*(cit + 3), 0);
    const Scalar v4y = vertices_(*(cit + 3), 1);
    const Scalar v4z = vertices_(*(cit + 3), 2);
    if ((v1x - v4x) * ((v2y - v4y) * (v3z - v4z) - (v3y - v4y) * (v2z - v4z)) + (v3x - v4x) * ((v1y - v4y) * (v2z - v4z) - (v2y - v4y) * (v1z - v4z)) < (v2x - v4x) * ((v1y - v4y) * (v3z - v4z) - (v3y - v4y) * (v1z - v4z)) ) std::swap(*cit, *(cit + 1));
    return;
  }
  buildSimplexMatrix(index, matrix);
  Scalar sign = 0.0;
  (void) matrix.computeLogAbsoluteDeterminant(sign, false);
  // In odd dimension the positive orientation is for a negative determinant of
  // the simplex matrix
  if ((sign > 0.0) == (dimension_ % 2 == 1)) return;
  IndicesCollection::iterator cit = simplices_.begin_at(index);
  std::swap(*cit, *(cit + 1));
}

/* Compute weights such that an integral of a function over the mesh
 * is a weighted sum of its values at the vertices
 */
Point Mesh::computeWeights() const
{
  // Compute the weights of the vertices by distributing the volume of each simplex among its vertices
  checkValidity();
  const UnsignedInteger numVertices = getVerticesNumber();
  const UnsignedInteger numSimplices = getSimplicesNumber();
  Point weights(numVertices, 0.0);
  Point simplexVolume(computeSimplicesVolume());
  for (UnsignedInteger simplex = 0; simplex < numSimplices; ++simplex)
  {
    const Scalar weight = simplexVolume[simplex];
    for (IndicesCollection::const_iterator cit = simplices_.cbegin_at(simplex); cit != simplices_.cend_at(simplex); ++cit)
    {
      weights[*cit] += weight;
    }
  }
  // Normalize the weights: each simplex has dim+1 vertices, so each vertex
  // get 1/(dim+1) of the volume of the simplices it belongs to
  weights /= (dimension_ + 1.0);
  return weights;
}

/* Comparison operator */
Bool Mesh::operator == (const Mesh & other) const
{
  if (this == &other) return true;
  return (vertices_ == other.vertices_) && (simplices_ == other.simplices_);
}

/* String converter */
String Mesh::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " dimension=" << getDimension()
         << " vertices=" << vertices_.__repr__()
         << " simplices=" << simplices_.__repr__();
}

String Mesh::__str__(const String & ) const
{
  return __repr__();
}

/* Drawing method */
Graph Mesh::draw() const
{
  if (dimension_ > 3) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh of dimension > 3.";
  if (dimension_ == 1) return draw1D();
  if (dimension_ == 2) return draw2D();
  if (dimension_ == 3) return draw3D();
  return Graph();
}

Graph Mesh::draw1D() const
{
  checkValidity();
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh of dimension different from 1 with the draw1D() method.";
  const UnsignedInteger verticesSize = getVerticesNumber();
  const UnsignedInteger simplicesSize = getSimplicesNumber();
  if (verticesSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh with no vertex.";
  Graph graph(String(OSS() << "Mesh " << getName()), "", getDescription()[0], true, "topright");
  // The vertices
  Cloud vertices(vertices_, Sample(verticesSize, Point(1, 0.0)));
  vertices.setColor("red");
  vertices.setLegend(String(OSS() << verticesSize << " node" << (verticesSize > 1 ? "s" : "")));
  // The simplices
  for (UnsignedInteger i = 0; i < simplicesSize; ++i)
  {
    Sample data(2, 2);
    data(0, 0) = vertices_(simplices_(i, 0), 0);
    data(1, 0) = vertices_(simplices_(i, 1), 0);
    Curve simplex(data);
    simplex.setColor("blue");
    if (i == 0) simplex.setLegend(String(OSS() << simplicesSize << " element" << (simplicesSize > 1 ? "s" : "")));
    graph.add(simplex);
  }
  graph.add(vertices);
  return graph;
}

Graph Mesh::draw2D() const
{
  checkValidity();
  const UnsignedInteger verticesSize = getVerticesNumber();
  const UnsignedInteger simplicesSize = getSimplicesNumber();
  if (verticesSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh with no vertex.";
  Graph graph(String(OSS() << "Mesh " << getName()), getDescription()[0], getDescription()[1], true, "topright");
  // The vertices
  Cloud vertices(vertices_);
  vertices.setColor("red");
  if (vertices_.getSize() > ResourceMap::GetAsUnsignedInteger("Mesh-LargeSize")) vertices.setPointStyle("dot");
  vertices.setLegend(String(OSS() << verticesSize << " node" << (verticesSize > 1 ? "s" : "")));
  // The simplices
  for (UnsignedInteger i = 0; i < simplicesSize; ++i)
  {
    Sample data(4, 2);
    data[0] = vertices_[simplices_(i, 0)];
    data[1] = vertices_[simplices_(i, 1)];
    data[2] = vertices_[simplices_(i, 2)];
    data[3] = vertices_[simplices_(i, 0)];
    Curve simplex(data);
    simplex.setColor("blue");
    if (i == 0) simplex.setLegend(String(OSS() << simplicesSize << " element" << (simplicesSize > 1 ? "s" : "")));
    graph.add(simplex);
  }
  graph.add(vertices);
  return graph;
}

Graph Mesh::draw3D(const Bool drawEdge,
                   const Scalar thetaX,
                   const Scalar thetaY,
                   const Scalar thetaZ,
                   const Bool shading,
                   const Scalar rho) const
{
  SquareMatrix R(3);
  const Scalar sinThetaX = sin(thetaX);
  const Scalar cosThetaX = cos(thetaX);
  const Scalar sinThetaY = sin(thetaY);
  const Scalar cosThetaY = cos(thetaY);
  const Scalar sinThetaZ = sin(thetaZ);
  const Scalar cosThetaZ = cos(thetaZ);
  R(0, 0) =  cosThetaY * cosThetaZ;
  R(1, 0) = -cosThetaY * sinThetaZ;
  R(2, 0) =  sinThetaY;
  R(0, 1) =  cosThetaX * sinThetaZ + sinThetaX * sinThetaY * cosThetaZ;
  R(1, 1) =  cosThetaX * cosThetaZ - sinThetaX * sinThetaY * sinThetaZ;
  R(2, 1) = -sinThetaX * cosThetaY;
  R(0, 2) =  sinThetaX * sinThetaZ - cosThetaX * sinThetaY * cosThetaZ;
  R(1, 2) =  sinThetaX * cosThetaZ + cosThetaX * sinThetaY * sinThetaZ;
  R(2, 2) =  cosThetaX * cosThetaY;
  return draw3D(drawEdge, R, shading, rho);
}

namespace
{
// Check whether a face of a simplex is inner or on a boundary.  Arguments must be sorted.
Bool Mesh_isInnerFace(const Indices & simplices0, const Indices & simplices1, const Indices & simplices2)
{
  std::vector<UnsignedInteger> common01;
  std::set_intersection(simplices0.begin(), simplices0.end(), simplices1.begin(), simplices1.end(), std::back_inserter(common01));
  if (common01.size() < 2) return false;
  std::vector<UnsignedInteger> common012;
  std::set_intersection(simplices2.begin(), simplices2.end(), common01.begin(), common01.end(), std::back_inserter(common012));
  return common012.size() > 1;
}

// Check whether a face of a simplex is oriented toward the front or back
Bool Mesh_isVisible(const Point & visuVertex0, const Point & visuVertex1, const Point & visuVertex2)
{
  return (visuVertex1[0] - visuVertex0[0]) * (visuVertex2[1] - visuVertex0[1]) <= (visuVertex1[1] - visuVertex0[1]) * (visuVertex2[0] - visuVertex0[0]);
}
} // anonymous namespace

Graph Mesh::draw3D(const Bool drawEdge,
                   const SquareMatrix & rotation,
                   const Bool shading,
                   const Scalar rho) const
{
  checkValidity();
  // First, check if the matrix is a rotation matrix of R^3
  if (rotation.getDimension() != 3) throw InvalidArgumentException(HERE) << "Error: the matrix is not a 3d square matrix.";
  if (Point((rotation * rotation.transpose() - IdentityMatrix(3)).getImplementation()).norm() > 1e-5) throw InvalidArgumentException(HERE) << "Error: the matrix is not a rotation matrix.";
  const UnsignedInteger verticesSize = getVerticesNumber();
  const UnsignedInteger simplicesSize = getSimplicesNumber();
  if (verticesSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh with no vertex or no simplex.";
  // We use a basic Painter algorithm for the visualization
  // Second, transform the vertices if needed
  const Bool noRotation = rotation.isDiagonal();
  const Point verticesCenter(noRotation ? Point(3) : vertices_.computeMean());
  const Sample visuVertices(noRotation ? vertices_ : rotation.getImplementation()->genSampleProd(vertices_ - verticesCenter, true, false, 'R') + verticesCenter);

  // Third, split all the simplices into triangles and compute their mean depth
  Collection< std::pair<Scalar, Indices> > trianglesAndDepth(4 * simplicesSize);
  UnsignedInteger triangleIndex = 0;
  Indices triangle(3);
  const UnsignedInteger numSimplices = getSimplicesNumber();
  const UnsignedInteger numVertices = getVerticesNumber();
  Collection<Indices> mapVerticesToSimplices(numVertices, Indices(0));
  for (UnsignedInteger i = 0; i < numSimplices; ++i)
  {
    for (IndicesCollection::const_iterator cit = simplices_.cbegin_at(i), guard = simplices_.cend_at(i); cit != guard; ++cit)
    {
      mapVerticesToSimplices[*cit].add(i);
    }
  } // Loop over simplices
  IndicesCollection verticesToSimplices(mapVerticesToSimplices);
  const Bool backfaceCulling = ResourceMap::GetAsBool("Mesh-BackfaceCulling");
  for (UnsignedInteger i = 0; i < simplicesSize; ++i)
  {
    const UnsignedInteger i0 = simplices_(i, 0);
    const UnsignedInteger i1 = simplices_(i, 1);
    const UnsignedInteger i2 = simplices_(i, 2);
    const UnsignedInteger i3 = simplices_(i, 3);
    const Indices simplicesVertex0(verticesToSimplices.cbegin_at(i0), verticesToSimplices.cend_at(i0));
    const Indices simplicesVertex1(verticesToSimplices.cbegin_at(i1), verticesToSimplices.cend_at(i1));
    const Indices simplicesVertex2(verticesToSimplices.cbegin_at(i2), verticesToSimplices.cend_at(i2));
    const Indices simplicesVertex3(verticesToSimplices.cbegin_at(i3), verticesToSimplices.cend_at(i3));
    const Point visuVertex0(visuVertices[i0]);
    const Point visuVertex1(visuVertices[i1]);
    const Point visuVertex2(visuVertices[i2]);
    const Point visuVertex3(visuVertices[i3]);
    // First face: AB=p0p1, AC=p0p2.
    if (((!backfaceCulling) || Mesh_isVisible(visuVertex0, visuVertex1, visuVertex2)) && (!Mesh_isInnerFace(simplicesVertex0, simplicesVertex1, simplicesVertex2)))
    {
      triangle = {i0, i1, i2};
      trianglesAndDepth[triangleIndex].first = visuVertices(i0, 2) + visuVertices(i1, 2) + visuVertices(i2, 2);
      trianglesAndDepth[triangleIndex].second = triangle;
      ++triangleIndex;
    }

    // Second face: AB=p0p2, AC=p0p3.
    if (((!backfaceCulling) || Mesh_isVisible(visuVertex0, visuVertex2, visuVertex3)) && (!Mesh_isInnerFace(simplicesVertex0, simplicesVertex2, simplicesVertex3)))
    {
      triangle[0] = i0;
      triangle[1] = i2;
      triangle[2] = i3;
      trianglesAndDepth[triangleIndex].first = visuVertices(i0, 2) + visuVertices(i2, 2) + visuVertices(i3, 2);
      trianglesAndDepth[triangleIndex].second = triangle;
      ++triangleIndex;
    }

    // Third face: AB=p0p3, AC=p0p1.
    if (((!backfaceCulling) || Mesh_isVisible(visuVertex0, visuVertex3, visuVertex1)) && (!Mesh_isInnerFace(simplicesVertex0, simplicesVertex3, simplicesVertex1)))
    {
      triangle[0] = i0;
      triangle[1] = i3;
      triangle[2] = i1;
      trianglesAndDepth[triangleIndex].first = visuVertices(i0, 2) + visuVertices(i3, 2) + visuVertices(i1, 2);
      trianglesAndDepth[triangleIndex].second = triangle;
      ++triangleIndex;
    }

    // Fourth face: AB=p1p3, AC=p1p2.
    if (((!backfaceCulling) || Mesh_isVisible(visuVertex1, visuVertex3, visuVertex2)) && (!Mesh_isInnerFace(simplicesVertex1, simplicesVertex3, simplicesVertex2)))
    {
      triangle[0] = i1;
      triangle[1] = i3;
      triangle[2] = i2;
      trianglesAndDepth[triangleIndex].first = visuVertices(i1, 2) + visuVertices(i3, 2) + visuVertices(i2, 2);
      trianglesAndDepth[triangleIndex].second = triangle;
      ++triangleIndex;
    }
  }
  trianglesAndDepth.resize(triangleIndex);

  // Fourth, draw the triangles in decreasing depth
  Graph graph(String(OSS() << "Mesh " << getName()), getDescription()[0], getDescription()[1], true, "topright");
  std::sort(trianglesAndDepth.begin(), trianglesAndDepth.end());
  Scalar clippedRho = std::min(1.0, std::max(0.0, rho));
  if (rho != clippedRho) LOGWARN(OSS() << "The shrinking factor must be in (0,1), here rho=" << rho);
  Sample face(3, 2);
  Sample data;
  Description palette;
  if (!drawEdge)
  {
    data = Sample(3 * trianglesAndDepth.getSize(), 2);
    palette = Description(trianglesAndDepth.getSize());
  }
  UnsignedInteger base = 0;
  UnsignedInteger index = 0;

  const Scalar kSpecular = ResourceMap::GetAsScalar("Mesh-SpecularFactor");
  const Scalar kDiffuse = ResourceMap::GetAsScalar("Mesh-DiffuseFactor");
  const Scalar kAmbient = ResourceMap::GetAsScalar("Mesh-AmbientFactor");
  const Scalar shininess = ResourceMap::GetAsScalar("Mesh-Shininess");

  const Scalar redAmbient = 1.0;
  const Scalar greenAmbient = 1.0;
  const Scalar blueAmbient = 0.0;
  Point Iambient(3);
  Iambient[0] = kAmbient * redAmbient;
  Iambient[1] = kAmbient * greenAmbient;
  Iambient[2] = kAmbient * blueAmbient;

  const Scalar redFace = 0.0;
  const Scalar greenFace = 0.0;
  const Scalar blueFace = 1.0;

  const Scalar redEdge = 1.0;
  const Scalar greenEdge = 0.0;
  const Scalar blueEdge = 0.0;

  const Scalar redLight = 1.0;
  const Scalar greenLight = 1.0;
  const Scalar blueLight = 1.0;
  Point Ilight(3);

  // Will be modified if shading == true
  String faceColor = Drawable::ConvertFromRGB(redFace, greenFace, blueFace);
  String edgeColor = Drawable::ConvertFromRGB(redEdge, greenEdge, blueEdge);

  for (UnsignedInteger i = trianglesAndDepth.getSize(); i > 0; --i)
  {
    const UnsignedInteger i0 = trianglesAndDepth[i - 1].second[0];
    const UnsignedInteger i1 = trianglesAndDepth[i - 1].second[1];
    const UnsignedInteger i2 = trianglesAndDepth[i - 1].second[2];
    if (clippedRho < 1.0)
    {
      const Point faceCenter((visuVertices[i0] + visuVertices[i1] + visuVertices[i2]) / 3.0);
      face(0, 0) = faceCenter[0];
      face(0, 1) = faceCenter[1];
      face(1, 0) = faceCenter[0];
      face(1, 1) = faceCenter[1];
      face(2, 0) = faceCenter[0];
      face(2, 1) = faceCenter[1];
      if (clippedRho > 0.0)
      {
        face(0, 0) += clippedRho * (visuVertices(i0, 0) - faceCenter[0]);
        face(0, 1) += clippedRho * (visuVertices(i0, 1) - faceCenter[1]);
        face(1, 0) += clippedRho * (visuVertices(i1, 0) - faceCenter[0]);
        face(1, 1) += clippedRho * (visuVertices(i1, 1) - faceCenter[1]);
        face(2, 0) += clippedRho * (visuVertices(i2, 0) - faceCenter[0]);
        face(2, 1) += clippedRho * (visuVertices(i2, 1) - faceCenter[1]);
      }
    }
    else
    {
      face(0, 0) = visuVertices(i0, 0);
      face(0, 1) = visuVertices(i0, 1);
      face(1, 0) = visuVertices(i1, 0);
      face(1, 1) = visuVertices(i1, 1);
      face(2, 0) = visuVertices(i2, 0);
      face(2, 1) = visuVertices(i2, 1);
    }

    if (shading)
    {
      // The light source is behind the observer
      const Point ab(visuVertices[i1] - visuVertices[i0]);
      const Point ac(visuVertices[i2] - visuVertices[i0]);
      Point N(3);
      // The normal is vect(ab, ac)
      N[0] = ab[1] * ac[2] - ab[2] * ac[1];
      N[1] = ab[2] * ac[0] - ab[0] * ac[2];
      N[2] = ab[0] * ac[1] - ab[1] * ac[0];
      N /= N.norm();
      // Flip the normal if it is pointing backward
      if (N[2] < 0.0) N *= -1.0;
      const Scalar cosTheta = N[2];
      // R is a unit vector by construction
      Point R(N * (2.0 * cosTheta));
      R[2] -= 1.0;
      const Scalar cosPhi = std::abs(R[2]);
      const Scalar Idiffuse = kDiffuse * cosTheta;
      const Scalar Ispecular = kSpecular * pow(cosPhi, shininess);
      Ilight[0] = Ispecular * redLight;
      Ilight[1] = Ispecular * greenLight;
      Ilight[2] = Ispecular * blueLight;
      // Face color using Phong model
      faceColor = Drawable::ConvertFromRGB(Iambient[0] + Idiffuse * redFace + Ilight[0], Iambient[1] + Idiffuse * greenFace + Ilight[1], Iambient[2] + Idiffuse * blueFace + Ilight[2]);
      edgeColor = Drawable::ConvertFromRGB(Iambient[0] + Idiffuse * redEdge + Ilight[0], Iambient[1] + Idiffuse * greenEdge + Ilight[1], Iambient[2] + Idiffuse * blueEdge + Ilight[2]);
    } // shading
    if (drawEdge)
    {
      Polygon faceAndEdge(face);
      faceAndEdge.setColor(faceColor);
      faceAndEdge.setEdgeColor(edgeColor);
      graph.add(faceAndEdge);
    } // drawEdge
    else
    {
      data(base, 0) = face(0, 0);
      data(base, 1) = face(0, 1);
      data(base + 1, 0) = face(1, 0);
      data(base + 1, 1) = face(1, 1);
      data(base + 2, 0) = face(2, 0);
      data(base + 2, 1) = face(2, 1);
      base += 3;
      palette[index] = faceColor;
      ++index;
    } // !drawEdge
  }
  if (!drawEdge) graph.add(PolygonArray(data, 3, palette));
  return graph;
}

/* Import mesh from FreeFem 2D mesh files */
Mesh Mesh::ImportFromMSHFile(const String & fileName)
{
  std::ifstream file(fileName.c_str(), std::ios::in);
  if (!file) throw FileNotFoundException(HERE) << "Error: can't open file " << fileName;
  // Bording case: empty file
  if (file.eof())
  {
    Log::Info(OSS() << "File " << fileName << " is empty.");
    return Mesh();
  }
  // First, the header: it is made of 3 integers, the number of vertices, the number of simplices and the number of elements on the boundary, currently not used by OT
  UnsignedInteger verticesNumber = 0;
  UnsignedInteger simplicesNumber = 0;
  UnsignedInteger scratch = 0;
  file >> verticesNumber;
  file >> simplicesNumber;
  file >> scratch;
  LOGINFO(OSS() << "Number of vertices=" << verticesNumber << ", number of simplices=" << simplicesNumber);
  // Parse the vertices
  Sample vertices(verticesNumber, 2);
  for (UnsignedInteger i = 0; i < verticesNumber; ++i)
  {
    file >> vertices(i, 0);
    file >> vertices(i, 1);
    file >> scratch;
    LOGINFO(OSS() << "vertex " << i << "=" << vertices[i]);
  }
  // Parse the simplices
  IndicesCollection simplices(simplicesNumber, 3);
  for (UnsignedInteger i = 0; i < simplicesNumber; ++i)
  {
    file >> simplices(i, 0);
    file >> simplices(i, 1);
    file >> simplices(i, 2);
    --simplices(i, 0);
    --simplices(i, 1);
    --simplices(i, 2);
    file >> scratch;
    LOGINFO(OSS() << "simplex " << i << "=" << simplices(i, 0) << " " << simplices(i, 1) << " " << simplices(i, 2));
  }
  file.close();
  Mesh result(vertices, simplices);
  result.fixOrientation();
  return result;
}

/* VTK export */
String Mesh::streamToVTKFormat() const
{
  return streamToVTKFormat(simplices_);
}

String Mesh::streamToVTKFormat(const IndicesCollection & simplices) const
{
  if (dimension_ > 3) throw InvalidDimensionException(HERE) << "Error: cannot export a mesh of dimension=" << dimension_ << " into the VTK format. Maximum dimension is 3.";
  const UnsignedInteger oldPrecision = PlatformInfo::GetNumericalPrecision();
  PlatformInfo::SetNumericalPrecision(16);
  OSS oss;
  // First, the file version and identifier
  oss << "# vtk DataFile Version 3.0\n";
  // Second, the header
  oss << getName() << "\n";
  // Third, the format
  oss << "ASCII\n";
  oss << "\n";
  // Fourth, the data set
  oss << "DATASET UNSTRUCTURED_GRID\n";
  // Fifth, the geometrical and topological data
  // The vertices
  const UnsignedInteger numVertices = getVerticesNumber();
  oss << "POINTS " << numVertices << " float\n";
  for (UnsignedInteger i = 0; i < numVertices; ++i)
  {
    String separator("");
    for (UnsignedInteger j = 0; j < dimension_; ++j, separator = " ")
      oss << separator << vertices_(i, j);
    for (UnsignedInteger j = dimension_; j < 3; ++j)
      oss << separator << "0.0";
    oss << "\n";
  }
  // The simplices
  oss << "\n";
  const UnsignedInteger numSimplices = simplices.getSize();
  // If no simplex, assume that it is a cloud of points
  if (numSimplices == 0)
  {
    oss << "CELLS " << numVertices << " " << 2 * numVertices << "\n";
    for (UnsignedInteger i = 0; i < numVertices; ++i) oss << "1 " << i << "\n";
    oss << "\n";
    oss << "CELL_TYPES " << numVertices << "\n";
    for (UnsignedInteger i = 0; i < numVertices; ++i) oss << "1\n";
    PlatformInfo::SetNumericalPrecision(oldPrecision);
    return oss;
  }
  // There is at least one simplex. Assume homogeneous simplices,
  // ie all the simplices are of the same kind as the first one
  UnsignedInteger verticesPerSimplex = 1;
  UnsignedInteger lastIndex = simplices(0, 0);
  while ((verticesPerSimplex <= dimension_) && (simplices(0, verticesPerSimplex) != lastIndex))
  {
    lastIndex = simplices(0, verticesPerSimplex);
    ++verticesPerSimplex;
  }
  oss << "CELLS " << numSimplices << " " << (verticesPerSimplex + 1) * numSimplices << "\n";
  for (UnsignedInteger i = 0; i < numSimplices; ++i)
  {
    oss << verticesPerSimplex;
    for (UnsignedInteger j = 0; j < verticesPerSimplex; ++j) oss << " " << simplices(i, j);
    oss << "\n";
  }
  oss << "\n";
  // If no simplices, assume vertices type
  oss << "CELL_TYPES " << numSimplices << "\n";
  UnsignedInteger cellType = 0;
  // Cell type is:
  // 1 for vertex
  // 3 for line
  // 5 for triangle
  // 10 for tetrahedron
  if (verticesPerSimplex == 1) cellType = 1;
  if (verticesPerSimplex == 2) cellType = 3;
  if (verticesPerSimplex == 3) cellType = 5;
  if (verticesPerSimplex == 4) cellType = 10;
  for (UnsignedInteger i = 0; i < numSimplices; ++i) oss << cellType << "\n";
  PlatformInfo::SetNumericalPrecision(oldPrecision);
  return oss;
}

void Mesh::exportToVTKFile(const String & fileName) const
{
  exportToVTKFile(fileName, simplices_);
}

void Mesh::exportToVTKFile(const String & fileName,
                           const IndicesCollection & simplices) const
{
  std::ofstream file(fileName.c_str(), std::ios::out);
  if (!file) throw FileNotFoundException(HERE) << "Error: can't open file " << fileName;
  const String content(streamToVTKFormat(simplices));
  file << content;
  file.close();
}

/* Method save() stores the object through the StorageManager */
void Mesh::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("hasBeenChecked_", hasBeenChecked_);
  adv.saveAttribute("vertices_", vertices_);
  adv.saveAttribute("simplices_", simplices_);
}

/* Method load() reloads the object from the StorageManager */
void Mesh::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("hasBeenChecked_", hasBeenChecked_);
  adv.loadAttribute("vertices_", vertices_);
  adv.loadAttribute("simplices_", simplices_);
}

END_NAMESPACE_OPENTURNS
