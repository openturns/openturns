//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast search of neighbours.
 *  This is the interface class
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
#include "openturns/NearestNeighbour.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KDTree.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NearestNeighbour)

/* Constructor with parameters */
NearestNeighbour::NearestNeighbour()
  : TypedInterfaceObject<NearestNeighbourImplementation>(new KDTree())
{
  // Nothing to do
}

/* Constructor with parameters */
NearestNeighbour::NearestNeighbour(const Sample & sample)
  : TypedInterfaceObject<NearestNeighbourImplementation>(new KDTree(sample))
{
  // Nothing to do
}

/* Parameters constructor */
NearestNeighbour::NearestNeighbour(const NearestNeighbourImplementation & implementation)
  : TypedInterfaceObject<NearestNeighbourImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
NearestNeighbour::NearestNeighbour(const Implementation & p_implementation)
  : TypedInterfaceObject<NearestNeighbourImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
NearestNeighbour::NearestNeighbour(NearestNeighbourImplementation * p_implementation)
  : TypedInterfaceObject<NearestNeighbourImplementation>(p_implementation)
{
  // Nothing to do
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger NearestNeighbour::getNearestNeighbourIndex(const Point & x) const
{
  return getImplementation()->getNearestNeighbourIndex(x);
}

/* Get the index of the nearest neighbour of the given points */
Indices NearestNeighbour::getNearestNeighbourIndex(const Sample & sample) const
{
  return getImplementation()->getNearestNeighbourIndex(sample);
}

/* Get the nearest neighbour of the given point */
Point NearestNeighbour::getNearestNeighbour(const Point & x) const
{
  return getImplementation()->getNearestNeighbour(x);
}

/* Get the nearest neighbour of the given points */
Sample NearestNeighbour::getNearestNeighbour(const Sample & sample) const
{
  return getImplementation()->getNearestNeighbour(sample);
}

/* Get the indices of the k nearest neighbours of the given point */
Indices NearestNeighbour::getNearestNeighboursIndices(const Point & x,
                                                      const UnsignedInteger k,
                                                      const Bool sorted) const
{
  return getImplementation()->getNearestNeighboursIndices(x, k, sorted);
}

/* Get the k nearest neighbours of the given point */
Sample NearestNeighbour::getNearestNeighbours(const Point & x,
                                              const UnsignedInteger k,
                                              const Bool sorted) const
{
  return getImplementation()->getNearestNeighbours(x, k, sorted);
}

/* String converter */
String NearestNeighbour::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NearestNeighbour::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String NearestNeighbour::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << NearestNeighbour::GetClassName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
