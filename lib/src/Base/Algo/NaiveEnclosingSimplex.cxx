//                                               -*- C++ -*-
/**
 *  @brief  This class provides a naive implementation of point
 *  location.  It is only meant for debugging purpose.
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
#include "openturns/NaiveEnclosingSimplex.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/KDTree.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NaiveEnclosingSimplex
 */

CLASSNAMEINIT(NaiveEnclosingSimplex)

static const Factory<NaiveEnclosingSimplex> Factory_NaiveEnclosingSimplex;

/* Constructor without parameters */
NaiveEnclosingSimplex::NaiveEnclosingSimplex()
  : EnclosingSimplexImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
NaiveEnclosingSimplex::NaiveEnclosingSimplex(const Sample & vertices,
                                             const IndicesCollection & simplices)
  : EnclosingSimplexImplementation(vertices, simplices)
{
  initialize();
}

/* Virtual constructor */
NaiveEnclosingSimplex * NaiveEnclosingSimplex::clone() const
{
  return new NaiveEnclosingSimplex(*this);
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger NaiveEnclosingSimplex::getEnclosingSimplexIndex(const Point & point) const
{
  if (point.getDimension() != vertices_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << vertices_.getDimension() << ", got dimension=" << point.getDimension();
  const UnsignedInteger nrSimplices = offsetSimplexIndices_.getSize() - 1;

  // First, check against the bounding box
  if (!boundingBox_.contains(point)) return nrSimplices;

  // Second, check the simplices containing the nearest vertex
  const UnsignedInteger nearestIndex = nearestNeighbour_.getNearestNeighbourIndex(point);
  const Indices simplicesCandidates(verticesToSimplices_[nearestIndex]);
  // Matrix used to store the affine matrix associated with a simplex.
  // It is allocated here to prevent allocation inside loops.
  SquareMatrix simplexMatrix(vertices_.getDimension() + 1);
  for (UnsignedInteger i = 0; i < simplicesCandidates.getSize(); ++i)
  {
    if (checkPointInSimplex(point, simplicesCandidates[i], simplexMatrix))
    {
      return simplicesCandidates[i];
    }
  }

  // Third, a full loop to deal with points not inside of a simplex associated to the nearest vertex
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    if (checkPointInSimplex(point, i, simplexMatrix))
    {
      return i;
    }
  }
  return nrSimplices;
}

void NaiveEnclosingSimplex::initialize()
{
  nearestNeighbour_ = KDTree(vertices_);

  const UnsignedInteger numSimplices = offsetSimplexIndices_.getSize() - 1;
  const UnsignedInteger numVertices = vertices_.getSize();
  const UnsignedInteger dimension = vertices_.getDimension();
  verticesToSimplices_ = IndicesCollection(numVertices, Indices(0));
  lowerBoundingBoxSimplices_ = Sample(numSimplices, Point(dimension, SpecFunc::MaxScalar));
  upperBoundingBoxSimplices_ = Sample(numSimplices, Point(dimension, - SpecFunc::MaxScalar));
  for (UnsignedInteger i = 0; i < numSimplices; ++i)
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
      verticesToSimplices_[index].add(i);
    }
  } // Loop over simplices
}

/* Method load() reloads the object from the StorageManager */
void NaiveEnclosingSimplex::load(Advocate & adv)
{
  EnclosingSimplexImplementation::load(adv);
  initialize();
}

END_NAMESPACE_OPENTURNS
