//                                               -*- C++ -*-
/**
 *  @brief Brute force algorithm for nearest-neighbour lookup
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
  NaiveNearestNeighbour * clone() const override;

#ifndef SWIG
  /** Virtual default constructor */
  NaiveNearestNeighbour * emptyClone() const override;
#endif

  /** Sample accessor */
  Sample getSample() const override;
  void setSample(const Sample & sample) override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Get the index of the nearest neighbour of the given point */
  UnsignedInteger query(const Point & x) const override;
  using NearestNeighbourAlgorithmImplementation::query;

  /** Get the indices of the k nearest neighbours of the given point */
  Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted  = false) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Input sample */
  Sample points_;

}; /* class NaiveNearestNeighbour */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NAIVENEARESTNEIGHBOUR_HXX */
