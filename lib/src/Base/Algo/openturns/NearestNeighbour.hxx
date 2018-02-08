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
#ifndef OPENTURNS_NEARESTNEIGHBOUR_HXX
#define OPENTURNS_NEARESTNEIGHBOUR_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/NearestNeighbourImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NearestNeighbour
 */

class OT_API NearestNeighbour
  : public TypedInterfaceObject<NearestNeighbourImplementation>
{

  CLASSNAME

public:

  typedef Pointer<NearestNeighbourImplementation>    Implementation;

  /** Default onstructor */
  NearestNeighbour();

  /** Parameter constructor */
  explicit NearestNeighbour(const Sample & sample);

  /** Copy constructors */
  NearestNeighbour(const NearestNeighbourImplementation & implementation);

  /** Constructor from implementation */
  NearestNeighbour(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  NearestNeighbour(NearestNeighbourImplementation * p_implementation);
#endif

  /** Get the index of the nearest neighbour of the given point */
  virtual UnsignedInteger getNearestNeighbourIndex(const Point & x) const;

  /** Get the indices of the nearest neighbour of the given points */
  virtual Indices getNearestNeighbourIndex(const Sample & sample) const;

  /** Get the nearest neighbour of the given point */
  virtual Point getNearestNeighbour(const Point & x) const;

  /** Get the nearest neighbour of the given points */
  virtual Sample getNearestNeighbour(const Sample & sample) const;

  /** Get the indices of the k nearest neighbours of the given point */
  virtual Indices getNearestNeighboursIndices(const Point & x,
                                              const UnsignedInteger k,
                                              const Bool sorted  = false) const;

  /** Get the k nearest neighbours of the given point */
  virtual Sample getNearestNeighbours(const Point & x,
                                      const UnsignedInteger k,
                                      const Bool sorted  = false) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;


} ; /* class NearestNeighbour */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTNEIGHBOUR_HXX */
