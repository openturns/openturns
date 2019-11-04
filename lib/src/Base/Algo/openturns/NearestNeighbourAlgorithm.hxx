//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast search of neighbours.
 *  This is the interface class
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_NEARESTNEIGHBOURALGORITHM_HXX
#define OPENTURNS_NEARESTNEIGHBOURALGORITHM_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NearestNeighbourAlgorithm
 */

class OT_API NearestNeighbourAlgorithm
  : public TypedInterfaceObject<NearestNeighbourAlgorithmImplementation>
{

  CLASSNAME

public:

  typedef Pointer<NearestNeighbourAlgorithmImplementation>    Implementation;

  /** Default constructor */
  NearestNeighbourAlgorithm();

  /** Parameters constructor */
  explicit NearestNeighbourAlgorithm(const Sample & sample);

  /** Copy constructors */
  NearestNeighbourAlgorithm(const NearestNeighbourAlgorithmImplementation & implementation);

  /** Constructor from implementation */
  NearestNeighbourAlgorithm(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  NearestNeighbourAlgorithm(NearestNeighbourAlgorithmImplementation * p_implementation);
#endif

  /** Sample accessor */
  virtual Sample getSample() const;
  virtual void setSample(const Sample & sample);

  /** Get the index of the nearest neighbour of the given point */
  virtual UnsignedInteger query(const Point & x) const;

  /** Get the indices of the nearest neighbour of the given points */
  virtual Indices query(const Sample & sample) const;

  /** Get the indices of the k nearest neighbours of the given point */
  virtual Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted  = false) const;

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

} ; /* class NearestNeighbourAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTNEIGHBOURALGORITHM_HXX */
