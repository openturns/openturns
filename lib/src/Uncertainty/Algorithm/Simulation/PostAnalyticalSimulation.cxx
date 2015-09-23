//                                               -*- C++ -*-
/**
 *  @brief PostAnalyticalSimulation is a generic view of postAnalyticalSimulation methods for computing
 * probabilities and related quantities by sampling and estimation
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
#include "PostAnalyticalSimulation.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/*
 * @class PostAnalyticalSimulation
 */

CLASSNAMEINIT(PostAnalyticalSimulation);

static const Factory<PostAnalyticalSimulation> RegisteredFactory;

/* Constructor with parameters */
PostAnalyticalSimulation::PostAnalyticalSimulation()
  : Simulation()
  , controlProbability_(0.)
{
}

/* Constructor with parameters */
PostAnalyticalSimulation::PostAnalyticalSimulation(const AnalyticalResult & analyticalResult)
  : Simulation(analyticalResult.getLimitStateVariable())
  , analyticalResult_(analyticalResult)
  , standardEvent_(StandardEvent(getEvent()))
  , standardDistribution_(standardEvent_.getImplementation()->getAntecedent()->getDistribution())
{
  // Compute the probability associated to the analytical result
  controlProbability_ = standardDistribution_.getMarginal(0).computeCDF(-analyticalResult.getHasoferReliabilityIndex());
}

/* Analytical result accessor */
AnalyticalResult PostAnalyticalSimulation::getAnalyticalResult() const
{
  return analyticalResult_;
}

/* Control probability accessor */
NumericalScalar PostAnalyticalSimulation::getControlProbability() const
{
  return controlProbability_;
}

/* String converter */
String PostAnalyticalSimulation::__repr__() const
{
  OSS oss;
  oss << "class=" << PostAnalyticalSimulation::GetClassName()
      << " analyticalResult=" << analyticalResult_
      << " controlProbability_=" << controlProbability_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void PostAnalyticalSimulation::save(Advocate & adv) const
{
  Simulation::save(adv);
  adv.saveAttribute("analyticalResult_", analyticalResult_);
}

/* Method load() reloads the object from the StorageManager */
void PostAnalyticalSimulation::load(Advocate & adv)
{
  Simulation::load(adv);
  adv.loadAttribute("analyticalResult_", analyticalResult_);
  standardEvent_ = StandardEvent(getEvent());
  standardDistribution_ = standardEvent_.getImplementation()->getAntecedent()->getDistribution();
  controlProbability_ = standardDistribution_.getMarginal(0).computeCDF(-analyticalResult_.getHasoferReliabilityIndex());
}

END_NAMESPACE_OPENTURNS
