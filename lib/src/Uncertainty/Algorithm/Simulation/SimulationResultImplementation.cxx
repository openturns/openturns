//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
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
#include <cstdlib>

#include "openturns/SimulationResultImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SimulationSensitivityAnalysis.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SimulationResultImplementation);

static const Factory<SimulationResultImplementation> RegisteredFactory;

/* Default constructor */
SimulationResultImplementation::SimulationResultImplementation()
  : PersistentObject()
  , event_()
  , probabilityEstimate_(0.0)
  , varianceEstimate_(0.0)
  , outerSampling_(0)
  , blockSize_(0)
{
  // Nothing to do
}

/* Standard constructor */
SimulationResultImplementation::SimulationResultImplementation(const Event & event,
    const NumericalScalar probabilityEstimate,
    const NumericalScalar varianceEstimate,
    const UnsignedInteger outerSampling,
    const UnsignedInteger blockSize)
  : PersistentObject()
  , event_(event)
  , probabilityEstimate_(probabilityEstimate)
  , varianceEstimate_(varianceEstimate)
  , outerSampling_(outerSampling)
  , blockSize_(blockSize)
{
  // Check if the probability estimate is within the range [0, 1]
  if ((probabilityEstimate < 0) || (probabilityEstimate > 1)) LOGINFO("The probability estimate should be in the range [0, 1]");
  // Check if the variance estimate is >= 0.0
  if (varianceEstimate < 0.0) throw InvalidArgumentException(HERE) << "The variance estimate must be >= 0";
}

/* Virtual constructor */
SimulationResultImplementation * SimulationResultImplementation::clone() const
{
  return new SimulationResultImplementation(*this);
}

/* Event accessor */
Event SimulationResultImplementation::getEvent() const
{
  return event_;
}

void SimulationResultImplementation::setEvent(const Event & event)
{
  event_ = event;
}

/* Probability estimate accessor */
NumericalScalar SimulationResultImplementation::getProbabilityEstimate() const
{
  return probabilityEstimate_;
}

void SimulationResultImplementation::setProbabilityEstimate(const NumericalScalar probabilityEstimate)
{
  probabilityEstimate_ = probabilityEstimate;
}

/* Variance estimate accessor */
NumericalScalar SimulationResultImplementation::getVarianceEstimate() const
{
  return varianceEstimate_;
}

void SimulationResultImplementation::setVarianceEstimate(const NumericalScalar varianceEstimate)
{
  varianceEstimate_ = varianceEstimate;
}

/* Coefficient of variation estimate accessor */
NumericalScalar SimulationResultImplementation::getCoefficientOfVariation() const
{
  // The usual case: the variance estimate is > 0.0 and the probability estimate is in ]0,1]
  if ((varianceEstimate_ > 0.0) && (probabilityEstimate_ > 0.0) && (probabilityEstimate_ <= 1.0)) return sqrt(varianceEstimate_) / probabilityEstimate_;
  // In all the other cases, return -1.0, waiting for a better strategy
  // when covarianceEstimate_ == 0.0 and probabilityEstimate_ > 0.0
  return -1.0;
}

/* Standard deviation estimate accessor */
NumericalScalar SimulationResultImplementation::getStandardDeviation() const
{
  // The usual case: the variance estimate is > 0.0
  if (varianceEstimate_ > 0.0) return sqrt(varianceEstimate_);
  // In all the other cases, return -1.0, waiting for a better strategy
  // when covarianceEstimate_ == 0.0 and probabilityEstimate_ > 0.0
  return -1.0;
}

/* Outer sampling accessor */
UnsignedInteger SimulationResultImplementation::getOuterSampling() const
{
  return outerSampling_;
}

void SimulationResultImplementation::setOuterSampling(const UnsignedInteger outerSampling)
{
  outerSampling_ = outerSampling;
}

/* Block size accessor */
UnsignedInteger SimulationResultImplementation::getBlockSize() const
{
  return blockSize_;
}

void SimulationResultImplementation::setBlockSize(const UnsignedInteger blockSize)
{
  blockSize_ = blockSize;
}

/* String converter */
String SimulationResultImplementation::__repr__() const
{
  const NumericalScalar defaultConfidenceLevel(ResourceMap::GetAsNumericalScalar( "SimulationResultImplementation-DefaultConfidenceLevel" ));
  OSS oss;
  oss.setPrecision(6);
  oss << std::scientific
      << "probabilityEstimate=" << probabilityEstimate_
      << " varianceEstimate=" << varianceEstimate_;

  oss.setPrecision(2)
      << " standard deviation=" << getStandardDeviation()
      << " coefficient of variation=" << getCoefficientOfVariation()
      << std::fixed
      << " confidenceLength(" << defaultConfidenceLevel << ")=" << std::scientific << getConfidenceLength(defaultConfidenceLevel) << std::fixed
      << " outerSampling=" << outerSampling_
      << " blockSize=" << blockSize_;

  return oss;
}

/* Confidence length */
NumericalScalar SimulationResultImplementation::getConfidenceLength(const NumericalScalar level) const
{
  // Check if the given level is in ]0, 1[
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Confidence level must be in ]0, 1[";
  // The probability estimate is asymptotically normal
  const NumericalScalar xq(DistFunc::qNormal(0.5 + 0.5 * level));
  return 2.0 * xq * sqrt(varianceEstimate_);
}

/* Mean point conditioned to the event realization accessor */
NumericalPoint SimulationResultImplementation::getMeanPointInEventDomain() const
{
  return SimulationSensitivityAnalysis(event_).computeMeanPointInEventDomain();
}

/* Get the importance factors based on the mean point in the event domain. The mean point is transformed into the standard space, then the importance factors are obtained as the normalized squared cosine directors. */
NumericalPointWithDescription SimulationResultImplementation::getImportanceFactors() const
{
  return SimulationSensitivityAnalysis(event_).computeImportanceFactors();
}

/* ImportanceFactors graph */
Graph SimulationResultImplementation::drawImportanceFactors() const
{
  return SimulationSensitivityAnalysis(event_).drawImportanceFactors();
}

/* Method save() stores the object through the StorageManager */
void SimulationResultImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("event_", event_);
  adv.saveAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.saveAttribute("varianceEstimate_", varianceEstimate_);
  adv.saveAttribute("outerSampling_", outerSampling_);
  adv.saveAttribute("blockSize_", blockSize_);
}

/* Method load() reloads the object from the StorageManager */
void SimulationResultImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("event_", event_);
  adv.loadAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.loadAttribute("varianceEstimate_", varianceEstimate_);
  adv.loadAttribute("outerSampling_", outerSampling_);
  adv.loadAttribute("blockSize_", blockSize_);
}

END_NAMESPACE_OPENTURNS
