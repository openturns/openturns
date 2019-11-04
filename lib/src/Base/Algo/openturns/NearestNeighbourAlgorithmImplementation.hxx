//                                               -*- C++ -*-
/**
 *  @brief  This class provides fast search of neighbours.
 *  This is an abstract class
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
#ifndef OPENTURNS_NEARESTNEIGHBOURALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_NEARESTNEIGHBOURALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NearestNeighbourAlgorithmImplementation
 */

class OT_API NearestNeighbourAlgorithmImplementation
  : public PersistentObject
{

  CLASSNAME

public:

  /** Default constructor */
  NearestNeighbourAlgorithmImplementation();

  /** Virtual copy constructor */
  virtual NearestNeighbourAlgorithmImplementation * clone() const;

#ifndef SWIG
  /** Default virtual constructor */
  virtual NearestNeighbourAlgorithmImplementation * emptyClone() const;
#endif

  /** Sample accessor */
  virtual Sample getSample() const;
  virtual void setSample(const Sample & sample);

  /** Get the index of the nearest neighbour of the given point */
  virtual UnsignedInteger query(const Point & x) const;

  /** Get the indices of the nearest neighbour of the given points */
  virtual Indices query(const Sample & sample) const;

  /* Get the indices of the k nearest neighbours of the given point */
  virtual Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted = false) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

} ; /* class NearestNeighbourAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTNEIGHBOURALGORITHMIMPLEMENTATION_HXX */
