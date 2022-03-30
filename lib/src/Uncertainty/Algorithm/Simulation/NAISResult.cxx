//                                               -*- C++ -*-
/**
 *  @brief class for NAISResult
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

static const Factory<NAISResult> Factory_NAISResult;

/* Default constructor */
NAISResult::NAISResult()
  : ProbabilitySimulationResult()
{
  //Nothing TO DO
}

/* Virtual constructor */
NAISResult * NAISResult::clone() const
{
  return new NAISResult(*this);
}

// Get Input Samples
Sample NAISResult::getAuxiliaryInputSample() const
{
  return auxiliaryInputSample_;
}

// Set Input Samples
void NAISResult::setAuxiliaryInputSample(const Sample & auxiliaryInputSample)
{
  auxiliaryInputSample_ = auxiliaryInputSample;
}

// Get Output Samples
Sample NAISResult::getAuxiliaryOutputSample() const
{
  return auxiliaryOutputSample_;
}

// Set Output Samples
void NAISResult::setAuxiliaryOutputSample(const Sample & auxiliaryOutputSample)
{
  auxiliaryOutputSample_ = auxiliaryOutputSample;
}

// Get Auxiliary Density
Distribution NAISResult::getAuxiliaryDistribution() const
{
  return auxiliaryDistribution_;
}

// Set Auxiliary Density
void NAISResult::setAuxiliaryDistribution(const Distribution & auxiliaryDistribution)
{
  auxiliaryDistribution_ = auxiliaryDistribution;
}

// Coefficient of variation estimate accessor 
Scalar NAISResult::getCoefficientOfVariation() const
{
  return sqrt(varianceEstimate_)/probabilityEstimate_;
}

// Method save() stores the object through the StorageManager
void NAISResult::save(Advocate & adv) const
{
  ProbabilitySimulationResult::save(adv);
  adv.saveAttribute("auxiliaryDistribution_", auxiliaryDistribution_);
  adv.saveAttribute("sample_", auxiliaryInputSample_);
}

// Method load() reloads the object from the StorageManager
void NAISResult::load(Advocate & adv)
{
  ProbabilitySimulationResult::load(adv);
  adv.loadAttribute("auxiliaryDistribution_", auxiliaryDistribution_);
  adv.loadAttribute("sample_", auxiliaryInputSample_);
}



END_NAMESPACE_OPENTURNS
