//                                               -*- C++ -*-
/**
 *  @brief Analytical implements an algorithm to find the design point
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
#ifndef OPENTURNS_ANALYTICAL_HXX
#define OPENTURNS_ANALYTICAL_HXX

#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"
#include "NumericalPointWithDescription.hxx"
#include "OptimizationSolver.hxx"
#include "Collection.hxx"
#include "Event.hxx"
#include "StandardEvent.hxx"
#include "NumericalSample.hxx"
#include "NumericalMathFunction.hxx"
#include "Graph.hxx"
#include "Description.hxx"
#include "AnalyticalResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Analytical
 * Analytical implements an algorithm to find the design point
 * and computes the results of an analytical result
 */

class OT_API Analytical
  : public PersistentObject
{

  CLASSNAME;
public:

  typedef Collection<NumericalPointWithDescription> Sensitivity;

  /** Constructor with parameters */
  Analytical(const OptimizationSolver & solver,
             const Event & compositeEvent,
             const NumericalPoint & physicalStartingPoint);


  /** Virtual constructor */
  virtual Analytical * clone() const;

  /** Physical starting point accessor */
  NumericalPoint getPhysicalStartingPoint() const;

  /** Physical starting point accessor */
  void setPhysicalStartingPoint(const NumericalPoint & physicalStartingPoint);

  /** Event accessor */
  Event getEvent() const;

  /** Event accessor */
  void setEvent(const Event & event);

  /** OptimizationSolver accessor */
  OptimizationSolver getNearestPointAlgorithm() const;

  /** OptimizationSolver accessor */
  void setNearestPointAlgorithm(const OptimizationSolver & solver);

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

  OptimizationSolver nearestPointAlgorithm_;
  Event event_;
  NumericalPoint physicalStartingPoint_;
  AnalyticalResult result_;

} ; /* class Analytical */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ANALYTICAL_HXX */
