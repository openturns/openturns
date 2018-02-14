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
#include "openturns/EnclosingSimplexImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplexImplementation
 */

CLASSNAMEINIT(EnclosingSimplexImplementation)

static const Factory<EnclosingSimplexImplementation> Factory_EnclosingSimplexImplementation;

/* Constructor without parameters */
EnclosingSimplexImplementation::EnclosingSimplexImplementation()
  : PersistentObject()
  , lowerBoundingBoxSimplices_()
  , upperBoundingBoxSimplices_()
{
  // Nothing to do
}

/* Parameter constructor */
EnclosingSimplexImplementation::EnclosingSimplexImplementation(const Sample & vertices, const IndicesCollection & simplices)
  : PersistentObject()
  , vertices_(vertices)
  , lowerBoundingBoxSimplices_()
  , upperBoundingBoxSimplices_()
{
  const UnsignedInteger nrSimplices = simplices.getSize();
  offsetSimplexIndices_ = Indices(nrSimplices + 1);
  flatSimplexIndices_ = Indices(0);
  offsetSimplexIndices_[0] = 0;
  for(UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    const Indices simplex = simplices[i];
    const UnsignedInteger simplexSize = simplex.getSize();
    offsetSimplexIndices_[i + 1] = offsetSimplexIndices_[i] + simplexSize;
    for(UnsignedInteger j = 0; j < simplexSize; ++j)
      flatSimplexIndices_.add(simplex[j]);
  }
  initialize();
}

/* Parameter constructor */
EnclosingSimplexImplementation::EnclosingSimplexImplementation(const Sample & vertices,
                                                               const Indices & flatSimplexIndices,
                                                               const Indices & offsetSimplexIndices)
  : PersistentObject()
  , vertices_(vertices)
  , flatSimplexIndices_(flatSimplexIndices)
  , offsetSimplexIndices_(offsetSimplexIndices)
  , lowerBoundingBoxSimplices_()
  , upperBoundingBoxSimplices_()
{
  initialize();
}

/* Virtual constructor */
EnclosingSimplexImplementation * EnclosingSimplexImplementation::clone() const
{
  return new EnclosingSimplexImplementation(*this);
}

void EnclosingSimplexImplementation::initialize()
{
  // Global bounding box
  boundingBox_ = Interval(vertices_.getMin(), vertices_.getMax());
  // Local bounding box of each simplex
  const UnsignedInteger dimension = vertices_.getDimension();
  const UnsignedInteger nrSimplices = offsetSimplexIndices_.getSize() - 1;
  lowerBoundingBoxSimplices_ = Sample(nrSimplices, Point(dimension, SpecFunc::MaxScalar));
  upperBoundingBoxSimplices_ = Sample(nrSimplices, Point(dimension, - SpecFunc::MaxScalar));
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    for (UnsignedInteger j = offsetSimplexIndices_[i]; j < offsetSimplexIndices_[i + 1]; ++j)
    {
      const UnsignedInteger index = flatSimplexIndices_[j];
      for(UnsignedInteger k = 0; k < dimension; ++k)
      {
        if (vertices_(index, k) < lowerBoundingBoxSimplices_(i, k))
          lowerBoundingBoxSimplices_(i, k) = vertices_(index, k);
        if (vertices_(index, k) > upperBoundingBoxSimplices_(i, k))
          upperBoundingBoxSimplices_(i, k) = vertices_(index, k);
      }
    }
  }
}

/* Points accessor */
Sample EnclosingSimplexImplementation::getVertices() const
{
  return vertices_;
}

/* Simplices accessor */
EnclosingSimplexImplementation::IndicesCollection EnclosingSimplexImplementation::getSimplices() const
{
  const UnsignedInteger nrSimplices = offsetSimplexIndices_.getSize() - 1;
  Collection<Indices> simplices;
  UnsignedInteger offset = 0;
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    const UnsignedInteger simplexSize = offsetSimplexIndices_[i + 1] - offsetSimplexIndices_[i];
    Indices simplex(simplexSize);
    for(UnsignedInteger j = 0; j < simplexSize; ++j)
    {
      simplex[j] = flatSimplexIndices_[offsetSimplexIndices_[i] + j];
    }
    simplices.add(simplex);
  }
  return simplices;
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger EnclosingSimplexImplementation::getEnclosingSimplexIndex(const Point & x) const
{
  throw NotYetImplementedException(HERE) << "In EnclosingSimplexImplementation::getEnclosingSimplexIndex(const Point & x) const";
}

/* Get the index of the enclosing simplex of the given points */
Indices EnclosingSimplexImplementation::getEnclosingSimplexIndex(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Indices result(size);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    result[i] = getEnclosingSimplexIndex(sample[i]);
  }
  return result;
}

/* Check if the given point is in the given simplex */
Bool EnclosingSimplexImplementation::checkPointInSimplex(const Point & point, const UnsignedInteger index, SquareMatrix & simplexMatrix) const
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
  for (UnsignedInteger j = 0; j <= dimension; ++j)
  {
    const Point vertexJ(vertices_[flatSimplexIndices_[offsetSimplexIndices_[index] + j]]);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      simplexMatrix(i, j) = vertexJ[i];
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
String EnclosingSimplexImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << EnclosingSimplexImplementation::GetClassName();
  return oss;
}

/* String converter */
String EnclosingSimplexImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << EnclosingSimplexImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void EnclosingSimplexImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("vertices_", vertices_);
  adv.saveAttribute("offsetSimplexIndices_", offsetSimplexIndices_);
  adv.saveAttribute("flatSimplexIndices_", flatSimplexIndices_);
}


/* Method load() reloads the object from the StorageManager */
void EnclosingSimplexImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("vertices_", vertices_);
  adv.loadAttribute("offsetSimplexIndices_", offsetSimplexIndices_);
  adv.loadAttribute("flatSimplexIndices_", flatSimplexIndices_);
  initialize();
}

END_NAMESPACE_OPENTURNS
