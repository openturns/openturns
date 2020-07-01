//                                               -*- C++ -*-
/**
 *  @brief Analytical implements an algorithm to find the design point
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
#ifndef OPENTURNS_ANALYTICAL_HXX
#define OPENTURNS_ANALYTICAL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Collection.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Description.hxx"
#include "openturns/AnalyticalResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Analytical
 * Analytical implements an algorithm to find the design point
 * and computes the results of an analytical result
 */

class OT_API Analytical
  : public PersistentObject
{

  CLASSNAME
public:

  typedef Collection<PointWithDescription> Sensitivity;

  /** Constructor with parameters */
  Analytical(const OptimizationAlgorithm & solver,
             const RandomVector & compositeEvent,
             const Point & physicalStartingPoint);


  /** Virtual constructor */
  virtual Analytical * clone() const;

  /** Physical starting point accessor */
  Point getPhysicalStartingPoint() const;

  /** Physical starting point accessor */
  void setPhysicalStartingPoint(const Point & physicalStartingPoint);

  /** Event accessor */
  RandomVector getEvent() const;

  /** Event accessor */
  virtual void setEvent(const RandomVector & event);

  /** OptimizationAlgorithm accessor */
  OptimizationAlgorithm getNearestPointAlgorithm() const;

  /** OptimizationAlgorithm accessor */
  void setNearestPointAlgorithm(const OptimizationAlgorithm & solver);

  /** Result accessor */
  virtual AnalyticalResult getAnalyticalResult() const;

  /** String converter */
  String __repr__() const;

  /** Performs the actual computation. */
  virtual void run();

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /** For save/load mechanism*/
  Analytical() {};

  friend class Factory<Analytical>;

private:

  OptimizationAlgorithm nearestPointAlgorithm_;
  RandomVector event_;
  Point physicalStartingPoint_;
  AnalyticalResult result_;

} ; /* class Analytical */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ANALYTICAL_HXX */
