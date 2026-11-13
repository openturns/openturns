//                                               -*- C++ -*-
/**
 *  @brief Handle of quantile simulation results
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
#include "openturns/QuantileSimulationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuantileSimulationResult)

/* Default constructor */
QuantileSimulationResult::QuantileSimulationResult()
  : TypedInterfaceObject<QuantileSimulationResultImplementation>(new QuantileSimulationResultImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
QuantileSimulationResult::QuantileSimulationResult(const QuantileSimulationResultImplementation & result)
  : TypedInterfaceObject<QuantileSimulationResultImplementation>(result.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
QuantileSimulationResult::QuantileSimulationResult(const Implementation & p_implementation)
  : TypedInterfaceObject<QuantileSimulationResultImplementation>(p_implementation)
{
  // Nothing to do
}

/* Output random vector accessor */
void QuantileSimulationResult::setOutput(const RandomVector & output)
{
  copyOnWrite();
  getImplementation()->setOutput(output);
}

RandomVector QuantileSimulationResult::getOutput() const
{
  return getImplementation()->getOutput();
}

/* Quantile level accessor */
void QuantileSimulationResult::setQuantileLevel(const Scalar alpha)
{
  copyOnWrite();
  getImplementation()->setQuantileLevel(alpha);
}

Scalar QuantileSimulationResult::getQuantileLevel() const
{
  return getImplementation()->getQuantileLevel();
}

/* Confidence level accessor */
void QuantileSimulationResult::setConfidenceLevel(const Scalar beta)
{
  copyOnWrite();
  getImplementation()->setConfidenceLevel(beta);
}

Scalar QuantileSimulationResult::getConfidenceLevel() const
{
  return getImplementation()->getConfidenceLevel();
}

/* Master seed accessor */
void QuantileSimulationResult::setMasterSeed(const UnsignedInteger seed)
{
  copyOnWrite();
  getImplementation()->setMasterSeed(seed);
}

UnsignedInteger QuantileSimulationResult::getMasterSeed() const
{
  return getImplementation()->getMasterSeed();
}

Bool QuantileSimulationResult::hasMasterSeed() const
{
  return getImplementation()->hasMasterSeed();
}

/* Quantile estimate accessor */
void QuantileSimulationResult::setQuantileEstimate(const Scalar estimate)
{
  copyOnWrite();
  getImplementation()->setQuantileEstimate(estimate);
}

Scalar QuantileSimulationResult::getQuantileEstimate() const
{
  return getImplementation()->getQuantileEstimate();
}

/* Standard error accessor */
void QuantileSimulationResult::setStandardError(const Scalar standardError)
{
  copyOnWrite();
  getImplementation()->setStandardError(standardError);
}

Scalar QuantileSimulationResult::getStandardError() const
{
  return getImplementation()->getStandardError();
}

/* Confidence interval accessor */
void QuantileSimulationResult::setConfidenceInterval(const Interval & interval)
{
  copyOnWrite();
  getImplementation()->setConfidenceInterval(interval);
}

Interval QuantileSimulationResult::getConfidenceInterval() const
{
  return getImplementation()->getConfidenceInterval();
}

/* Estimator type accessor */
void QuantileSimulationResult::setEstimatorType(const QuantileSimulationResultImplementation::EstimatorType type)
{
  copyOnWrite();
  getImplementation()->setEstimatorType(type);
}

QuantileSimulationResultImplementation::EstimatorType QuantileSimulationResult::getEstimatorType() const
{
  return getImplementation()->getEstimatorType();
}

/* Achieved coverage accessor */
void QuantileSimulationResult::setCoverageType(const QuantileSimulationResultImplementation::CoverageType type)
{
  copyOnWrite();
  getImplementation()->setCoverageType(type);
}

QuantileSimulationResultImplementation::CoverageType QuantileSimulationResult::getCoverageType() const
{
  return getImplementation()->getCoverageType();
}

/* Required coverage accessor */
void QuantileSimulationResult::setRequiredCoverage(const QuantileSimulationResultImplementation::CoverageType type)
{
  copyOnWrite();
  getImplementation()->setRequiredCoverage(type);
}

QuantileSimulationResultImplementation::CoverageType QuantileSimulationResult::getRequiredCoverage() const
{
  return getImplementation()->getRequiredCoverage();
}

/* Effective sample size accessor */
void QuantileSimulationResult::setEffectiveSampleSize(const Scalar ess)
{
  copyOnWrite();
  getImplementation()->setEffectiveSampleSize(ess);
}

Scalar QuantileSimulationResult::getEffectiveSampleSize() const
{
  return getImplementation()->getEffectiveSampleSize();
}

/* Sample size accessor */
void QuantileSimulationResult::setSampleSize(const UnsignedInteger sampleSize)
{
  copyOnWrite();
  getImplementation()->setSampleSize(sampleSize);
}

UnsignedInteger QuantileSimulationResult::getSampleSize() const
{
  return getImplementation()->getSampleSize();
}

/* Statistical block length accessor */
void QuantileSimulationResult::setBlockLength(const UnsignedInteger blockLength)
{
  copyOnWrite();
  getImplementation()->setBlockLength(blockLength);
}

UnsignedInteger QuantileSimulationResult::getBlockLength() const
{
  return getImplementation()->getBlockLength();
}

/* Bias estimate accessor */
void QuantileSimulationResult::setBiasEstimate(const Scalar bias)
{
  copyOnWrite();
  getImplementation()->setBiasEstimate(bias);
}

Scalar QuantileSimulationResult::getBiasEstimate() const
{
  return getImplementation()->getBiasEstimate();
}

Bool QuantileSimulationResult::hasBiasEstimate() const
{
  return getImplementation()->hasBiasEstimate();
}

/* Fixed sample flag accessor */
void QuantileSimulationResult::setIsFixedSample(const Bool fixedSample)
{
  copyOnWrite();
  getImplementation()->setIsFixedSample(fixedSample);
}

Bool QuantileSimulationResult::isFixedSample() const
{
  return getImplementation()->isFixedSample();
}

/* Convergence history accessor */
void QuantileSimulationResult::setConvergenceHistory(const Sample & history)
{
  copyOnWrite();
  getImplementation()->setConvergenceHistory(history);
}

Sample QuantileSimulationResult::getConvergenceHistory() const
{
  return getImplementation()->getConvergenceHistory();
}

/* Outer sampling accessor */
void QuantileSimulationResult::setOuterSampling(const UnsignedInteger outerSampling)
{
  copyOnWrite();
  getImplementation()->setOuterSampling(outerSampling);
}

UnsignedInteger QuantileSimulationResult::getOuterSampling() const
{
  return getImplementation()->getOuterSampling();
}

/* Evaluation block size accessor */
void QuantileSimulationResult::setBlockSize(const UnsignedInteger blockSize)
{
  copyOnWrite();
  getImplementation()->setBlockSize(blockSize);
}

UnsignedInteger QuantileSimulationResult::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

/* Elapsed time accessor */
void QuantileSimulationResult::setTimeDuration(const Scalar time)
{
  copyOnWrite();
  getImplementation()->setTimeDuration(time);
}

Scalar QuantileSimulationResult::getTimeDuration() const
{
  return getImplementation()->getTimeDuration();
}

/* Estimator and coverage descriptions */
String QuantileSimulationResult::getEstimatorDescription() const
{
  return getImplementation()->getEstimatorDescription();
}

String QuantileSimulationResult::getCoverageDescription() const
{
  return getImplementation()->getCoverageDescription();
}

/* Draw the quantile convergence */
Graph QuantileSimulationResult::drawQuantileConvergence() const
{
  return getImplementation()->drawQuantileConvergence();
}

/* String converter */
String QuantileSimulationResult::__repr__() const
{
  return getImplementation()->__repr__();
}

String QuantileSimulationResult::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
