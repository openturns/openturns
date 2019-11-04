//                                               -*- C++ -*-
/**
 *  @brief The class RegularGrid implements an equaly spaced set of real values
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
 *  MERCHANTABILITY or FITNESS FOR A PARTCULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_REGULARGRID_HXX
#define OPENTURNS_REGULARGRID_HXX

#include "openturns/Mesh.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RegularGrid
 *
 * The RegularGrid class defines some regular time slice defined by three values:
 *  - either a start time, an interval (aka timeStep) and a number of steps
 *  - or a start time, a number of steps and an end time
 *
 * Note: the end time conforms to the STL standard of "one past the end", so
 * the following formula defines the end time:
 * end time = start time + ( timeStep * steps )
 */

class OT_API RegularGrid
  : public Mesh
{
  CLASSNAME

public:

  /** Default constructor */
  RegularGrid();

  /** Parameters constructor */
  RegularGrid(const Scalar start,
              const Scalar step,
              const UnsignedInteger n);

  /** Parameters constructor */
  RegularGrid(const Mesh & mesh);

  /** Virtual constructor method */
  virtual RegularGrid * clone() const;

  /** Comparison operator */
  Bool operator == (const RegularGrid & rhs) const;

  /** Comparison operator */
  Bool operator != (const RegularGrid & rhs) const;

  /** Start accessor */
  Scalar getStart() const;

  /** This method computes the timestamp of the very next step past the time series (STL convention) */
  Scalar getEnd() const;

  /** Step accessor */
  Scalar getStep() const;

  /** TimeStamp number accessor */
  UnsignedInteger getN() const;

  /** Get the value at index i */
  Scalar getValue(const UnsignedInteger i) const;

  /** Get all the values as a sample */
  Point getValues() const;

  /** Check if the given grid follows the current grid */
  Bool follows(const RegularGrid & starter) const;

  /** Tells if it is regular */
  Bool isRegular() const;

  /**
   * String converter
   */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** The start of the regular grid */
  Scalar start_;

  /** The step of the regular grid */
  Scalar step_;

  /** The number of stamps of the underlying regular grid */
  UnsignedInteger n_;

}; // end class RegularGrid

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_REGULARGRID_HXX */
