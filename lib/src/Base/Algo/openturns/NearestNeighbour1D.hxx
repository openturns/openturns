//                                               -*- C++ -*-
/**
 *  @brief Nearest-neighbour lookup for 1D sample
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
#ifndef OPENTURNS_NEARESTNEIGHBOUR1D_HXX
#define OPENTURNS_NEARESTNEIGHBOUR1D_HXX

#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

class NearestNeighbours1DFinder;

/**
 * @class NearestNeighbour1D
 *
 * Nearest-neighbour lookup for 1D sample, by dichotomy.
 */
class OT_API NearestNeighbour1D
  : public NearestNeighbourAlgorithmImplementation
{
  CLASSNAME

  friend class NearestNeighbours1DFinder;

public:

  /** Default constructor */
  NearestNeighbour1D();

  /** Parameters constructor */
  explicit NearestNeighbour1D(const Sample & sample);

  /** Virtual constructor */
  virtual NearestNeighbour1D * clone() const;

#ifndef SWIG
  /** Virtual default constructor */
  virtual NearestNeighbour1D * emptyClone() const;
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

  /** Get the index of the nearest neighbour of the given scalar */
  UnsignedInteger queryScalar(const Scalar x) const;

  /** Get the index of the nearest neighbour of the given scalars */
  Indices queryScalar(const Point & x) const;

  /** Get the indices of the k nearest neighbours of the given point */
  Indices queryK(const Point & x, const UnsignedInteger k, const Bool sorted  = false) const;

  /** Get the indices of the k nearest neighbours of the given scalar */
  Indices queryScalarK(const Scalar x, const UnsignedInteger k, const Bool sorted  = false) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Input sample */
  Sample points_;

  /** Sorted input sample, stored into a Point for performance reasons */
  Point sorted_;

  /** Indirection */
  Indices originalIndex_;

}; /* class NearestNeighbour1D */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTNEIGHBOUR1D_HXX */
