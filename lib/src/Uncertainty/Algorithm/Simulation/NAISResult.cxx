//                                               -*- C++ -*-
/**
 *  @brief class for NAISResult
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
#include "openturns/NAISResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NAISResult
 */


CLASSNAMEINIT(NAISResult);

/* Default constructor */
NAISResult::NAISResult()
  : SimulationResult()
{
  //Nothing TO DO
}

/** get Probability Estimate */
Scalar NAISResult::getProbabilityEstimate() const 
{
  return probabilityEstimate_;
}

/** set Probability Estimate */
void NAISResult::setProbabilityEstimate(const Scalar & probabilityEstimate)
{
  probabilityEstimate_ = probabilityEstimate;
}

/** get Samples */
Sample NAISResult::getSamples() const 
{
  return samples_;
}

/** set NAISResult Samples */
void NAISResult::setSamples(const Sample & samples)
{
  samples_ = samples;
}

/** get Auxiliary Density */
Distribution NAISResult::getAuxiliaryDensity() const
{
  return auxiliaryDistribution_;
}

/** set Auxiliary Density */
void NAISResult::setAuxiliaryDensity(const Distribution & auxiliaryDistribution)
{
  auxiliaryDistribution_ = auxiliaryDistribution;
}

/* Method save() stores the object through the StorageManager */
void NAISResult::save(Advocate & adv) const
{
  SimulationResult::save( adv );
  adv.saveAttribute("auxiliaryDistribution_", auxiliaryDistribution_);
  adv.saveAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.saveAttribute("samples_", samples_);
}

/* Method load() reloads the object from the StorageManager */
void NAISResult::load(Advocate & adv)
{
  SimulationResult::load( adv );
  adv.loadAttribute("auxiliaryDistribution_", auxiliaryDistribution_);
  adv.loadAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.loadAttribute("samples_", samples_);
}

END_NAMESPACE_OPENTURNS
