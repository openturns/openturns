//                                               -*- C++ -*-
/**
 *  @brief A class which implements the AggregatedProcess process
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
#ifndef OPENTURNS_AGGREGATEDPROCESS_HXX
#define OPENTURNS_AGGREGATEDPROCESS_HXX

#include "openturns/ProcessImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Process.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AggregatedProcess
 *
 * The implementation of the AggregatedProcess class
 */
class OT_API AggregatedProcess
  : public ProcessImplementation
{
  CLASSNAME

public:

  typedef Collection< Process >           ProcessCollection;
  typedef PersistentCollection< Process > ProcessPersistentCollection;

  /** Default constructor */
  AggregatedProcess();

  /** Standard constructor */
  AggregatedProcess(const ProcessCollection & coll);

  /** Virtual constructor */
  AggregatedProcess * clone() const;

  /** String converter */
  String __repr__() const;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const;

  /** Realization accessor */
  Field getRealization() const;

  /** Continuous realization accessor */
  Function getContinuousRealization() const;

  /** Continuation of the last realization on a given number of steps */
  using ProcessImplementation::getFuture;
  TimeSeries getFuture(const UnsignedInteger stepNumber) const;

  /** Get the process corresponding to the i-th marginal component */
  Process getMarginal(const UnsignedInteger i) const;

  /** Get the process corresponding to indices components */
  Process getMarginal(const Indices & indices) const;

  /** Process collection accessor */
  void setProcessCollection(const ProcessCollection & coll);
  ProcessCollection getProcessCollection() const;

  /** Mesh accessor */
  void setMesh(const Mesh & mesh);
  void setTimeGrid(const RegularGrid & timeGrid);

  /** Is the underlying gaussian process ? */
  virtual Bool isNormal() const;

  /** Is the underlying a stationary process ? */
  virtual Bool isStationary() const;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const;

  /** Trend accessor */
  TrendTransform getTrend() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** The collection of process to aggregate */
  ProcessPersistentCollection processCollection_;

}; /* class AggregatedProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_AGGREGATEDPROCESS_HXX */
