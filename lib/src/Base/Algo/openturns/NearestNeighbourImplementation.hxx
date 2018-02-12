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
#ifndef OPENTURNS_NEARESTNEIGHBOURIMPLEMENTATION_HXX
#define OPENTURNS_NEARESTNEIGHBOURIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NearestNeighbourImplementation
 */

class OT_API NearestNeighbourImplementation
  : public PersistentObject
{

  CLASSNAME

public:

  /** Default constructor without parameters */
  NearestNeighbourImplementation();

  /** Virtual copy constructor */
  virtual NearestNeighbourImplementation * clone() const;

  /* Get the index of the nearest neighbour of the given point */
  virtual UnsignedInteger getNearestNeighbourIndex(const Point & x) const;

  /** Get the indices of the nearest neighbour of the given points */
  virtual Indices getNearestNeighbourIndex(const Sample & sample) const;

  /* Get the nearest neighbour of the given point */
  virtual Point getNearestNeighbour(const Point & x) const;

  /** Get the nearest neighbour of the given points */
  virtual Sample getNearestNeighbour(const Sample & sample) const;

  /* Get the indices of the k nearest neighbours of the given point */
  virtual Indices getNearestNeighboursIndices(const Point & x,
                                              const UnsignedInteger k,
                                              const Bool sorted = false) const;

  /* Get the k nearest neighbours of the given point */
  virtual Sample getNearestNeighbours(const Point & x,
                                      const UnsignedInteger k,
                                      const Bool sorted = false) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class NearestNeighbourImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTNEIGHBOURIMPLEMENTATION_HXX */
