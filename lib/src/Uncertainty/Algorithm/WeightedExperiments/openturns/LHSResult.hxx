//                                               -*- C++ -*-
/**
 *  @brief LHSResult
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
#ifndef OPENTURNS_LHSRESULT_HXX
#define OPENTURNS_LHSRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/SpaceFilling.hxx"
#include "openturns/LHSResult.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Sample.hxx"
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
  CLASSNAME

public:

  typedef PersistentCollection<Sample> SamplePersistentCollection;

  /** Default constructor */
  LHSResult();

  /** Default constructor */
  explicit LHSResult(const SpaceFilling & spaceFilling, UnsignedInteger restart = 0);

  /** Virtual constructor */
  virtual LHSResult * clone() const;

  /** Accessors */
  UnsignedInteger getNumberOfRestarts() const;

  /** Attributes for getting elements of result */
  Sample getOptimalDesign() const;
  Sample getOptimalDesign(UnsignedInteger restart) const;
  Scalar getOptimalValue() const;
  Scalar getOptimalValue(UnsignedInteger restart) const;
  Sample getAlgoHistory() const;
  Sample getAlgoHistory(UnsignedInteger restart) const;
  Scalar getC2() const;
  Scalar getC2(UnsignedInteger restart) const;
  Scalar getPhiP() const;
  Scalar getPhiP(UnsignedInteger restart) const;
  Scalar getMinDist() const;
  Scalar getMinDist(UnsignedInteger restart) const;

  /** Graphical methods - Draw criterion history */
  Graph drawHistoryCriterion(const String & title = "") const;
  Graph drawHistoryCriterion(UnsignedInteger restart, const String & title = "") const;
  Graph drawHistoryProbability(const String & title = "") const;
  Graph drawHistoryProbability(UnsignedInteger restart, const String & title = "") const;
  Graph drawHistoryTemperature(const String & title = "") const;
  Graph drawHistoryTemperature(UnsignedInteger restart, const String & title = "") const;

  /** Add an optimal design with its history */
  void add(const Sample & optimalDesign, Scalar criterion,
           Scalar C2, Scalar PhiP, Scalar MinDist,
           const Sample & algoHistory);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  /** Method that draw data */
  Graph drawCurveData(const Sample & data, const String & title) const;

  /** Method to extract field index from data */
  UnsignedInteger findDescription(const char *text) const;

private:
  /** Space filling criterion */
  SpaceFilling spaceFilling_;

  /** Number of restarts */
  UnsignedInteger restart_;

  /** Best criterion value */
  mutable Scalar optimalCriterion_;

  /** Index of the best designs in all restarts */
  mutable UnsignedInteger optimalIndex_;

  /** Collection of best designs (size 1+restart_)*/
  SamplePersistentCollection collDesigns_;

  /** Collection of algo histories (size 1+restart_, dimension depending on algorithm)*/
  SamplePersistentCollection collAlgoHistory_;

  /** Collection of all result criteria (size 1+restart_, dimension 4)*/
  Sample criteria_;

}; /* class LHSResult */

} /* namespace OT */

#endif /* OT_LHSRESULT_HXX */
