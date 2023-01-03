//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast point location.
 *  This is the interface class.
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
#include "openturns/EnclosingSimplexAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/BoundingVolumeHierarchy.hxx"
#include "openturns/RegularGridEnclosingSimplex.hxx"
#include "openturns/EnclosingSimplexMonotonic1D.hxx"
#include "openturns/NaiveEnclosingSimplex.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(EnclosingSimplexAlgorithm)

/* Constructor with parameters */
EnclosingSimplexAlgorithm::EnclosingSimplexAlgorithm()
  : TypedInterfaceObject<EnclosingSimplexAlgorithmImplementation>(new BoundingVolumeHierarchy())
{
  // Nothing to do
}

/* Constructor with parameters */
EnclosingSimplexAlgorithm::EnclosingSimplexAlgorithm(const Sample & vertices, const IndicesCollection & simplices)
  : TypedInterfaceObject<EnclosingSimplexAlgorithmImplementation>()
{
  if (vertices.getDimension() == 1)
  {
    // Check that simplices correspond to standard 1D simplices (i,i+1)
    const UnsignedInteger nrSimplices = simplices.getSize();
    Bool standardSimplices = true;
    if (nrSimplices > 0)
    {
      for(UnsignedInteger i = 0; i < nrSimplices && standardSimplices; ++i)
      {
        if (simplices.cend_at(i) - simplices.cbegin_at(i) != 2 ||
            simplices(i, 0) != i || simplices(i, 1) != i + 1)
          standardSimplices = false;
      }
    }
    if (standardSimplices)
    {
      Mesh mesh(vertices, simplices);
      if (mesh.isRegular())
        p_implementation_ = new RegularGridEnclosingSimplex(mesh);
      else
      {
        try
        {
          // It will throw if vertices are not sorted.
          p_implementation_ = new EnclosingSimplexMonotonic1D(vertices);
        }
        catch (const InvalidArgumentException &)
        {}
      }
    }
  }
  if (!p_implementation_)
  {
    const UnsignedInteger largeDimension = ResourceMap::GetAsUnsignedInteger("EnclosingSimplexAlgorithm-LargeDimension");
    if (vertices.getDimension() >= largeDimension)
      p_implementation_ = new NaiveEnclosingSimplex(vertices, simplices);
    else
      p_implementation_ = new BoundingVolumeHierarchy(vertices, simplices);
  }
}

/* Parameters constructor */
EnclosingSimplexAlgorithm::EnclosingSimplexAlgorithm(const EnclosingSimplexAlgorithmImplementation & implementation)
  : TypedInterfaceObject<EnclosingSimplexAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
EnclosingSimplexAlgorithm::EnclosingSimplexAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<EnclosingSimplexAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
EnclosingSimplexAlgorithm::EnclosingSimplexAlgorithm(EnclosingSimplexAlgorithmImplementation * p_implementation)
  : TypedInterfaceObject<EnclosingSimplexAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/** Points accessor */
Sample EnclosingSimplexAlgorithm::getVertices() const
{
  return getImplementation()->getVertices();
}

/** Simplices accessor (flattened view) */
IndicesCollection EnclosingSimplexAlgorithm::getSimplices() const
{
  return getImplementation()->getSimplices();
}

void EnclosingSimplexAlgorithm::setVerticesAndSimplices(const Sample & vertices, const IndicesCollection & simplices)
{
  copyOnWrite();
  getImplementation()->setVerticesAndSimplices(vertices, simplices);
}

/* Get the index of the enclosing simplex of the given point */
UnsignedInteger EnclosingSimplexAlgorithm::query(const Point & x) const
{
  return getImplementation()->query(x);
}

/* Get the index of the enclosing simplex of the given points */
Indices EnclosingSimplexAlgorithm::query(const Sample & sample) const
{
  return getImplementation()->query(sample);
}

/* String converter */
String EnclosingSimplexAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << EnclosingSimplexAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String EnclosingSimplexAlgorithm::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << EnclosingSimplexAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
