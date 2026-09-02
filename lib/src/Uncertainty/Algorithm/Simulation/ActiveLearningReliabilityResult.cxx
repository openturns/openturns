//                                               -*- C++ -*-
/**
 *  @brief Implementation of ActiveLearningReliabilityResult
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ActiveLearningReliabilityResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ActiveLearningReliabilityResult)

static const Factory<ActiveLearningReliabilityResult> Factory_ActiveLearningReliabilityResult;

/* Default constructor */
ActiveLearningReliabilityResult::ActiveLearningReliabilityResult()
{
  // Nothing to do
}

/* Standard constructor */
ActiveLearningReliabilityResult::ActiveLearningReliabilityResult(const Scalar probabilityEstimate,
                                                                 const Scalar reliabilityIndex,
                                                                 const GaussianProcessRegressionResult &gprResult,
                                                                 const ScalarCollection probabilityHistory,
                                                                 const ScalarCollection reliabilityIndexHistory, 
                                                                 const UnsignedInteger functionCallNumber, 
                                                                 const Interval probabilityCI, 
                                                                 const Interval reliabilityIndexCI)
                                                                 : probabilityEstimate_(probabilityEstimate)
                                                                 , reliabilityIndex_(reliabilityIndex)
                                                                 , gprResult_(gprResult)
                                                                 , probabilityHistory_(probabilityHistory)
                                                                 , reliabilityIndexHistory_(reliabilityIndexHistory)
                                                                 , functionCallNumber_(functionCallNumber)
                                                                 , probabilityCI_(probabilityCI)
                                                                 , reliabilityIndexCI_(reliabilityIndexCI) 
{
// Check if the probability estimate is within the range [0, 1]
  if ((probabilityEstimate < 0.0) || (probabilityEstimate > 1.0)) LOGINFO("The probability estimate should be in the range [0, 1]");
}

/* Virtual constructor */
ActiveLearningReliabilityResult * ActiveLearningReliabilityResult::clone() const
{
  return new ActiveLearningReliabilityResult(*this);
}


/* Accessors */

// Probability Estimate
Scalar ActiveLearningReliabilityResult::getProbabilityEstimate() const
{
  return probabilityEstimate_;
}

void ActiveLearningReliabilityResult::setProbabilityEstimate(const Scalar probabilityEstimate)
{
  probabilityEstimate_ = probabilityEstimate;
}

// Reliability Index
Scalar ActiveLearningReliabilityResult::getReliabilityIndex() const
{
  return reliabilityIndex_;
}

void ActiveLearningReliabilityResult::setReliabilityIndex(const Scalar reliabilityIndex)
{
  reliabilityIndex_ = reliabilityIndex;
}

// GPR Result
GaussianProcessRegressionResult ActiveLearningReliabilityResult::getGprResult() const
{
  return gprResult_;
}

void ActiveLearningReliabilityResult::setGprResult(const GaussianProcessRegressionResult & gprResult)
{
  gprResult_ = gprResult;
}

// Probability History
Collection<Scalar> ActiveLearningReliabilityResult::getProbabilityHistory() const
{
  return probabilityHistory_;
}

void ActiveLearningReliabilityResult::setProbabilityHistory(const ScalarCollection & probabilityHistory)
{
  probabilityHistory_ = probabilityHistory;
}

// Reliability Index History
Collection<Scalar> ActiveLearningReliabilityResult::getReliabilityIndexHistory() const
{
  return reliabilityIndexHistory_;
}

void ActiveLearningReliabilityResult::setReliabilityIndexHistory(const ScalarCollection & reliabilityIndexHistory)
{
  reliabilityIndexHistory_ = reliabilityIndexHistory;
}

// Function call number
UnsignedInteger ActiveLearningReliabilityResult::getFunctionCallNumber() const
{
  return functionCallNumber_;
}

void ActiveLearningReliabilityResult::setFunctionCallNumber(const UnsignedInteger functionCallNumber)
{
  functionCallNumber_ = functionCallNumber;
}

// Probability confidence interval
Interval ActiveLearningReliabilityResult::getProbabilityConfidenceInterval() const
{
  return probabilityCI_;
}

void ActiveLearningReliabilityResult::setProbabilityConfidenceInterval(const Interval probabilityCI)
{
  probabilityCI_ = probabilityCI;
}

// Reliability index confidence interval
Interval ActiveLearningReliabilityResult::getReliabilityIndexConfidenceInterval() const
{
  return reliabilityIndexCI_;
}

void ActiveLearningReliabilityResult::setReliabilityIndexConfidenceInterval(const Interval reliabilityIndexCI)
{
  reliabilityIndexCI_ = reliabilityIndexCI;
}

/* Method save() stores the object through the StorageManager */
/*void ActiveLearningReliabilityResult::save(Advocate & adv) const
{
  adv.saveAttribute("reliabilityIndex_", reliabilityIndex_);
  adv.saveAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.saveAttribute("gprResult_", gprResult_);
  adv.saveAttribute("probabilityHistory_", probabilityHistory_);
  adv.saveAttribute("reliabilityIndexHistory_", reliabilityIndexHistory_);
  adv.saveAttribute("functionCallNumber_", functionCallNumber_);
  adv.saveAttribute("probabilityCI_", probabilityCI_);
  adv.saveAttribute("reliabilityIndexCI_", reliabilityIndexCI_);  
}*/

/* Method load() reloads the object from the StorageManager */
/*void ActiveLearningReliabilityResult::load(Advocate & adv)
{
  adv.loadAttribute("reliabilityIndex_", reliabilityIndex_);
  adv.loadAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.loadAttribute("gprResult_", gprResult_);
  adv.loadAttribute("probabilityHistory_", probabilityHistory_);
  adv.loadAttribute("reliabilityIndexHistory_", reliabilityIndexHistory_);
  adv.loadAttribute("functionCallNumber_", functionCallNumber_);
  adv.loadAttribute("probabilityCI_", probabilityCI_);
  adv.loadAttribute("reliabilityIndexCI_", reliabilityIndexCI_);
}*/

END_NAMESPACE_OPENTURNS
