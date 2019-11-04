//                                               -*- C++ -*-
/**
 *  @brief RandomGeneratorState implements methods to manage the random generator state
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
#ifndef OPENTURNS_RANDOMGENERATORSTATE_HXX
#define OPENTURNS_RANDOMGENERATORSTATE_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Indices.hxx"


BEGIN_NAMESPACE_OPENTURNS

/** A couple (internal state array, picking index) */
class OT_API RandomGeneratorState
  : public PersistentObject
{
  CLASSNAME

  friend class RandomGenerator;

public:

  /** Default constructor */
  RandomGeneratorState();

  /** Standard constructor */
  RandomGeneratorState(const Indices buffer, const UnsignedInteger index);

  /** Virtual constructor */
  RandomGeneratorState * clone() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Buffer Acccessor */
  Indices getBuffer() const;

  /** Index Acccessor */
  UnsignedInteger getIndex() const;

  /** Comparison operator */
  Bool operator ==(const RandomGeneratorState & other) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


private:
  Indices buffer_;
  UnsignedInteger index_;


}; /* end class RandomGeneratorState */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMGENERATORSTATE_HXX */
