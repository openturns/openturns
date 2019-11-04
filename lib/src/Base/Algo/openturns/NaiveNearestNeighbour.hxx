//                                               -*- C++ -*-
/**
 *  @brief Brute force algorithm for nearest-neighbour lookup
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
#ifndef OPENTURNS_NAIVENEARESTNEIGHBOUR_HXX
#define OPENTURNS_NAIVENEARESTNEIGHBOUR_HXX

#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

class NaiveNearestNeighboursFinder;

/**
 * @class NaiveNearestNeighbour
 *
 * Brute force algorithm for nearest-neighbour lookup.
 */
class OT_API NaiveNearestNeighbour
  : public NearestNeighbourAlgorithmImplementation
{
  CLASSNAME

  friend class NaiveNearestNeighboursFinder;

public:

  /** Default constructor */
  NaiveNearestNeighbour();

  /** Parameters constructor */
  explicit NaiveNearestNeighbour(const Sample & sample);

  /** Virtual constructor */
  virtual NaiveNearestNeighbour * clone() const;

#ifndef SWIG
  /** Virtual default constructor */
  virtual NaiveNearestNeighbour * emptyClone() const;
#endif

  /** Sample accessor */
  virtual Sample getSample() const;
  virtual void setSample(const Sample & sample);

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Get the index of the nearest neighbour of the given point */
  UnsignedInteger query(const Point & x) const;
  using NearestNeighbourAlgorithmImplementation::query;

  /** Get the indices of the k nearest neighbours of the given point */
  virtual Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted  = false) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Input sample */
  Sample points_;

}; /* class NaiveNearestNeighbour */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAIVENEARESTNEIGHBOUR_HXX */
