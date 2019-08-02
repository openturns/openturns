//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/ProbabilitySimulationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SimulationSensitivityAnalysis.hxx"
#include "openturns/Log.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProbabilitySimulationResult)

static const Factory<ProbabilitySimulationResult> Factory_ProbabilitySimulationResult;

/* Default constructor */
ProbabilitySimulationResult::ProbabilitySimulationResult()
  : SimulationResult()
  , event_()
  , probabilityEstimate_(0.0)
  , varianceEstimate_(0.0)
{
  // Nothing to do
}

/* Standard constructor */
ProbabilitySimulationResult::ProbabilitySimulationResult(const RandomVector & event,
    const Scalar probabilityEstimate,
    const Scalar varianceEstimate,
    const UnsignedInteger outerSampling,
    const UnsignedInteger blockSize)
  : SimulationResult(outerSampling, blockSize)
  , event_(event)
  , probabilityEstimate_(probabilityEstimate)
  , varianceEstimate_(varianceEstimate)
{
  // Check if the probability estimate is within the range [0, 1]
  if ((probabilityEstimate < 0.0) || (probabilityEstimate > 1.0)) LOGINFO("The probability estimate should be in the range [0, 1]");
  // Check if the variance estimate is >= 0.0
  if (!(varianceEstimate >= 0.0)) throw InvalidArgumentException(HERE) << "The variance estimate must be >= 0";
}


/* Virtual constructor */
ProbabilitySimulationResult * ProbabilitySimulationResult::clone() const
{
  return new ProbabilitySimulationResult(*this);
}

/* Event accessor */
RandomVector ProbabilitySimulationResult::getEvent() const
{
  return event_;
}

void ProbabilitySimulationResult::setEvent(const RandomVector & event)
{
  event_ = event;
}

/* Probability estimate accessor */
Scalar ProbabilitySimulationResult::getProbabilityEstimate() const
{
  return probabilityEstimate_;
}

void ProbabilitySimulationResult::setProbabilityEstimate(const Scalar probabilityEstimate)
{
  probabilityEstimate_ = probabilityEstimate;
}

/* Variance estimate accessor */
Scalar ProbabilitySimulationResult::getVarianceEstimate() const
{
  return varianceEstimate_;
}

void ProbabilitySimulationResult::setVarianceEstimate(const Scalar varianceEstimate)
{
  varianceEstimate_ = varianceEstimate;
}

/* Probability estimate distribution accessor */
Normal ProbabilitySimulationResult::getProbabilityDistribution() const
{
  return Normal(probabilityEstimate_, std::sqrt(varianceEstimate_));
}

/* Coefficient of variation estimate accessor */
Scalar ProbabilitySimulationResult::getCoefficientOfVariation() const
{
  // The usual case: the variance estimate is > 0.0 and the probability estimate is in ]0,1] or the user accept a zero variance
  if ((varianceEstimate_ > 0.0) && (probabilityEstimate_ > 0.0) && (probabilityEstimate_ <= 1.0)) return sqrt(varianceEstimate_) / probabilityEstimate_;
  // when covarianceEstimate_ == 0.0 and 0.0 < probabilityEstimate_ < 1.0
  const Bool checkPositiveVariance = ResourceMap::GetAsBool("ProbabilitySimulationResult-CheckPositiveVariance");
  if (!checkPositiveVariance && (probabilityEstimate_ > 0.0) && (probabilityEstimate_ < 1.0) && (outerSampling_ * blockSize_ > 1)) return sqrt(varianceEstimate_) / probabilityEstimate_;
  // In all the other cases, return -1.0, waiting for a better strategy
  return -1.0;
}

/* Standard deviation estimate accessor */
Scalar ProbabilitySimulationResult::getStandardDeviation() const
{
  // The usual case: the variance estimate is > 0.0 or the user accept a zero variance
  if (varianceEstimate_ > 0.0) return sqrt(varianceEstimate_);
  // when covarianceEstimate_ == 0.0 and 0.0 < probabilityEstimate_ < 1.0
  const Bool checkPositiveVariance = ResourceMap::GetAsBool("ProbabilitySimulationResult-CheckPositiveVariance");
  if (!checkPositiveVariance && (probabilityEstimate_ > 0.0) && (probabilityEstimate_ < 1.0) && (outerSampling_ * blockSize_ > 1)) return sqrt(varianceEstimate_);
  // In all the other cases, return -1.0, waiting for a better strategy
  return -1.0;
}

/* String converter */
String ProbabilitySimulationResult::__repr__() const
{
  const Scalar defaultConfidenceLevel = ResourceMap::GetAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel");
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
Scalar ProbabilitySimulationResult::getConfidenceLength(const Scalar level) const
{
  // Check if the given level is in ]0, 1[
  if ((level <= 0.0) || (level >= 1.0)) throw InvalidArgumentException(HERE) << "Confidence level must be in ]0, 1[";
  // The probability estimate is asymptotically normal
  const Scalar xq = DistFunc::qNormal(0.5 + 0.5 * level);
  return 2.0 * xq * sqrt(varianceEstimate_);
}

/* Mean point conditioned to the event realization accessor */
Point ProbabilitySimulationResult::getMeanPointInEventDomain() const
{
  return SimulationSensitivityAnalysis(event_).computeMeanPointInEventDomain();
}

/* Get the importance factors based on the mean point in the event domain. The mean point is transformed into the standard space, then the importance factors are obtained as the normalized squared cosine directors. */
PointWithDescription ProbabilitySimulationResult::getImportanceFactors() const
{
  return SimulationSensitivityAnalysis(event_).computeImportanceFactors();
}

/* ImportanceFactors graph */
Graph ProbabilitySimulationResult::drawImportanceFactors() const
{
  return SimulationSensitivityAnalysis(event_).drawImportanceFactors();
}

/* Method save() stores the object through the StorageManager */
void ProbabilitySimulationResult::save(Advocate & adv) const
{
  SimulationResult::save(adv);
  adv.saveAttribute("event_", event_);
  adv.saveAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.saveAttribute("varianceEstimate_", varianceEstimate_);
}

/* Method load() reloads the object from the StorageManager */
void ProbabilitySimulationResult::load(Advocate & adv)
{
  SimulationResult::load(adv);
  adv.loadAttribute("event_", event_);
  adv.loadAttribute("probabilityEstimate_", probabilityEstimate_);
  adv.loadAttribute("varianceEstimate_", varianceEstimate_);
}

END_NAMESPACE_OPENTURNS
