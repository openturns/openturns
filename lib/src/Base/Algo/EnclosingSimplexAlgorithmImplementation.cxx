//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is an abstract class.
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
#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

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
UnsignedInteger EnclosingSimplexAlgorithmImplementation::query(const Point & x) const
{
  throw NotYetImplementedException(HERE) << "In EnclosingSimplexAlgorithmImplementation::query(const Point & x) const";
}

/* Get the index of the enclosing simplex of the given points */
Indices EnclosingSimplexAlgorithmImplementation::query(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Indices result(size);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    result[i] = query(sample[i]);
  }
  return result;
}

/* Check if the given point is in the given simplex */
Bool EnclosingSimplexAlgorithmImplementation::checkPointInSimplex(const Point & point, const UnsignedInteger index, SquareMatrix & simplexMatrix) const
{
  // Exit if point is outside global bounding box
  if (!boundingBox_.contains(point)) return false;

  const UnsignedInteger dimension = vertices_.getDimension();
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
String EnclosingSimplexAlgorithmImplementation::__str__(const String & offset) const
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
