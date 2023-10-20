//                                               -*- C++ -*-
/**
 *  @brief Boundary extraction algorithm for meshes
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
#include <unordered_map>

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/BoundaryMesher.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BoundaryMesher)
static const Factory<BoundaryMesher> Factory_BoundaryMesher;


/* Default constructor */
BoundaryMesher::BoundaryMesher()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
BoundaryMesher * BoundaryMesher::clone() const
{
  return new BoundaryMesher(*this);
}

/* String converter */
String BoundaryMesher::__repr__() const
{
  OSS oss(true);
  oss << "class=" << BoundaryMesher::GetClassName();
  return oss;
}

/* String converter */
String BoundaryMesher::__str__(const String & ) const
{
  return __repr__();
}

/* Here is the interface that all derived class must implement */


namespace
{
struct FaceHasher
{
  // This hash function is taken from
  // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
  // It is adapted to the fact that only the (size-1) first components have to be
  // considered.
  UnsignedInteger operator()(const Indices &face) const
  {
    UnsignedInteger seed = face.getSize() - 1;
    for (UnsignedInteger i = 0; i < face.getSize() - 1; ++i)
    {
      UnsignedInteger x = face[i];
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = (x >> 16) ^ x;
      seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
}; // FaceHasher

struct FaceCompare
{
  Bool operator()(const Indices &face1, const Indices &face2) const
  {
    // The two faces are of size (dimension+1), and we test
    // only the dimension first components for equality
    return std::equal(face1.begin(), face1.end() - 1, face2.begin());
  }
}; // FaceCompare

struct ComputeNormalPolicy
{
  const Sample & vertices_;
  const Collection< Indices > & boundaryFaces_;
  const UnsignedInteger dimension_;
  const UnsignedInteger baseVertexIndex_;
  const Scalar offset_;
  Sample & boundaryVertices_;
  IndicesCollection & boundarySimplices_;

  ComputeNormalPolicy(const Sample & vertices,
                      const Collection< Indices > & boundaryFaces,
                      const UnsignedInteger baseVertexIndex,
                      const Scalar offset,
                      Sample & boundaryVertices,
                      IndicesCollection & boundarySimplices)
    : vertices_(vertices)
    , boundaryFaces_(boundaryFaces)
    , dimension_(boundaryVertices.getDimension())
    , baseVertexIndex_(baseVertexIndex)
    , offset_(offset)
    , boundaryVertices_(boundaryVertices)
    , boundarySimplices_(boundarySimplices)
  {
    // Nothing to do
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
  {
    Indices face;
    Matrix U;
    Matrix VT;
    SquareMatrix M(dimension_);
    SquareMatrix simplexMatrix(dimension_ + 1);

    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i)
    {
      UnsignedInteger newVertexIndex = baseVertexIndex_ + i;
      face = boundaryFaces_[i];
      const UnsignedInteger remainingVertexIndex = face[dimension_];
      // Build the face matrix & compute the boundary vertex to thicken the face
      // In order to avoid roundoff we first compute the center of the face,
      // then we compute the hyperplane of the face translated to the center
      Point center(dimension_);
      for (UnsignedInteger j = 0; j < dimension_; ++j)
      {
        const UnsignedInteger vertexIndex = face[j];
        for (UnsignedInteger k = 0; k < dimension_; ++k)
          center[k] += boundaryVertices_(vertexIndex, k);
      } // j
      center /= dimension_;
      // Now the centered matrix
      for (UnsignedInteger j = 0; j < dimension_; ++j)
      {
        const UnsignedInteger vertexIndex = face[j];
        for (UnsignedInteger k = 0; k < dimension_; ++k)
          M(j, k) = boundaryVertices_(vertexIndex, k) - center[k];
      } // j
      // Add a new vertex
      try
      {
        // Compute the face normal
        (void) M.computeSVD(U, VT, true, false);
        Point normal(dimension_);
        // The normal is the last row of VT
        for (UnsignedInteger j = 0; j < dimension_; ++j)
          normal[j] = VT(dimension_ - 1, j);
        // Check if the vertex removed from the simplex to form a face is on the same side
        // of the plane as the one pointed by the normal
        if (normal.dot(vertices_[remainingVertexIndex] - center) < 0.0)
          center += normal * offset_;
        else
          center -= normal * offset_;
      }
      catch (const Exception &)
      {
        // The face was in fact degenerated, not a (dimension - 1) entity
        // Nothing to do
      }
      boundaryVertices_[newVertexIndex] = center;
      boundarySimplices_(i, dimension_) = newVertexIndex;
      // Fix the simplex orientation
      for (UnsignedInteger j = 0; j <= dimension_; ++j)
        {
          const UnsignedInteger vertexJ = boundarySimplices_(i, j);
          for (UnsignedInteger k = 0; k < dimension_; ++k) simplexMatrix(k, j) = boundaryVertices_(vertexJ, k);
          simplexMatrix(dimension_, j) = 1.0;
        }
      Scalar sign = 0.0;
      (void) simplexMatrix.computeLogAbsoluteDeterminant(sign, false);
      // In odd dimension the positive orientation is for a negative determinant of
      // the simplex matrix
      if ((sign > 0.0) != (dimension_ % 2 == 1))
        {
          IndicesCollection::iterator cit = boundarySimplices_.begin_at(i);
          std::swap(*cit, *(cit + 1));
        }
    } // i
  } // operator ()
};  // struct ComputeNormalPolicy

} // Anonymous namespace

/* The faces defining the boundary of a mesh of simplices are the faces of
   simplices not shared by two simplices.
   The faces of a simplex are obtained by removing one of its vertices, so in
   dimension d a simplex has (d+1) faces.
   In order to compare two faces, which are made of vertices indices, we sort the
   indices.
   As we want to keep track of the removed vertex in order to know on which side
   of the face is the interior of the domain, we store a face as a (d+1) list of
   integers, the d first ones being the indices of the face vertices, the last
   one is the index of the removed vertex. A consequence is that we must provide
   a dedicated hash function and comparison operator to the std::unordered_map
   The whole algorithm is made of 3 main steps:
   1) Detect the unique faces
   2) Select the corresponding vertices and compact the corresponding indices
   3) (optional) If one ask for a thick boundary, compute a normal vector for
      each face of the boundary and create a new vertex on the correct side of
      the face (inside of the domain if offset<0, outside of the domain if
      offset>0)
*/
Mesh BoundaryMesher::build(const Mesh & mesh,
                                const Scalar offset) const
{
  const UnsignedInteger dimension = mesh.getDimension();
  // I. build the list of unique faces
  const UnsignedInteger simplicesNumber = mesh.getSimplicesNumber();
  IndicesCollection simplices(mesh.getSimplices());
  // a) generate the faces given a simplex: for one simplex we get (dimension+1) faces
  IndicesCollection simplex2Faces(dimension + 1, dimension + 1);
  for (UnsignedInteger i = 0; i <= dimension; ++i)
  {
    for (UnsignedInteger j = 0; j < i; ++j)
      simplex2Faces(i, j) = j;
    for (UnsignedInteger j = i; j < dimension; ++j)
      simplex2Faces(i, j) = j + 1;
    simplex2Faces(i, dimension) = i;
  }
  // b) for all the faces of all the simplices, count how many time they appear
  //    using an unordered map. Here we reserve the maximum possible size in order
  //    to avoid costly memory reallocation and rehash
  std::unordered_map<Indices, UnsignedInteger, FaceHasher, FaceCompare> faces;
  faces.reserve(simplices.getSize() * (dimension + 1));
  Indices face(dimension + 1);
  std::unordered_map<Indices, UnsignedInteger, FaceHasher, FaceCompare>::iterator itFaces;
  for (UnsignedInteger i = 0; i < simplices.getSize(); ++i)
  {
    for (UnsignedInteger j = 0; j < dimension + 1; ++j)
    {
      for (UnsignedInteger k = 0; k < dimension + 1; ++k)
        face[k] = simplices(i, simplex2Faces(j, k));
      // Sort only the indices related to the face vertices, not the last one
      std::sort(face.begin(), face.end() - 1);
      itFaces = faces.find(face);
      if (itFaces == faces.end())
        faces[face] = 1;
      else
        itFaces->second += 1;
    } // j
  } // i
  // c) now we can detect the boundary faces: they have a count equal to 1 in the map
  // We use a collection of indices instead of an IndicesCollection here because the size
  // of the collection is not known in advance
  Collection< Indices > boundaryFaces(simplicesNumber * (dimension + 1), Indices(dimension + 1));
  UnsignedInteger boundaryFaceIndex = 0;
  for (std::unordered_map<Indices, UnsignedInteger, FaceHasher, FaceCompare>::const_iterator it = faces.begin(); it != faces.end(); ++it)
    if (it->second == 1)
    {
      boundaryFaces[boundaryFaceIndex] = it->first;
      ++boundaryFaceIndex;
    } // if
  // Remove the unused space
  boundaryFaces.erase(boundaryFaces.begin() + boundaryFaceIndex, boundaryFaces.end());
  // II. Create the boundary simplices and vertices
  const Sample vertices(mesh.getVertices());
  // a) Preallocate enough space to store the boundary vertices and the offset vertex (if asked for)
  //    Compute the renumbering of the boundary vertices on the fly
  const UnsignedInteger boundaryVerticesReserve = (offset == 0.0 ? 0 : boundaryFaces.getSize());
  Sample boundaryVertices(vertices.getSize() + boundaryVerticesReserve, dimension);
  UnsignedInteger newVertexIndex = 0;
  {
    const UnsignedInteger nbVertices = vertices.getSize();
    Indices oldToNewIndices(nbVertices, nbVertices);
    for (UnsignedInteger i = 0; i < boundaryFaces.getSize(); ++i)
      {
        for (UnsignedInteger j = 0; j < dimension; ++j)
          {
            // Get the old vertex index of the jth vertex of the ith boundary face
            const UnsignedInteger oldVertexIndex = boundaryFaces[i][j];
            // If the vertex has not been seen so far, insert it into the sample of boundary vertices
            if (oldToNewIndices[oldVertexIndex] == nbVertices)
              {
                boundaryFaces[i][j] = newVertexIndex;
                for (UnsignedInteger k = 0; k < dimension; ++k)
                  boundaryVertices(newVertexIndex, k) = vertices(oldVertexIndex, k);
                oldToNewIndices[oldVertexIndex] = newVertexIndex;
                ++newVertexIndex;
              }
            else
              {
                boundaryFaces[i][j] = oldToNewIndices[oldVertexIndex];
              } // if
          } //j
      } // i
  } // In a dedicated scope to allow for the liberation of oldToNewIndices
  // Resize the boundary vertices
  boundaryVertices.erase(boundaryVertices.getImplementation()->begin() + newVertexIndex + boundaryVerticesReserve, boundaryVertices.getImplementation()->end());
  // Now, create the face in the boundary mesh
  IndicesCollection boundarySimplices(boundaryFaces);
  for (UnsignedInteger i = 0; i < boundarySimplices.getSize(); ++i)
    // The last index is repeated in order to indicate that it is a surface mesh
    boundarySimplices(i, dimension) = boundarySimplices(i, dimension - 1);

  // III. If offset is not zero, compute the normal of each face
  //      using a SVD decomposition of the vertices matrix.
  //      Here we use the initial vertices to get the vertex of the simplex
  //      from which the face has been extracted not in the face. It allows us
  //      to make a distinction between the interior and the exterior of the
  //      domain.
  if (offset != 0.0)
  {
    const ComputeNormalPolicy policy(vertices, boundaryFaces, boundaryVertices.getSize() - boundaryVerticesReserve, offset, boundaryVertices, boundarySimplices);
    TBBImplementation::ParallelFor(0, boundaryFaces.getSize(), policy);
  } // offset != 0
  // Return the boundary mesh
  return Mesh(boundaryVertices, boundarySimplices, false);
}

END_NAMESPACE_OPENTURNS
