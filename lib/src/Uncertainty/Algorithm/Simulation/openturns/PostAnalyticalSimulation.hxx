//                                               -*- C++ -*-
/**
 *  @brief PostAnalyticalSimulation is a generic view of postAnalyticalSimulation methods for computing
 * probabilities and related quantities by sampling and estimation
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
#ifndef OPENTURNS_POSTANALYTICALSIMULATION_HXX
#define OPENTURNS_POSTANALYTICALSIMULATION_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/AnalyticalResult.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class PostAnalyticalSimulation
 */

class OT_API PostAnalyticalSimulation
  : public EventSimulation
{

  CLASSNAME
public:
  /** for save/load mecanism */
  PostAnalyticalSimulation();

  /** Constructor with parameters */
  explicit PostAnalyticalSimulation(const AnalyticalResult & analyticalResult);

  /** Virtual constructor */
  PostAnalyticalSimulation * clone() const override;

  /** Event accessor */
  AnalyticalResult getAnalyticalResult() const;

  /** Control probability accessor */
  Scalar getControlProbability() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** FORM result upon which one compute a correction */
  AnalyticalResult analyticalResult_;

  /** Standard event associated to analytical result */
  StandardEvent standardEvent_;

  /** Standard distribution associated to the standard event */
  Distribution standardDistribution_;

  /** Probability associated to the control */
  Scalar controlProbability_;

} ; /* class PostAnalyticalSimulation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POSTANALYTICALSIMULATION_HXX */
