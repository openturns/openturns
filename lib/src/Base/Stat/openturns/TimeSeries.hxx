//                                               -*- C++ -*-
/**
 *  @brief The class TimeSeries implements values indexed by time
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TIMESERIES_HXX
#define OPENTURNS_TIMESERIES_HXX

#include "openturns/Point.hxx"
#include "openturns/Description.hxx"
#include "openturns/Indices.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Graph.hxx"
#include "openturns/RegularGrid.hxx"
#include "openturns/FieldImplementation.hxx"
#include "openturns/Field.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TimeSeries
 */

class OT_API TimeSeries
  : public FieldImplementation
{
  CLASSNAME

public:

  /**
   * Default constructor
   */
  TimeSeries();

  /** Standard constructor */
  TimeSeries(const UnsignedInteger n,
             const UnsignedInteger dim);

  /** Constructor from a TimeGrid and a dimension */
  TimeSeries(const RegularGrid & tg,
             const UnsignedInteger dim);

  /** Constructor from a TimeGrid and a sample */
  TimeSeries(const RegularGrid & tg,
             const Sample & sample);

  /** Constructor from a Field */
  TimeSeries(const Field & field);

#ifndef SWIG
  /** Constructor from a collection of Point */
  TimeSeries(const Collection<Point> & coll);
#endif

  /** Virtual constructor */
  TimeSeries * clone() const override;

  /**
   * String converter
   * This method shows human readable information on the
   * internal state of an TimeSeries. It is used when streaming
   * the TimeSeries or for user information.
   */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  using FieldImplementation::operator ==;
  Bool operator ==(const TimeSeries & other) const;

  /** Append an element to the collection */
  TimeSeries & add(const Point & point);

  /** Append a sample to the collection */
  TimeSeries & add(const Sample & sample);

  /** Append another time series to the collection. The time grids must match (one follows the other) */
  TimeSeries & add(const TimeSeries & continuer);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The start time of the time series */
  Scalar start_;

  /** The interval of the underlying regular time grid */
  Scalar timeStep_;

  /** The number of timestamps of the underlying regular time grid */
  UnsignedInteger n_;

}; /* class TimeSeries */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TIMESERIES_HXX */
