//                                               -*- C++ -*-
/**
 *  @brief A class which implements the AggregatedProcess process
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  AggregatedProcess * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const override;

  /** Realization accessor */
  Field getRealization() const override;

  /** Continuous realization accessor */
  Function getContinuousRealization() const override;

  /** Continuation of the last realization on a given number of steps */
  using ProcessImplementation::getFuture;
  TimeSeries getFuture(const UnsignedInteger stepNumber) const override;

  /** Get the process corresponding to the i-th marginal component */
  Process getMarginal(const UnsignedInteger i) const override;

  /** Get the process corresponding to indices components */
  Process getMarginal(const Indices & indices) const override;

  /** Process collection accessor */
  void setProcessCollection(const ProcessCollection & coll);
  ProcessCollection getProcessCollection() const;

  /** Mesh accessor */
  void setMesh(const Mesh & mesh) override;
  void setTimeGrid(const RegularGrid & timeGrid) override;

  /** Is the underlying gaussian process ? */
  Bool isNormal() const override;

  /** Is the underlying a stationary process ? */
  Bool isStationary() const override;

  /** Covariance model accessor */
  CovarianceModel getCovarianceModel() const override;

  /** Trend accessor */
  TrendTransform getTrend() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** The collection of process to aggregate */
  ProcessPersistentCollection processCollection_;

}; /* class AggregatedProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_AGGREGATEDPROCESS_HXX */
