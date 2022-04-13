//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast search of neighbours.
 *  This is an abstract class
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NearestNeighbourAlgorithmImplementation
 */

CLASSNAMEINIT(NearestNeighbourAlgorithmImplementation)

static const Factory<NearestNeighbourAlgorithmImplementation> Factory_NearestNeighbourAlgorithmImplementation;

/* Constructor without parameters */
NearestNeighbourAlgorithmImplementation::NearestNeighbourAlgorithmImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
NearestNeighbourAlgorithmImplementation * NearestNeighbourAlgorithmImplementation::clone() const
{
  return new NearestNeighbourAlgorithmImplementation(*this);
}

/* Default virtual constructor */
NearestNeighbourAlgorithmImplementation * NearestNeighbourAlgorithmImplementation::emptyClone() const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourAlgorithmImplementation::emptyClone() const";
}

/* Sample accessor */
Sample NearestNeighbourAlgorithmImplementation::getSample() const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourAlgorithmImplementation::getSample() const";
}

void NearestNeighbourAlgorithmImplementation::setSample(const Sample & )
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourAlgorithmImplementation::setSample(const Sample & sample)";
}

/* Get the index of the nearest neighbour of the given point */
UnsignedInteger NearestNeighbourAlgorithmImplementation::query(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourAlgorithmImplementation::query(const Point & x) const";
}

/* Get the index of the nearest neighbour of the given points */
Indices NearestNeighbourAlgorithmImplementation::query(const Sample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Indices result(size);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    result[i] = query(sample[i]);
  }
  return result;
}

/* Get the indices of the k nearest neighbours of the given point */
Indices NearestNeighbourAlgorithmImplementation::queryK(const Point &,
    const UnsignedInteger,
    const Bool ) const
{
  throw NotYetImplementedException(HERE) << "In NearestNeighbourAlgorithmImplementation::queryK(const Point & x, const UnsignedInteger k, const Bool sorted) const";
}

/* String converter */
String NearestNeighbourAlgorithmImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NearestNeighbourAlgorithmImplementation::GetClassName();
  return oss;
}

/* String converter */
String NearestNeighbourAlgorithmImplementation::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class=" << NearestNeighbourAlgorithmImplementation::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
