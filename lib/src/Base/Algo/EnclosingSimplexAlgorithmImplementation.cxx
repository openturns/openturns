//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is an abstract class.
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
#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplexAlgorithmImplementation
 */

CLASSNAMEINIT(EnclosingSimplexAlgorithmImplementation)

static const Factory<EnclosingSimplexAlgorithmImplementation> Factory_EnclosingSimplexAlgorithmImplementation;

/* Constructor without parameters */
EnclosingSimplexAlgorithmImplementation::EnclosingSimplexAlgorithmImplementation()
  : PersistentObject()
  , vertices_()
  , simplices_()
  , lowerBoundingBoxSimplices_()
  , upperBoundingBoxSimplices_()
{
  // Nothing to do
}

/* Parameter constructor */
EnclosingSimplexAlgorithmImplementation::EnclosingSimplexAlgorithmImplementation(const Sample & vertices, const IndicesCollection & simplices)
  : PersistentObject()
  , vertices_(vertices)
  , simplices_()
  , lowerBoundingBoxSimplices_()
  , upperBoundingBoxSimplices_()
{
  setVerticesAndSimplices(vertices, simplices);
}

/* Virtual constructor */
EnclosingSimplexAlgorithmImplementation * EnclosingSimplexAlgorithmImplementation::clone() const
{
  return new EnclosingSimplexAlgorithmImplementation(*this);
}

EnclosingSimplexAlgorithmImplementation * EnclosingSimplexAlgorithmImplementation::emptyClone() const
{
  return new EnclosingSimplexAlgorithmImplementation();
}

/* Points accessor */
Sample EnclosingSimplexAlgorithmImplementation::getVertices() const
{
  return vertices_;
}

/* Simplices accessor */
IndicesCollection EnclosingSimplexAlgorithmImplementation::getSimplices() const
{
  return simplices_;
}

void EnclosingSimplexAlgorithmImplementation::setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices)
{
  if (vertices == vertices_ && simplices == simplices_) return;

  vertices_ = vertices;
  simplices_ = simplices;

  // Global bounding box
  boundingBox_ = Interval(vertices_.getMin(), vertices_.getMax());
  // Local bounding box of each simplex
  const UnsignedInteger dimension = vertices_.getDimension();
  const UnsignedInteger nrSimplices = simplices_.getSize();
  lowerBoundingBoxSimplices_ = Sample(nrSimplices, dimension);
  upperBoundingBoxSimplices_ = Sample(nrSimplices, dimension);
  Point lower(dimension, SpecFunc::MaxScalar);
  Point upper(dimension, - SpecFunc::MaxScalar);
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    for(UnsignedInteger k = 0; k < dimension; ++k)
    {
      lower[k] = SpecFunc::MaxScalar;
      upper[k] = - SpecFunc::MaxScalar;
    }
    for (IndicesCollection::const_iterator cit = simplices_.cbegin_at(i), guard = simplices_.cend_at(i); cit != guard; ++cit)
    {
      for(UnsignedInteger k = 0; k < dimension; ++k)
      {
        const Scalar coordinate = vertices_(*cit, k);
        lower[k] = std::min(lower[k], coordinate);
        upper[k] = std::max(upper[k], coordinate);
      }
    }
    for(UnsignedInteger k = 0; k < dimension; ++k)
    {
      lowerBoundingBoxSimplices_(i, k) = lower[k];
      upperBoundingBoxSimplices_(i, k) = upper[k];
    }
  }
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger EnclosingSimplexAlgorithmImplementation::query(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In EnclosingSimplexAlgorithmImplementation::query(const Point & x) const";
}

/* TBB policy to speed-up query over a sample */
struct EnclosingSimplexAlgorithmImplementationQueryPolicy
{
  const Sample & points_;
  Indices & indices_;
  const EnclosingSimplexAlgorithmImplementation & algorithm_;

  EnclosingSimplexAlgorithmImplementationQueryPolicy(const Sample & points,
      Indices & indices,
      const EnclosingSimplexAlgorithmImplementation & algorithm)
    : points_(points)
    , indices_(indices)
    , algorithm_(algorithm)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) indices_[i] = algorithm_.query(points_[i]);
  }

}; /* end struct EnclosingSimplexAlgorithmImplementationQueryPolicy */

/* Get the index of the enclosing simplex of the given points */
Indices EnclosingSimplexAlgorithmImplementation::query(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Indices result(size);
  if (size == 0) return result;
  const EnclosingSimplexAlgorithmImplementationQueryPolicy policy( sample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

namespace
{
inline Bool EnclosingSimplexAlgorithm_checkPointInSimplex1D(const Scalar v1, const Scalar v2, const Scalar pt)
{
  return (pt - v1) * (pt - v2) <= SpecFunc::ScalarEpsilon;
}

inline Bool EnclosingSimplexAlgorithm_checkPointInSimplex2D(const Scalar * const v1, const Scalar * const v2, const Scalar * const v3, const Scalar * const pt)
{
  const Scalar x = *pt;
  const Scalar y = *(pt + 1);
  const Scalar v1x = *v1;
  const Scalar v1y = *(v1 + 1);
  const Scalar v2x = *v2;
  const Scalar v2y = *(v2 + 1);
  const Scalar v3x = *v3;
  const Scalar v3y = *(v3 + 1);
  const Scalar totalVolume = (v3x - v2x) * (v1y - v2y) - (v1x - v2x) * (v3y - v2y);
  const Scalar volume1 = (x - v2x) * (v1y - v2y) - (v1x - v2x) * (y - v2y);
  const Scalar volume2 = (x - v3x) * (v2y - v3y) - (v2x - v3x) * (y - v3y);
  const Scalar volume3 = (x - v1x) * (v3y - v1y) - (v3x - v1x) * (y - v1y);
  if (totalVolume > SpecFunc::ScalarEpsilon)
  {
    return volume1 > - SpecFunc::ScalarEpsilon && volume2 > - SpecFunc::ScalarEpsilon && volume3 > - SpecFunc::ScalarEpsilon;
  }
  else if (totalVolume < - SpecFunc::ScalarEpsilon)
  {
    return volume1 < SpecFunc::ScalarEpsilon && volume2 < SpecFunc::ScalarEpsilon && volume3 < SpecFunc::ScalarEpsilon;
  }
  else
  {
    // Degenerate case
    if (volume1 > SpecFunc::ScalarEpsilon || volume1 < - SpecFunc::ScalarEpsilon ||
        volume2 > SpecFunc::ScalarEpsilon || volume2 < - SpecFunc::ScalarEpsilon ||
        volume3 > SpecFunc::ScalarEpsilon || volume3 < - SpecFunc::ScalarEpsilon)
      return false;
    // Point is on the same line
    return x <= std::max(std::max(v1x, v2x), v3x) && x >= std::min(std::min(v1x, v2x), v3x) &&
           y <= std::max(std::max(v1y, v2y), v3y) && y >= std::min(std::min(v1y, v2y), v3y);
  }
} // EnclosingSimplexAlgorithm_checkPointInSimplex2D

inline Bool EnclosingSimplexAlgorithm_checkPointInSimplex3D(const Scalar * const v1, const Scalar * const v2, const Scalar * const v3, const Scalar * const v4, const Scalar * const pt)
{
  const Scalar x = *pt;
  const Scalar y = *(pt + 1);
  const Scalar z = *(pt + 2);
  const Scalar v1x = *v1;
  const Scalar v1y = *(v1 + 1);
  const Scalar v1z = *(v1 + 2);
  const Scalar v2x = *v2;
  const Scalar v2y = *(v2 + 1);
  const Scalar v2z = *(v2 + 2);
  const Scalar v3x = *v3;
  const Scalar v3y = *(v3 + 1);
  const Scalar v3z = *(v3 + 2);
  const Scalar v4x = *v4;
  const Scalar v4y = *(v4 + 1);
  const Scalar v4z = *(v4 + 2);
  const Scalar totalVolume = (v1x - v4x) * ((v2y - v4y) * (v3z - v4z) - (v3y - v4y) * (v2z - v4z)) - (v2x - v4x) * ((v1y - v4y) * (v3z - v4z) - (v3y - v4y) * (v1z - v4z)) + (v3x - v4x) * ((v1y - v4y) * (v2z - v4z) - (v2y - v4y) * (v1z - v4z));
  const Scalar volume1 = (x - v4x) * ((v2y - v4y) * (v3z - v4z) - (v3y - v4y) * (v2z - v4z)) - (v2x - v4x) * ((y - v4y) * (v3z - v4z) - (v3y - v4y) * (z - v4z)) + (v3x - v4x) * ((y - v4y) * (v2z - v4z) - (v2y - v4y) * (z - v4z));
  const Scalar volume2 = (v1x - v4x) * ((y - v4y) * (v3z - v4z) - (v3y - v4y) * (z - v4z)) - (x - v4x) * ((v1y - v4y) * (v3z - v4z) - (v3y - v4y) * (v1z - v4z)) + (v3x - v4x) * ((v1y - v4y) * (z - v4z) - (y - v4y) * (v1z - v4z));
  const Scalar volume3 = (v1x - v4x) * ((v2y - v4y) * (z - v4z) - (y - v4y) * (v2z - v4z)) - (v2x - v4x) * ((v1y - v4y) * (z - v4z) - (y - v4y) * (v1z - v4z)) + (x - v4x) * ((v1y - v4y) * (v2z - v4z) - (v2y - v4y) * (v1z - v4z));
  const Scalar volume4 = (v1x - x) * ((v2y - y) * (v3z - z) - (v3y - y) * (v2z - z)) - (v2x - x) * ((v1y - y) * (v3z - z) - (v3y - y) * (v1z - z)) + (v3x - x) * ((v1y - y) * (v2z - z) - (v2y - y) * (v1z - z));
  if (totalVolume > SpecFunc::ScalarEpsilon)
  {
    return volume1 > - SpecFunc::ScalarEpsilon && volume2 > - SpecFunc::ScalarEpsilon && volume3 > - SpecFunc::ScalarEpsilon && volume4 > - SpecFunc::ScalarEpsilon;
  }
  else if (totalVolume < - SpecFunc::ScalarEpsilon)
  {
    return volume1 < SpecFunc::ScalarEpsilon && volume2 < SpecFunc::ScalarEpsilon && volume3 < SpecFunc::ScalarEpsilon && volume4 < SpecFunc::ScalarEpsilon;
  }
  else
  {
    // Degenerate case
    if (volume1 > SpecFunc::ScalarEpsilon || volume1 < - SpecFunc::ScalarEpsilon ||
        volume2 > SpecFunc::ScalarEpsilon || volume2 < - SpecFunc::ScalarEpsilon ||
        volume3 > SpecFunc::ScalarEpsilon || volume3 < - SpecFunc::ScalarEpsilon ||
        volume4 > SpecFunc::ScalarEpsilon || volume4 < - SpecFunc::ScalarEpsilon)
      return false;
    // Point is on the same line
    return x <= std::max(std::max(std::max(v1x, v2x), v3x), v4x) && x >= std::min(std::min(std::min(v1x, v2x), v3x), v4x) &&
           y <= std::max(std::max(std::max(v1y, v2y), v3y), v4y) && y >= std::min(std::min(std::min(v1y, v2y), v3y), v4y) &&
           z <= std::max(std::max(std::max(v1z, v2z), v3z), v4z) && z >= std::min(std::min(std::min(v1z, v2z), v3z), v4z);
  }
} // EnclosingSimplexAlgorithm_checkPointInSimplex3D

} // namespace

/* Check if the given point is in the given simplex */
Bool EnclosingSimplexAlgorithmImplementation::checkPointInSimplex(const Point & point, const UnsignedInteger index, SquareMatrix & simplexMatrix) const
{
  const UnsignedInteger dimension = vertices_.getDimension();
  // Special case for dimension==1
  // It is more efficient to skip the tests against both the global bounding box
  // and the triangle bounding box
  if (dimension == 1)
  {
    IndicesCollection::const_iterator cit1d = simplices_.cbegin_at(index);
    return EnclosingSimplexAlgorithm_checkPointInSimplex1D(vertices_(*cit1d, 0), vertices_(*(cit1d + 1), 0), point[0]);
  }
  // Special case for dimension==2
  // It is more efficient to skip the tests against both the global bounding box
  // and the triangle bounding box
  else if (dimension == 2)
  {
    IndicesCollection::const_iterator cit2d = simplices_.cbegin_at(index);
    return EnclosingSimplexAlgorithm_checkPointInSimplex2D(&vertices_(*cit2d, 0), &vertices_(*(cit2d + 1), 0), &vertices_(*(cit2d + 2), 0), &point[0]);
  }
  // Special case for dimension==3
  // It is more efficient to skip the tests against both the global bounding box
  // and the triangle bounding box
  else if (dimension == 3)
  {
    IndicesCollection::const_iterator cit3d = simplices_.cbegin_at(index);
    return EnclosingSimplexAlgorithm_checkPointInSimplex3D(&vertices_(*cit3d, 0), &vertices_(*(cit3d + 1), 0), &vertices_(*(cit3d + 2), 0), &vertices_(*(cit3d + 3), 0), &point[0]);
  }

  // Exit if point is outside global bounding box
  if (!boundingBox_.contains(point)) return false;

  // Exit if point is outside simplex bounding box
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if(point[i] < lowerBoundingBoxSimplices_(index, i))
      return false;
    if(point[i] > upperBoundingBoxSimplices_(index, i))
      return false;
  }

  // Build the affine matrix associated with this simplex
  IndicesCollection::const_iterator cit = simplices_.cbegin_at(index);
  for (UnsignedInteger j = 0; j <= dimension; ++j, ++cit)
  {
    for (UnsignedInteger i = 0; i < dimension; ++i)
      simplexMatrix(i, j) = vertices_(*cit, i);
    simplexMatrix(dimension, j) = 1.0;
  }
  Point v(dimension + 1, 1.0);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    v[i] = point[i];
  const Point coordinates(simplexMatrix.solveLinearSystem(v, false));
  for (UnsignedInteger i = 0; i <= dimension; ++i)
    if ((coordinates[i] < 0.0) || (coordinates[i] > 1.0))
      return false;
  return true;
}

/* String converter */
String EnclosingSimplexAlgorithmImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << EnclosingSimplexAlgorithmImplementation::GetClassName();
  return oss;
}

/* String converter */
String EnclosingSimplexAlgorithmImplementation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << EnclosingSimplexAlgorithmImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void EnclosingSimplexAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("vertices_", vertices_);
  adv.saveAttribute("simplices_", simplices_);
}


/* Method load() reloads the object from the StorageManager */
void EnclosingSimplexAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  Sample vertices;
  adv.loadAttribute("vertices_", vertices);
  IndicesCollection simplices;
  adv.loadAttribute("simplices_", simplices);
  setVerticesAndSimplices(vertices, simplices);
}

END_NAMESPACE_OPENTURNS
