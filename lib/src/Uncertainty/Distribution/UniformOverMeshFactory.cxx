//                                               -*- C++
/**
 *  @brief Factory for UniformOverMesh distribution
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

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include "openturns/UniformOverMeshFactory.hxx"
#include "openturns/Collection.hxx"
#include "openturns/KDTree.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace
{

struct Point2
{
  Scalar x;
  Scalar y;
};

struct Triplet
{
  UnsignedInteger i;
  UnsignedInteger j;
  UnsignedInteger k;
};

// Cross product (b - a) x (c - a), sign of the orientation of (a, b, c)
Scalar orient2d(const Point2 & a,
                const Point2 & b,
                const Point2 & c)
{
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Is p inside the circumcircle of the CCW triangle (a, b, c)?
Bool inCircle(const Point2 & a,
              const Point2 & b,
              const Point2 & c,
              const Point2 & p)
{
  const Scalar adx = a.x - p.x;
  const Scalar ady = a.y - p.y;
  const Scalar bdx = b.x - p.x;
  const Scalar bdy = b.y - p.y;
  const Scalar cdx = c.x - p.x;
  const Scalar cdy = c.y - p.y;
  const Scalar ad2 = adx * adx + ady * ady;
  const Scalar bd2 = bdx * bdx + bdy * bdy;
  const Scalar cd2 = cdx * cdx + cdy * cdy;
  const Scalar det = adx * (bdy * cd2 - bd2 * cdy)
                     - ady * (bdx * cd2 - bd2 * cdx)
                     + ad2 * (bdx * cdy - bdy * cdx);
  return det > 0.0;
}

// Delaunay triangulation in 2D using the Bowyer-Watson algorithm.
// The input points are augmented with an enclosing super-triangle whose
// vertices have indices nbPoints, nbPoints + 1 and nbPoints + 2. The
// returned triangles only refer to the original points.
std::vector<Triplet> triangulate2D(const std::vector<Point2> & points)
{
  std::vector<Triplet> result;
  const UnsignedInteger nbPoints = static_cast<UnsignedInteger>(points.size());
  if (nbPoints < 3) return result;
  // Bounding box of the points
  Scalar minX = points[0].x;
  Scalar maxX = points[0].x;
  Scalar minY = points[0].y;
  Scalar maxY = points[0].y;
  for (UnsignedInteger i = 1; i < nbPoints; ++i)
  {
    minX = std::min(minX, points[i].x);
    maxX = std::max(maxX, points[i].x);
    minY = std::min(minY, points[i].y);
    maxY = std::max(maxY, points[i].y);
  }
  const Scalar deltaX = maxX - minX + 1.0;
  const Scalar deltaY = maxY - minY + 1.0;
  const Scalar delta = std::max(deltaX, deltaY);
  const Scalar midX = 0.5 * (minX + maxX);
  const Scalar midY = 0.5 * (minY + maxY);
  // Super-triangle enclosing all the points, oriented counter-clockwise
  Point2 a;
  a.x = midX - 2.0 * delta;
  a.y = midY - delta;
  Point2 b;
  b.x = midX + 2.0 * delta;
  b.y = midY - delta;
  Point2 c;
  c.x = midX;
  c.y = midY + 2.0 * delta;
  std::vector<Point2> allPoints(points);
  allPoints.push_back(a);
  allPoints.push_back(b);
  allPoints.push_back(c);
  const UnsignedInteger indexA = nbPoints;
  const UnsignedInteger indexB = nbPoints + 1;
  const UnsignedInteger indexC = nbPoints + 2;
  std::vector<Triplet> triangles(1, Triplet {indexA, indexB, indexC});
  for (UnsignedInteger p = 0; p < nbPoints; ++p)
  {
    const Point2 pt(points[p]);
    // Identify the triangles whose circumcircle contains the point
    std::vector<Bool> badFlag(triangles.size(), false);
    for (UnsignedInteger i = 0; i < triangles.size(); ++i)
    {
      const Triplet t(triangles[i]);
      const Point2 & pa(allPoints[t.i]);
      const Point2 & pb(allPoints[t.j]);
      const Point2 & pc(allPoints[t.k]);
      if (inCircle(pa, pb, pc, pt)) badFlag[i] = true;
    }
    // Boundary edges of the union of the bad triangles
    std::map<std::pair<UnsignedInteger, UnsignedInteger>, UnsignedInteger> edgeCount;
    for (UnsignedInteger i = 0; i < triangles.size(); ++i)
    {
      if (!badFlag[i]) continue;
      const Triplet t(triangles[i]);
      const std::array<UnsignedInteger, 3> vertex = {{t.i, t.j, t.k}};
      for (UnsignedInteger e = 0; e < 3; ++e)
      {
        std::pair<UnsignedInteger, UnsignedInteger> edge(vertex[e], vertex[(e + 1) % 3]);
        if (edge.first > edge.second) std::swap(edge.first, edge.second);
        ++edgeCount[edge];
      }
    }
    // Remove the bad triangles
    std::vector<Triplet> newTriangles;
    for (UnsignedInteger i = 0; i < triangles.size(); ++i)
      if (!badFlag[i]) newTriangles.push_back(triangles[i]);
    triangles.swap(newTriangles);
    // Add the triangles (edge, p) for each boundary edge
    for (std::map<std::pair<UnsignedInteger, UnsignedInteger>, UnsignedInteger>::const_iterator it = edgeCount.begin(); it != edgeCount.end(); ++it)
    {
      if (it->second != 1) continue;
      const UnsignedInteger i = it->first.first;
      const UnsignedInteger j = it->first.second;
      // Valid triangle if the three points are not aligned
      if (std::abs(orient2d(allPoints[i], allPoints[j], pt)) <= SpecFunc::ScalarEpsilon * delta * delta) continue;
      Triplet t;
      t.i = i;
      t.j = j;
      t.k = p;
      // Keep the triangle counter-clockwise
      if (orient2d(allPoints[i], allPoints[j], allPoints[p]) < 0.0) std::swap(t.j, t.k);
      triangles.push_back(t);
    }
  }
  // Discard the triangles incident to the super-triangle
  for (UnsignedInteger i = 0; i < triangles.size(); ++i)
  {
    const Triplet t(triangles[i]);
    if ((t.i < nbPoints) && (t.j < nbPoints) && (t.k < nbPoints))
      result.push_back(t);
  }
  return result;
}

} // anonymous namespace

CLASSNAMEINIT(UniformOverMeshFactory)

static const Factory<UniformOverMeshFactory> Factory_UniformOverMeshFactory;

UniformOverMeshFactory::UniformOverMeshFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

UniformOverMeshFactory * UniformOverMeshFactory::clone() const
{
  return new UniformOverMeshFactory(*this);
}

Distribution UniformOverMeshFactory::build(const Sample & sample) const
{
  return buildAsUniformOverMesh(sample).clone();
}

Distribution UniformOverMeshFactory::build(const Point & parameters) const
{
  return buildAsUniformOverMesh(parameters).clone();
}

Distribution UniformOverMeshFactory::build() const
{
  return buildAsUniformOverMesh().clone();
}

UniformOverMesh UniformOverMeshFactory::buildAsUniformOverMesh() const
{
  return UniformOverMesh();
}

UniformOverMesh UniformOverMeshFactory::buildAsUniformOverMesh(const Point & parameters) const
{
  if (parameters.getDimension() != 0)
    throw InvalidArgumentException(HERE) << "Error: the UniformOverMesh distribution has no parameter, got " << parameters.getDimension() << " values.";
  return UniformOverMesh();
}

UniformOverMesh UniformOverMeshFactory::buildAsUniformOverMesh(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  const UnsignedInteger ambientDimension = sample.getDimension();
  if (ambientDimension < 3)
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a surface mesh from a sample of dimension < 3, here dimension=" << ambientDimension;
  if (size < 6)
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a surface mesh from a sample of size < 6, here size=" << size;
  const UnsignedInteger requestedNeighbors = ResourceMap::GetAsUnsignedInteger("UniformOverMeshFactory-NearestNeighbors");
  const UnsignedInteger k = std::min(requestedNeighbors, size - 1);
  if (k < 5)
    throw InvalidArgumentException(HERE) << "Error: the number of nearest neighbors must be at least 5, got " << k;
  // Nearest neighbors structure
  const KDTree tree(sample);
  // For each point, Delaunay triangulate its neighbors projected onto the
  // local tangent plane estimated by PCA, and gather the resulting triangles.
  std::set<std::tuple<UnsignedInteger, UnsignedInteger, UnsignedInteger> > triangleSet;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point center(sample[i]);
    const Indices neighbors(tree.queryK(center, k, false));
    // Local PCA of the neighbor offsets around the point
    SymmetricMatrix covariance(ambientDimension);
    const Scalar invK = 1.0 / static_cast<Scalar>(k);
    for (UnsignedInteger j = 0; j < k; ++j)
    {
      const Point offset(sample[neighbors[j]] - center);
      for (UnsignedInteger a = 0; a < ambientDimension; ++a)
        for (UnsignedInteger b = 0; b <= a; ++b)
          covariance(a, b) += invK * offset[a] * offset[b];
    }
    SquareMatrix eigenvectors;
    const Point eigenvalues(covariance.computeEVInPlace(eigenvectors));
    const Scalar maxEigenvalue = eigenvalues[ambientDimension - 1];
    const Scalar secondEigenvalue = eigenvalues[ambientDimension - 2];
    if ((maxEigenvalue <= 0.0) || (secondEigenvalue <= 0.0)) continue;
    // The two eigenvectors associated with the largest eigenvalues give a
    // basis of the local tangent plane
    Point basis0(ambientDimension);
    Point basis1(ambientDimension);
    for (UnsignedInteger j = 0; j < ambientDimension; ++j)
    {
      basis0[j] = eigenvectors(j, ambientDimension - 1);
      basis1[j] = eigenvectors(j, ambientDimension - 2);
    }
    // Project the neighbors onto the tangent plane
    std::vector<Point2> chart(k);
    for (UnsignedInteger j = 0; j < k; ++j)
    {
      const Point offset(sample[neighbors[j]] - center);
      Point2 projected;
      projected.x = offset.dot(basis0);
      projected.y = offset.dot(basis1);
      chart[j] = projected;
    }
    // Delaunay triangulate the chart
    const std::vector<Triplet> triangles(triangulate2D(chart));
    for (UnsignedInteger t = 0; t < triangles.size(); ++t)
    {
      const Triplet triplet(triangles[t]);
      if ((triplet.i >= k) || (triplet.j >= k) || (triplet.k >= k)) continue;
      if ((triplet.i == triplet.j) || (triplet.i == triplet.k) || (triplet.j == triplet.k)) continue;
      std::array<UnsignedInteger, 3> vertex = {{neighbors[triplet.i], neighbors[triplet.j], neighbors[triplet.k]}};
      std::sort(vertex.begin(), vertex.end());
      triangleSet.insert(std::make_tuple(vertex[0], vertex[1], vertex[2]));
    }
  }
  if (triangleSet.empty())
    throw InvalidArgumentException(HERE) << "Error: cannot estimate a surface mesh from the given sample.";
  // Convert the set into a vector of triangles and compute their areas
  std::vector<std::array<UnsignedInteger, 3> > triangles;
  triangles.reserve(triangleSet.size());
  for (std::set<std::tuple<UnsignedInteger, UnsignedInteger, UnsignedInteger> >::const_iterator it = triangleSet.begin(); it != triangleSet.end(); ++it)
  {
    std::array<UnsignedInteger, 3> triangle = {{std::get<0>(*it), std::get<1>(*it), std::get<2>(*it)}};
    triangles.push_back(triangle);
  }
  const UnsignedInteger nbTriangles = static_cast<UnsignedInteger>(triangles.size());
  Point areas(nbTriangles);
  Scalar maxArea = 0.0;
  for (UnsignedInteger i = 0; i < nbTriangles; ++i)
  {
    const Point e1(sample[triangles[i][1]] - sample[triangles[i][0]]);
    const Point e2(sample[triangles[i][2]] - sample[triangles[i][0]]);
    const Scalar squared = e1.normSquare() * e2.normSquare() - std::pow(e1.dot(e2), 2.0);
    areas[i] = 0.5 * std::sqrt(std::max(0.0, squared));
    maxArea = std::max(maxArea, areas[i]);
  }
  if (!(maxArea > 0.0))
    throw InvalidArgumentException(HERE) << "Error: the estimated surface mesh is degenerate.";
  // Prune the degenerate triangles, i.e. those of negligible area
  const Scalar areaThreshold = ResourceMap::GetAsScalar("UniformOverMeshFactory-MinimumAreaFactor") * maxArea;
  std::vector<Bool> keep(nbTriangles, true);
  for (UnsignedInteger i = 0; i < nbTriangles; ++i)
    if (areas[i] < areaThreshold) keep[i] = false;
  // Build the mesh from the kept triangles
  Collection<Indices> simplicesCollection;
  for (UnsignedInteger i = 0; i < nbTriangles; ++i)
  {
    if (!keep[i]) continue;
    Indices simplex(3);
    for (UnsignedInteger j = 0; j < 3; ++j) simplex[j] = triangles[i][j];
    simplicesCollection.add(simplex);
  }
  if (simplicesCollection.getSize() == 0)
    throw InvalidArgumentException(HERE) << "Error: the estimated surface mesh is empty after pruning.";
  // Store the simplices following the Mesh convention: the simplex of a
  // mesh of intrinsic dimension 2 embedded in an ambient space of dimension
  // dimension has 3 distinct leading vertex indices followed by trailing
  // entries equal to the last distinct vertex index, so that the number of
  // vertex indices per simplex is dimension + 1
  const UnsignedInteger verticesPerSimplex = sample.getDimension() + 1;
  IndicesCollection simplices(simplicesCollection.getSize(), verticesPerSimplex);
  for (UnsignedInteger i = 0; i < simplicesCollection.getSize(); ++i)
  {
    for (UnsignedInteger j = 0; j < 3; ++j)
      simplices(i, j) = simplicesCollection[i][j];
    for (UnsignedInteger j = 3; j < verticesPerSimplex; ++j)
      simplices(i, j) = simplices(i, 2);
  }
  const Mesh mesh(sample, simplices, false);
  UniformOverMesh result(mesh);
  result.setDescription(sample.getDescription());
  return result;
}

END_NAMESPACE_OPENTURNS