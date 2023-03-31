//                                               -*- C++ -*-
/**
 *  @brief class for CrossEntropyResult
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CrossEntropyResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CrossEntropyResult
 */


CLASSNAMEINIT(CrossEntropyResult);

static const Factory<CrossEntropyResult> Factory_CrossEntropyResult;

/* Default constructor */
CrossEntropyResult::CrossEntropyResult()
  : ProbabilitySimulationResult()
{
  //Nothing TO DO
}

/* Virtual constructor */
CrossEntropyResult * CrossEntropyResult::clone() const
{
  return new CrossEntropyResult(*this);
}

// Get Input Samples
Sample CrossEntropyResult::getAuxiliaryInputSample() const
{
  return auxiliaryInputSample_;
}

// Set Input Samples
void CrossEntropyResult::setAuxiliaryInputSample(const Sample & auxiliaryInputSample)
{
  auxiliaryInputSample_ = auxiliaryInputSample;
}



// Get Output Samples
Sample CrossEntropyResult::getAuxiliaryOutputSample() const
{
  return auxiliaryOutputSample_;
}

// Set Output Samples
void CrossEntropyResult::setAuxiliaryOutputSample(const Sample & auxiliaryOutputSample)
{
  auxiliaryOutputSample_ = auxiliaryOutputSample;
}

// Get Auxiliary Density
Distribution CrossEntropyResult::getAuxiliaryDistribution() const
{
  return auxiliaryDistribution_;
}

// Set Auxiliary Density
void CrossEntropyResult::setAuxiliaryDistribution(const Distribution & auxiliaryDistribution)
{
  auxiliaryDistribution_ = auxiliaryDistribution;
}

// Coefficient of variation estimate accessor
Scalar CrossEntropyResult::getCoefficientOfVariation() const
{
  return sqrt(varianceEstimate_) / probabilityEstimate_;
}

// Method save() stores the object through the StorageManager
void CrossEntropyResult::save(Advocate & adv) const
{
  ProbabilitySimulationResult::save(adv);
  adv.saveAttribute("auxiliaryDistribution_", auxiliaryDistribution_);
  adv.saveAttribute("sample_", auxiliaryInputSample_);
}

// Method load() reloads the object from the StorageManager
void CrossEntropyResult::load(Advocate & adv)
{
  ProbabilitySimulationResult::load(adv);
  adv.loadAttribute("auxiliaryDistribution_", auxiliaryDistribution_);
  adv.loadAttribute("sample_", auxiliaryInputSample_);
}



END_NAMESPACE_OPENTURNS
