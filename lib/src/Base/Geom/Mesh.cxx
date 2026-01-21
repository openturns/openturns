//                                               -*- C++ -*-
/**
 *  @brief Mesh is defined as a collection of n-D vertices and simplices
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <deque>

#include "openturns/Mesh.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/PolygonArray.hxx"
#include "openturns/Collection.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PlatformInfo.hxx"

#ifdef OPENTURNS_HAVE_BOOST
#if BOOST_VERSION < 107500
#define BOOST_ALLOW_DEPRECATED_HEADERS
#endif
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/strategies/strategies.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#endif

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Mesh)

static const Factory<Mesh> Factory_Mesh;

/* Default constructor */
Mesh::Mesh(const UnsignedInteger dimension)
  : PersistentObject()
  , dimension_(dimension)
  , vertices_(1, dimension) // At least one point
{
  // Nothing to do
  if (vertices_.getDescription().isBlank()) vertices_.setDescription(Description::BuildDefault(dimension, "t"));
}

/* Parameters constructor, simplified interface for 1D case */
Mesh::Mesh(const Sample & vertices)
  : PersistentObject()
  , dimension_(vertices.getDimension())
  , vertices_(0, vertices.getDimension())
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

/* Intrinsic dimension accessor */
UnsignedInteger Mesh::getIntrinsicDimension() const
{
  if (!getSimplicesNumber())
    return 0;
  UnsignedInteger verticesPerSimplex = 1;
  UnsignedInteger lastIndex = simplices_(0, 0);
  while ((verticesPerSimplex <= dimension_) && (simplices_(0, verticesPerSimplex) != lastIndex))
  {
    lastIndex = simplices_(0, verticesPerSimplex);
    ++verticesPerSimplex;
  }
  return verticesPerSimplex - 1;
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
  if (!(index < getVerticesNumber())) throw InvalidArgumentException(HERE) << "Error: the vertex index=" << index << " must be less than the number of vertices=" << getVerticesNumber();
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
  if (!(index < getSimplicesNumber())) throw InvalidArgumentException(HERE) << "Error: the simplex index=" << index << " must be less than the number of simplices=" << getSimplicesNumber();
  return Indices(simplices_.cbegin_at(index), simplices_.cend_at(index));
}

/* Check the mesh validity */
void Mesh::checkValidity() const
{
  if (hasBeenChecked_) return;

  // Check the vertices: no duplicate, no unused vertex
  // Check the simplices: no simplex with duplicate vertices, no simplex with unknown vertex, no simplex with a number of vertices different from dimension+1
  Indices arity(getVerticesNumber(), 0);
  for (UnsignedInteger i = 0; i < getSimplicesNumber(); ++ i)
  {
    Indices simplex(getSimplex(i));
    if (simplex.getSize() != getDimension() + 1)
      throw InvalidArgumentException(HERE) << "Error: mesh has dimension " << getDimension() << " but simplex #" << i << " has size " << simplex.getSize();

    if (*std::max_element(simplex.begin(), simplex.end()) >= getVerticesNumber())
      throw InvalidArgumentException(HERE) << "Error: mesh has " << getVerticesNumber() << " vertices but simplex #" << i << " = " << simplex << " refers to an unknown vertex";

    for (UnsignedInteger j = 0; j <= getDimension(); ++ j)
      ++ arity[simplex[j]];
  }

  // Check for unused vertex
  const UnsignedInteger unusedVertexIndex = arity.find(0);
  if (unusedVertexIndex < getVerticesNumber())
    throw InvalidArgumentException(HERE) << "Vertex #" << unusedVertexIndex << " is unconnected";

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
  catch (const Exception &)
  {
    result = false;
  }
  return result;
}

/* Build the affine matrix associated to the simplex at the given index*/
void Mesh::buildSimplexMatrix(const UnsignedInteger index,
                              SquareMatrix & matrix) const
{
  if (!(index < getSimplicesNumber())) throw InvalidArgumentException(HERE) << "Error: the simplex index=" << index << " must be less than the number of simplices=" << getSimplicesNumber();
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
  if (!(index < getSimplicesNumber())) return false;
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
  coordinates = matrix.solveLinearSystemInPlace(v);
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
  // No simplex
  if (nrSimplices == 0) return result;
  // The mesh has the same dimension as the ambiant space
  const UnsignedInteger intrinsicDimension = getIntrinsicDimension();
  if (dimension_ == intrinsicDimension)
  {
    if (dimension_ == 1)
    {
      for (UnsignedInteger index = 0; index < nrSimplices; ++index)
      {
        const Scalar x0 = vertices_(simplices_(index, 0), 0);
        const Scalar x1 = vertices_(simplices_(index, 1), 0);
        result[index] = std::abs(x1 - x0);
      }
    } // dimension_ == 1
    else if (dimension_ == 2)
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
    } // dimension_ = 2
    else if (dimension_ == 3)
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
    } // dimension_ == 3
    else
    {
      // General case
      SquareMatrix matrix(dimension_ + 1);
      Scalar sign = 0.0;
      const Scalar logGamma = SpecFunc::LogGamma(dimension_ + 1);
      for (UnsignedInteger index = 0; index < nrSimplices; ++index)
      {
        buildSimplexMatrix(index, matrix);
        result[index] = exp(matrix.computeLogAbsoluteDeterminantInPlace(sign) - logGamma);
      }
    } // dimension_ > 3
  } // dimension_ == getIntrinsicDimension()
  // The mesh is of codimension 1
  else if (dimension_ == intrinsicDimension + 1)
  {
    if (intrinsicDimension == 1)
    {
      for (UnsignedInteger index = 0; index < nrSimplices; ++index)
        result[index] = (vertices_[simplices_(index, 0)] - vertices_[simplices_(index, 1)]).norm();
    } // intrinsicDimension == 1
    else if (intrinsicDimension == 2)
    {
      for (UnsignedInteger index = 0; index < nrSimplices; ++index)
      {
        Scalar a = (vertices_[simplices_(index, 0)] - vertices_[simplices_(index, 1)]).norm();
        Scalar b = (vertices_[simplices_(index, 1)] - vertices_[simplices_(index, 2)]).norm();
        Scalar c = (vertices_[simplices_(index, 2)] - vertices_[simplices_(index, 0)]).norm();
        // Sort a, b, c in decreasing order
        if (a < b)
          std::swap(a, b);
        if (a < c)
          std::swap(a, c);
        if (b < c)
          std::swap(b, c);
        // Here we use a stable formula
        result[index] = 0.25 * std::sqrt((a + (b + c)) * (c - (a - b)) * (c + (a - b)) * (a + (b - c)));
      } // index
    } // intrinsicDimension == 2
    else
    {
      Sample workingVertices(dimension_ + 1, dimension_);
      IndicesCollection simplex(1, dimension_ + 1);
      SquareMatrix M(dimension_);
      SquareMatrix eigenVectors;
      // For each simplex in the mesh
      for (UnsignedInteger index = 0; index < nrSimplices; ++index)
      {
        // We know (https://urresearch.rochester.edu/fileDownloadForInstitutionalItem.action?itemId=13451&itemFileId=31154)
        // that the hyperplane goes through the center of the vertices
        // (ie the mean) and has the eigenvector associated with the
        // smallest eigenvalue of M'M (a semi-definite symmetric real
        // matrix, so its eigenvalues are all nonnegative)
        Point sum(dimension_, 0.0);
        for (UnsignedInteger i = 0; i < dimension_; ++i)
        {
          sum += vertices_[simplices_(index, i)];
          workingVertices[i] = vertices_[simplices_(index, i)];
        }
        // Here we cannot use computeMean as the last vertex has to
        // be dropped from the computation
        workingVertices -= sum / dimension_;
        // Compute a vector normal to the hyperplane containing
        // the current degenerated simplex
        for (UnsignedInteger i = 0; i < dimension_; ++i)
          for (UnsignedInteger j = 0; j < dimension_; ++j)
            M(j, i) = workingVertices(i, j);
        CovarianceMatrix MtM(M.computeGram(false));
        // We know that the eigenvalues are sorted into ascending
        // order so the eigenvector we are looking for is the first one.
        // No need to store the eigenvalues.
        (void) MtM.computeEVInPlace(eigenVectors);
        const Point normal(eigenVectors.getImplementation()->begin(), eigenVectors.getImplementation()->begin() + dimension_);
        // Add the new point defining a full dimension simplex which
        // volume is the surface of the lower dimensional simplex
        for (UnsignedInteger i = 0; i < dimension_; ++i)
        {
          workingVertices(dimension_, i) = workingVertices(dimension_ - 1, i) + normal[i];
          simplex(0, i) = i;
        }
        simplex(0, dimension_) = dimension_;
        result[index] = dimension_ * Mesh(workingVertices, simplex).computeSimplicesVolume()[0];
      } // index
    } // intrinsicDimension > 3
  } // dimension_ == intrinsicDimension + 1
  return result;
}

/* Compute P1 gram matrix */
CovarianceMatrix Mesh::computeP1Gram() const
{
  // If no simplex, the P1 gram matrix is null
  if (simplices_.getSize() == 0) return CovarianceMatrix(0);
  const UnsignedInteger simplexSize = getDimension() + 1;
  SquareMatrix elementaryGram(simplexSize, Point(simplexSize * simplexSize, 1.0 / (simplexSize * (simplexSize + 1.0))));
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
  (void) matrix.computeLogAbsoluteDeterminantInPlace(sign);
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
  return OSS() << getClassName() << "(dimension=" << getDimension()
         << ", vertices=" << vertices_.getSize()
         << ", simplices=" << simplices_.getSize() << ")";
}

/* Drawing method */
Graph Mesh::draw() const
{
  if (!(dimension_ <= 3)) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh of dimension > 3, here dimension=" << dimension_;
  if (dimension_ == 1) return draw1D();
  if (dimension_ == 2) return draw2D();
  if (dimension_ == 3) return draw3D();
  return Graph();
}

Graph Mesh::draw1D() const
{
  checkValidity();
  if (dimension_ != 1) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh of dimension different from 1 with the draw1D() method, here dimension=" << dimension_;
  const UnsignedInteger verticesSize = getVerticesNumber();
  const UnsignedInteger simplicesSize = getSimplicesNumber();
  if (!(verticesSize > 0)) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh with no vertex.";
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
  if (!(verticesSize > 0)) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh with no vertex.";
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
  if (!(Point((rotation * rotation.transpose() - IdentityMatrix(3)).getImplementation()).norm() <= 1e-5)) throw InvalidArgumentException(HERE) << "Error: the matrix is not a rotation matrix.";
  const UnsignedInteger verticesSize = getVerticesNumber();
  const UnsignedInteger simplicesSize = getSimplicesNumber();
  if (!(verticesSize > 0)) throw InvalidArgumentException(HERE) << "Error: cannot draw a mesh with no vertex or no simplex.";
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
  const Scalar clippedRho = SpecFunc::Clip01(rho);
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
    LOGINFO(OSS() << "File " << fileName << " is empty.");
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
  if (!(dimension_ <= 3)) throw InvalidDimensionException(HERE) << "Error: cannot export a mesh of dimension=" << dimension_ << " into the VTK format. Maximum dimension is 3.";
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

/* Intersection */
Mesh Mesh::intersect(const Mesh & other) const
{
  if (getDimension() != other.getDimension())
    throw InvalidArgumentException(HERE) << "Expected a mesh of dimension " << getDimension() << " got " << other.getDimension();
#ifdef OPENTURNS_HAVE_BOOST
  if (getDimension() != 2)
    throw NotYetImplementedException(HERE) << "Cannot compute intersection of a Mesh of dimension != 2";

  typedef boost::geometry::model::d2::point_xy<Scalar> point_t;

  // set clockwise=false for consistency with what IntervalMesher returns
  typedef boost::geometry::model::polygon<point_t, false> polygon_t;

  Sample vertices(0, 2);
  Collection<Indices> simplices;
  // compute the intersection as union of intersection of triangle combinations from each mesh
  for (UnsignedInteger i1 = 0; i1 < getSimplicesNumber(); ++ i1)
  {
    polygon_t tri1;
    for(UnsignedInteger j1 = 0; j1 < 4; ++ j1)
    {
      // the first vertex is repeated at the end
      const Point pj1(vertices_[simplices_(i1, j1 % 3)]);
      boost::geometry::append(tri1.outer(), point_t(pj1[0], pj1[1]));
    }

    // fix orientation
    if (boost::geometry::area(tri1) < 0.0)
    {
      boost::geometry::correct(tri1);
    }

    for (UnsignedInteger i2 = 0; i2 < other.getSimplicesNumber(); ++ i2)
    {
      polygon_t tri2;
      for(UnsignedInteger j2 = 0; j2 < 4; ++ j2)
      {
        // the first vertex is repeated at the end
        const Point pj2(other.vertices_[other.simplices_(i2, j2 % 3)]);
        boost::geometry::append(tri2.outer(), point_t(pj2[0], pj2[1]));
      }
      if (boost::geometry::area(tri2) < 0.0)
        throw InvalidArgumentException(HERE) << "Simplex at index " << i2 << " is not anti-clockwise";
      std::deque<polygon_t> output;
      boost::geometry::intersection(tri1, tri2, output);
      for (const polygon_t & poly : output)
      {
        const UnsignedInteger offset = vertices.getSize();
        // take into account the repeated vertex at the end
        for (UnsignedInteger j3 = 0; j3 < poly.outer().size() - 1; ++ j3)
        {
          const point_t & pj3 = poly.outer()[j3];
          vertices.add(Point({pj3.x(), pj3.y()}));
        }
        // take into account the repeated vertex at the end
        for (UnsignedInteger j3 = 0; j3 < poly.outer().size() - 3; ++ j3)
        {
          // [0,1,2], [0,2,3], [0,3,4], [0,4,5] depending on number of intersection edges ([3-6])
          Indices simplex(3, offset);
          simplex[1] += j3 + 1;
          simplex[2] += j3 + 2;
          simplices.add(simplex);
        } // j3
      } // poly
    } // i2
  } // i1
  IndicesCollection simplices2(simplices.getSize(), 3);
  for (UnsignedInteger i = 0; i < simplices.getSize(); ++ i)
    for (UnsignedInteger j = 0; j < 3; ++ j)
      simplices2(i, j) = simplices[i][j];
  return Mesh(vertices, simplices2);
#else
  throw NotYetImplementedException(HERE) << "No boost support";
#endif
}

Mesh Mesh::getSubMesh(const Indices & simplicesIndices) const
{
  if (!simplicesIndices.check(simplices_.getSize()))
    throw InvalidArgumentException(HERE) << "Simplices indices must be in [0, " << simplices_.getSize() << "[";
  const UnsignedInteger simplexSize = getDimension() + 1;
  IndicesCollection simplices(simplicesIndices.getSize(), simplexSize);
  Sample vertices(0, getDimension());
  std::map <UnsignedInteger, UnsignedInteger> usedVertices;
  for (UnsignedInteger i = 0; i < simplicesIndices.getSize(); ++ i)
    for (UnsignedInteger j = 0; j < simplexSize; ++ j)
    {
      const UnsignedInteger vertexIndex = simplices_(simplicesIndices[i], j);
      UnsignedInteger newVertexIndex = vertices.getSize();
      if (usedVertices.count(vertexIndex))
        newVertexIndex = usedVertices.at(vertexIndex);
      else
      {
        usedVertices[vertexIndex] = newVertexIndex;
        vertices.add(vertices_[vertexIndex]);
      }
      simplices(i, j) = newVertexIndex;
    }
  const Mesh result(vertices, simplices);
  return result;
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
