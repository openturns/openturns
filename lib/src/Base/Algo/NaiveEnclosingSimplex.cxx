//                                               -*- C++ -*-
/**
 *  @brief  This class provides a naive implementation of point
 *  location.  It is only meant for debugging purpose.
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
  : EnclosingSimplexAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameter constructor */
NaiveEnclosingSimplex::NaiveEnclosingSimplex(const Sample & vertices,
    const IndicesCollection & simplices)
  : EnclosingSimplexAlgorithmImplementation()
{
  setVerticesAndSimplices(vertices, simplices);
}

/* Virtual constructor */
NaiveEnclosingSimplex * NaiveEnclosingSimplex::clone() const
{
  return new NaiveEnclosingSimplex(*this);
}

NaiveEnclosingSimplex * NaiveEnclosingSimplex::emptyClone() const
{
  return new NaiveEnclosingSimplex();
}

void NaiveEnclosingSimplex::setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices)
{
  EnclosingSimplexAlgorithmImplementation::setVerticesAndSimplices(vertices, simplices);

  nearestNeighbour_.setSample(vertices_);

  const UnsignedInteger nrSimplices = simplices_.getSize();
  const UnsignedInteger nrVertices = vertices_.getSize();
  Collection<Indices> mapVerticesToSimplices(nrVertices, Indices(0));
  for (UnsignedInteger i = 0; i < nrSimplices; ++i)
  {
    for (IndicesCollection::const_iterator cit = simplices_.cbegin_at(i), guard = simplices_.cend_at(i); cit != guard; ++cit)
      mapVerticesToSimplices[*cit].add(i);
  }
  verticesToSimplices_ = IndicesCollection(mapVerticesToSimplices);
}

/** Nearest neighbour algorithm accessor */
NearestNeighbourAlgorithm NaiveEnclosingSimplex::getNearestNeighbourAlgorithm() const
{
  return nearestNeighbour_;
}

void NaiveEnclosingSimplex::setNearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & nearestNeighbour)
{
  NearestNeighbourAlgorithm emptyClone(nearestNeighbour.getImplementation()->emptyClone());
  nearestNeighbour_.swap(emptyClone);
  nearestNeighbour_.setSample(vertices_);
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger NaiveEnclosingSimplex::query(const Point & point) const
{
  if (point.getDimension() != vertices_.getDimension()) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << vertices_.getDimension() << ", got dimension=" << point.getDimension();
  const UnsignedInteger nrSimplices = simplices_.getSize();

  // First, check against the bounding box
  if (!boundingBox_.contains(point)) return nrSimplices;

  // Second, check the simplices containing the nearest vertex
  const UnsignedInteger nearestIndex = nearestNeighbour_.query(point);
  // Matrix used to store the affine matrix associated with a simplex.
  // It is allocated here to prevent allocation inside loops.
  SquareMatrix simplexMatrix(vertices_.getDimension() + 1);

  for (IndicesCollection::const_iterator cit = verticesToSimplices_.cbegin_at(nearestIndex), guard = verticesToSimplices_.cend_at(nearestIndex); cit != guard; ++cit)
  {
    const UnsignedInteger simplexIndex = *cit;
    if (checkPointInSimplex(point, simplexIndex, simplexMatrix))
    {
      return simplexIndex;
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

/* String converter */
String NaiveEnclosingSimplex::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NaiveEnclosingSimplex::GetClassName()
      << " nearestNeighbourAlgorithm=" << nearestNeighbour_.getImplementation()->getClassName();
  return oss;
}

/* String converter */
String NaiveEnclosingSimplex::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << NaiveEnclosingSimplex::GetClassName()
      << " nearestNeighbourAlgorithm=" << nearestNeighbour_.getImplementation()->getClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void NaiveEnclosingSimplex::save(Advocate & adv) const
{
  EnclosingSimplexAlgorithmImplementation::save(adv);
  adv.saveAttribute("nearestNeighbour_", nearestNeighbour_);
}


/* Method load() reloads the object from the StorageManager */
void NaiveEnclosingSimplex::load(Advocate & adv)
{
  EnclosingSimplexAlgorithmImplementation::load(adv);
  adv.loadAttribute("nearestNeighbour_", nearestNeighbour_);
  setVerticesAndSimplices(vertices_, simplices_);
}

END_NAMESPACE_OPENTURNS
