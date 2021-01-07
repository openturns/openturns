//                                               -*- C++ -*-
/**
 *  @brief  This class provides a point location algorithm on monotonic 1D meshes
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/EnclosingSimplexMonotonic1D.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class EnclosingSimplexMonotonic1D
 */

CLASSNAMEINIT(EnclosingSimplexMonotonic1D)

static const Factory<EnclosingSimplexMonotonic1D> Factory_EnclosingSimplexMonotonic1D;

/* Constructor without parameters */
EnclosingSimplexMonotonic1D::EnclosingSimplexMonotonic1D()
  : EnclosingSimplexAlgorithmImplementation()
  , increasing_(true)
{
  // Nothing to do
}

/* Parameter constructor */
EnclosingSimplexMonotonic1D::EnclosingSimplexMonotonic1D(const Sample & vertices)
  : EnclosingSimplexAlgorithmImplementation()
  , increasing_(true)
{
  IndicesCollection simplices;
  setVerticesAndSimplices(vertices, simplices);
}

/* Virtual constructor */
EnclosingSimplexMonotonic1D * EnclosingSimplexMonotonic1D::clone() const
{
  return new EnclosingSimplexMonotonic1D(*this);
}

EnclosingSimplexMonotonic1D * EnclosingSimplexMonotonic1D::emptyClone() const
{
  return new EnclosingSimplexMonotonic1D();
}

void EnclosingSimplexMonotonic1D::setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices)
{
  EnclosingSimplexAlgorithmImplementation::setVerticesAndSimplices(vertices, simplices);
  const UnsignedInteger nrVertices = vertices_.getSize();
  if (!(nrVertices >= 2))
    throw InvalidArgumentException(HERE) << "Invalid mesh, it must contain at least 2 vertices";

  // Check that simplices are either not set, or correspond to implicit numbering
  const UnsignedInteger nrSimplices = simplices_.getSize();
  if (nrSimplices > 0)
  {
    for (UnsignedInteger i = 0; i < nrSimplices; ++i)
    {
      if (!(simplices_(i, 0) == i  && simplices_(i, 1) ==  i + 1))
        throw InvalidArgumentException(HERE) << "Invalid simplex " << i << ", it contains (" << simplices_(i, 0) << ", " << simplices_(i, 1) << ") instead of (" << i << ", " << (i + 1) << ")";
    }
  }
  else
  {
    simplices_ = IndicesCollection(vertices_.getSize() - 1, 2);
    for(UnsignedInteger i = 0; i < nrVertices - 1; ++i)
    {
      simplices_(i, 0) = i;
      simplices_(i, 1) = i + 1;
    }
  }
  // Check that vertices are monotonic
  Scalar delta = vertices_(1, 0) - vertices_(0, 0);
  for (UnsignedInteger i = 1; i < nrVertices - 1; ++i)
  {
    Scalar newDelta = vertices_(i +  1, 0) - vertices_(i, 0);
    if (delta == 0.0)
      delta = newDelta;
    else if (!(delta *  newDelta >= 0.0))
      throw InvalidArgumentException(HERE) << "Vertices are not monotonic";
  }
  increasing_ =  delta > 0.0;
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger EnclosingSimplexMonotonic1D::query(const Point & point) const
{
  if (!(point.getDimension() == 1)) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=1, got dimension=" << point.getDimension();
  return queryScalar(point[0]);
}

/* Get the simplex index such that
      vertices_(simplices_(index,0),0) <= x <= vertices_(simplices_(index,1),0)
   or vertices_(simplices_(index,1),0) <= x <= vertices_(simplices_(index,0),0)  */
UnsignedInteger EnclosingSimplexMonotonic1D::queryScalar(const Scalar x) const
{
  const UnsignedInteger size = vertices_.getSize();
  const UnsignedInteger notFound = simplices_.getSize();
  if (increasing_ && (x < vertices_(0, 0) || x > vertices_(size - 1, 0)))
    return notFound;
  if (!increasing_ && (x < vertices_(size - 1, 0) || x > vertices_(0, 0)))
    return notFound;
  UnsignedInteger iMin = 0;
  UnsignedInteger iMax = size - 1;
  while (iMax > iMin + 1)
  {
    const UnsignedInteger iMiddle = (iMax + iMin) / 2;
    const Scalar value = vertices_(iMiddle, 0);
    if (value == x)
    {
      // Value found
      return iMiddle;
    }
    if (increasing_ == (value < x))
    {
      iMin = iMiddle;
    }
    else
    {
      iMax = iMiddle;
    }
  }
  return iMin;
}


Indices EnclosingSimplexMonotonic1D::queryScalar(const Point & values) const
{
  Indices result(values.getSize());
  for(UnsignedInteger i = 0; i < values.getSize(); ++i)
    result[i] = queryScalar(values[i]);
  return result;
}

/* String converter */
String EnclosingSimplexMonotonic1D::__repr__() const
{
  return OSS(true) << "class=" << EnclosingSimplexMonotonic1D::GetClassName();
}

/* String converter */
String EnclosingSimplexMonotonic1D::__str__(const String & ) const
{
  return OSS(false) << "class=" << EnclosingSimplexMonotonic1D::GetClassName();
}

END_NAMESPACE_OPENTURNS
