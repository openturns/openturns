//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast search of neighbours.
 *  This is an abstract class
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
#include "openturns/NearestNeighbourImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NearestNeighbourImplementation
 */

CLASSNAMEINIT(NearestNeighbourImplementation)

static const Factory<NearestNeighbourImplementation> Factory_NearestNeighbourImplementation;

/* Constructor without parameters */
NearestNeighbourImplementation::NearestNeighbourImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
NearestNeighbourImplementation * NearestNeighbourImplementation::clone() const
{
  return new NearestNeighbourImplementation(*this);
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger NearestNeighbourImplementation::getNearestNeighbourIndex(const Point & x) const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourImplementation::getNearestNeighbourIndex(const Point & x) const";
}

/* Get the index of the nearest neighbour of the given points */
Indices NearestNeighbourImplementation::getNearestNeighbourIndex(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Indices result(size);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    result[i] = getNearestNeighbourIndex(sample[i]);
  }
  return result;
}

/* Get the nearest neighbour of the given point */
Point NearestNeighbourImplementation::getNearestNeighbour(const Point & x) const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourImplementation::getNearestNeighbour(const Point & x) const";
}

/* Get the nearest neighbour of the given points */
Sample NearestNeighbourImplementation::getNearestNeighbour(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Sample result(0, sample.getDimension());
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    result.add(getNearestNeighbour(sample[i]));
  }
  return result;
}

/* Get the indices of the k nearest neighbours of the given point */
Indices NearestNeighbourImplementation::getNearestNeighboursIndices(const Point & x,
                                                                    const UnsignedInteger k,
                                                                    const Bool sorted) const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourImplementation::getNearestNeighboursIndices(const Point & x, const UnsignedInteger k, const Bool sorted) const";
}

/* Get the k nearest neighbours of the given point */
Sample NearestNeighbourImplementation::getNearestNeighbours(const Point & x,
                                                            const UnsignedInteger k,
                                                            const Bool sorted) const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourImplementation::getNearestNeighbours(const Point & x, const UnsignedInteger k, const Bool sorted) const";
}

/* String converter */
String NearestNeighbourImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NearestNeighbourImplementation::GetClassName();
  return oss;
}

/* String converter */
String NearestNeighbourImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << NearestNeighbourImplementation::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
