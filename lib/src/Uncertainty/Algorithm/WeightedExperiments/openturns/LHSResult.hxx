//                                               -*- C++ -*-
/**
 *  @brief LHSResult
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_LHSRESULT_HXX
#define OPENTURNS_LHSRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/SpaceFilling.hxx"
#include "openturns/LHSResult.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Graph.hxx"

namespace OT
{

/**
 * @class LHSResult
 *
 * LHSResult is the interface class for temperature profile computation for SimulatedAnnealing
 */
class OT_API LHSResult
  : public PersistentObject
{
  CLASSNAME;

public:

  typedef PersistentCollection<NumericalSample> NumericalSamplePersistentCollection;

  /** Default constructor */
  LHSResult();

  /** Default constructor */
  explicit LHSResult(const SpaceFilling & spaceFilling, UnsignedInteger restart = 0);

  /** Virtual constructor */
  virtual LHSResult * clone() const;

  /** Accessors */
  UnsignedInteger getNumberOfRestarts() const;

  /** Attributes for getting elements of result */
  NumericalSample getOptimalDesign() const;
  NumericalSample getOptimalDesign(UnsignedInteger restart) const;
  NumericalScalar getOptimalValue() const;
  NumericalScalar getOptimalValue(UnsignedInteger restart) const;
  NumericalSample getAlgoHistory() const;
  NumericalSample getAlgoHistory(UnsignedInteger restart) const;
  NumericalScalar getC2() const;
  NumericalScalar getC2(UnsignedInteger restart) const;
  NumericalScalar getPhiP() const;
  NumericalScalar getPhiP(UnsignedInteger restart) const;
  NumericalScalar getMinDist() const;
  NumericalScalar getMinDist(UnsignedInteger restart) const;

  /** Graphical methods - Draw criterion history */
  Graph drawHistoryCriterion(const String & title = "") const;
  Graph drawHistoryCriterion(UnsignedInteger restart, const String & title = "") const;
  Graph drawHistoryProbability(const String & title = "") const;
  Graph drawHistoryProbability(UnsignedInteger restart, const String & title = "") const;
  Graph drawHistoryTemperature(const String & title = "") const;
  Graph drawHistoryTemperature(UnsignedInteger restart, const String & title = "") const;

  /** Add an optimal design with its history */
  void add(const NumericalSample & optimalDesign, NumericalScalar criterion,
           NumericalScalar C2, NumericalScalar PhiP, NumericalScalar MinDist,
           const NumericalSample & algoHistory);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  /** Method that draw data */
  Graph drawCurveData(const NumericalSample & data, const String & title) const;

  /** Method to extract field index from data */
  UnsignedInteger findDescription(const char *text) const;

private:
  /** Space filling criterion */
  SpaceFilling spaceFilling_;

  /** Number of restarts */
  UnsignedInteger restart_;

  /** Best criterion value */
  mutable NumericalScalar optimalCriterion_;

  /** Index of the best designs in all restarts */
  mutable UnsignedInteger optimalIndex_;

  /** Collection of best designs (size 1+restart_)*/
  NumericalSamplePersistentCollection collDesigns_;

  /** Collection of algo histories (size 1+restart_, dimension depending on algorithm)*/
  NumericalSamplePersistentCollection collAlgoHistory_;

  /** Collection of all result criteria (size 1+restart_, dimension 4)*/
  NumericalSample criteria_;

}; /* class LHSResult */

} /* namespace OT */

#endif /* OT_LHSRESULT_HXX */
