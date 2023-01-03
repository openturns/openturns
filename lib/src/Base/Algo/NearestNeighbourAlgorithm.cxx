//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast search of neighbours.
 *  This is the interface class
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
#include "openturns/NearestNeighbourAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/KDTree.hxx"
#include "openturns/RegularGridNearestNeighbour.hxx"
#include "openturns/NearestNeighbour1D.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NearestNeighbourAlgorithm)

/* Constructor without parameters */
NearestNeighbourAlgorithm::NearestNeighbourAlgorithm()
  : TypedInterfaceObject<NearestNeighbourAlgorithmImplementation>(new KDTree())
{
  // Nothing to do
}

/** Constructor with parameter */
NearestNeighbourAlgorithm::NearestNeighbourAlgorithm(const Sample & sample)
  : TypedInterfaceObject<NearestNeighbourAlgorithmImplementation>()
{
  if (sample.getDimension() == 1)
  {
    Mesh mesh(sample);
    if (mesh.isRegular())
      p_implementation_ = new RegularGridNearestNeighbour(sample);
    else
      p_implementation_ = new NearestNeighbour1D(sample);
  }
  else
    p_implementation_ = new KDTree(sample);
}

/* Copy constructors */
NearestNeighbourAlgorithm::NearestNeighbourAlgorithm(const NearestNeighbourAlgorithmImplementation & implementation)
  : TypedInterfaceObject<NearestNeighbourAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation */
NearestNeighbourAlgorithm::NearestNeighbourAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<NearestNeighbourAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation pointer */
NearestNeighbourAlgorithm::NearestNeighbourAlgorithm(NearestNeighbourAlgorithmImplementation * p_implementation)
  : TypedInterfaceObject<NearestNeighbourAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/* Sample accessor */
Sample NearestNeighbourAlgorithm::getSample() const
{
  return getImplementation()->getSample();
}

void NearestNeighbourAlgorithm::setSample(const Sample & sample)
{
  return getImplementation()->setSample(sample);
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger NearestNeighbourAlgorithm::query(const Point & x) const
{
  return getImplementation()->query(x);
}

/* Get the index of the nearest neighbour of the given points */
Indices NearestNeighbourAlgorithm::query(const Sample & sample) const
{
  return getImplementation()->query(sample);
}

/* Get the indices of the k nearest neighbours of the given point */
Indices NearestNeighbourAlgorithm::queryK(const Point & x,
    const UnsignedInteger k,
    const Bool sorted) const
{
  return getImplementation()->queryK(x, k, sorted);
}

/* String converter */
String NearestNeighbourAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NearestNeighbourAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* String converter */
String NearestNeighbourAlgorithm::__str__(const String & offset) const
{
  OSS oss(false);
  oss << "class=" << NearestNeighbourAlgorithm::GetClassName()
      << " implementation=" << getImplementation()->__str__(offset);
  return oss;
}

END_NAMESPACE_OPENTURNS
