//                                               -*- C++ -*-
/**
 *  @brief A class which implements the RandomWalk process
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_RANDOMWALK_HXX
#define OPENTURNS_RANDOMWALK_HXX

#include "ProcessImplementation.hxx"
#include "Process.hxx"
#include "Pointer.hxx"
#include "Distribution.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RandomWalk
 *
 * An interface class for composite White Noise
 */
class OT_API RandomWalk
  : public ProcessImplementation
{
  CLASSNAME;

public:

  /** Default constructor */
  RandomWalk();

  /** Standard constructor */
  RandomWalk(const NumericalPoint & origin,
             const Distribution & distribution);

  /** Standard constructor */
  RandomWalk(const NumericalPoint & origin,
             const Distribution & distribution,
             const RegularGrid & timeGrid);

  /** Virtual constructor */
  virtual RandomWalk * clone() const;

  /** String converter */
  String __repr__() const;

  /** Is the underlying stationary ? */
  Bool isStationary() const;

  /** Is the underlying a Normal process ? */
  Bool isNormal() const;

  /** Realization accessor */
  Field getRealization() const;

  /** Continuation of the last realization on a given number of steps */
  using ProcessImplementation::getFuture;
  TimeSeries getFuture(const UnsignedInteger stepNumber) const;

  /** Get the process corresponding to the i-th marginal component */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the marginal process corresponding to indices components */
  Implementation getMarginal(const Indices & indices) const;

  /** Distribution accessor */
  Distribution getDistribution() const;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);

  /** Origin accessor */
  NumericalPoint getOrigin() const;

  /** Origin accessor */
  void setOrigin(const NumericalPoint & origin);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /** The origin of the walk */
  NumericalPoint origin_;

  /** The distribution of the walk steps */
  Distribution distribution_;

  /** The current position of the walk */
  mutable NumericalPoint currentPosition_;

}; /* class RandomWalk */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMWALK_HXX */
