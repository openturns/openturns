//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is the interface class.
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
#include "openturns/EnclosingSimplex.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/BoundingVolumeHierarchy.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EnclosingSimplex)

/* Constructor with parameters */
EnclosingSimplex::EnclosingSimplex()
  : TypedInterfaceObject<EnclosingSimplexImplementation>(new BoundingVolumeHierarchy())
{
  // Nothing to do
}

/* Constructor with parameters */
EnclosingSimplex::EnclosingSimplex(const Sample & vertices, const IndicesCollection & simplices)
  : TypedInterfaceObject<EnclosingSimplexImplementation>(new BoundingVolumeHierarchy(vertices, simplices))
{
  // Nothing to do
}

/* Constructor with parameters */
EnclosingSimplex::EnclosingSimplex(const Sample & vertices,
                                   const Indices & flatSimplexIndices,
                                   const Indices & offsetSimplexIndices)
  : TypedInterfaceObject<EnclosingSimplexImplementation>(new BoundingVolumeHierarchy(vertices, flatSimplexIndices, offsetSimplexIndices))
{
  // Nothing to do
}

/* Parameters constructor */
EnclosingSimplex::EnclosingSimplex(const EnclosingSimplexImplementation & implementation)
  : TypedInterfaceObject<EnclosingSimplexImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
EnclosingSimplex::EnclosingSimplex(const Implementation & p_implementation)
  : TypedInterfaceObject<EnclosingSimplexImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
EnclosingSimplex::EnclosingSimplex(EnclosingSimplexImplementation * p_implementation)
  : TypedInterfaceObject<EnclosingSimplexImplementation>(p_implementation)
{
  // Nothing to do
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger EnclosingSimplex::getEnclosingSimplexIndex(const Point & x) const
{
  return getImplementation()->getEnclosingSimplexIndex(x);
}

/* Get the index of the enclosing simplex of the given points */
Indices EnclosingSimplex::getEnclosingSimplexIndex(const Sample & sample) const
{
  return getImplementation()->getEnclosingSimplexIndex(sample);
}

/* String converter */
String EnclosingSimplex::__repr__() const
{
  OSS oss(true);
  oss << "class=" << EnclosingSimplex::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String EnclosingSimplex::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << EnclosingSimplex::GetClassName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
