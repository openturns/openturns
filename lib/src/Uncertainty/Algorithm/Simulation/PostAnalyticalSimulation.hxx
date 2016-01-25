//                                               -*- C++ -*-
/**
 *  @brief PostAnalyticalSimulation is a generic view of postAnalyticalSimulation methods for computing
 * probabilities and related quantities by sampling and estimation
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
#ifndef OPENTURNS_POSTANALYTICALSIMULATION_HXX
#define OPENTURNS_POSTANALYTICALSIMULATION_HXX

#include "Simulation.hxx"
#include "AnalyticalResult.hxx"
#include "StandardEvent.hxx"
#include "Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class PostAnalyticalSimulation
 */

class OT_API PostAnalyticalSimulation
  : public Simulation
{

  CLASSNAME;
public:
  /** for save/load mecanism */
  PostAnalyticalSimulation();

  /** Constructor with parameters */
  explicit PostAnalyticalSimulation(const AnalyticalResult & analyticalResult);

  /** Event accessor */
  AnalyticalResult getAnalyticalResult() const;

  /** Control probability accessor */
  NumericalScalar getControlProbability() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** FORM result upon which one compute a correction */
  AnalyticalResult analyticalResult_;

  /** Standard event associated to analytical result */
  StandardEvent standardEvent_;

  /** Standard distribution associated to the standard event */
  Distribution standardDistribution_;

  /** Probability associated to the control */
  NumericalScalar controlProbability_;

} ; /* class PostAnalyticalSimulation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POSTANALYTICALSIMULATION_HXX */
