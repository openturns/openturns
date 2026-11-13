//                                               -*- C++ -*-
/**
 *  @brief Handle of quantile simulation algorithms
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
#include "openturns/QuantileSimulation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuantileSimulation)

/* Default constructor */
QuantileSimulation::QuantileSimulation()
  : TypedInterfaceObject<QuantileSimulationImplementation>(new QuantileSimulationImplementation())
{
  // Nothing to do
}

/* Standard constructor */
QuantileSimulation::QuantileSimulation(const RandomVector & output,
                                       const Scalar alpha,
                                       const Scalar beta)
  : TypedInterfaceObject<QuantileSimulationImplementation>(new QuantileSimulationImplementation(output, alpha, beta))
{
  // Nothing to do
}

/* Constructor from implementation */
QuantileSimulation::QuantileSimulation(const QuantileSimulationImplementation & implementation)
  : TypedInterfaceObject<QuantileSimulationImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
QuantileSimulation::QuantileSimulation(const Implementation & p_implementation)
  : TypedInterfaceObject<QuantileSimulationImplementation>(p_implementation)
{
  // Nothing to do
}

/* Output random vector accessor */
void QuantileSimulation::setOutput(const RandomVector & output)
{
  copyOnWrite();
  getImplementation()->setOutput(output);
}

RandomVector QuantileSimulation::getOutput() const
{
  return getImplementation()->getOutput();
}

/* Quantile level accessor */
void QuantileSimulation::setAlpha(const Scalar alpha)
{
  copyOnWrite();
  getImplementation()->setAlpha(alpha);
}

Scalar QuantileSimulation::getAlpha() const
{
  return getImplementation()->getAlpha();
}

/* Confidence level accessor */
void QuantileSimulation::setConfidenceLevel(const Scalar beta)
{
  copyOnWrite();
  getImplementation()->setConfidenceLevel(beta);
}

Scalar QuantileSimulation::getConfidenceLevel() const
{
  return getImplementation()->getConfidenceLevel();
}

/* Sample size accessor */
void QuantileSimulation::setSampleSize(const UnsignedInteger sampleSize)
{
  copyOnWrite();
  getImplementation()->setSampleSize(sampleSize);
}

UnsignedInteger QuantileSimulation::getSampleSize() const
{
  return getImplementation()->getSampleSize();
}

/* Estimator type accessor */
void QuantileSimulation::setEstimatorType(const QuantileSimulationResultImplementation::EstimatorType type)
{
  copyOnWrite();
  getImplementation()->setEstimatorType(type);
}

QuantileSimulationResultImplementation::EstimatorType QuantileSimulation::getEstimatorType() const
{
  return getImplementation()->getEstimatorType();
}

/* Required coverage accessor */
void QuantileSimulation::setRequiredCoverage(const QuantileSimulationResultImplementation::CoverageType type)
{
  copyOnWrite();
  getImplementation()->setRequiredCoverage(type);
}

QuantileSimulationResultImplementation::CoverageType QuantileSimulation::getRequiredCoverage() const
{
  return getImplementation()->getRequiredCoverage();
}

/* Performs the actual computation */
void QuantileSimulation::run()
{
  copyOnWrite();
  getImplementation()->run();
}

/* Result accessor */
QuantileSimulationResult QuantileSimulation::getResult() const
{
  return getImplementation()->getResult();
}

/* Maximum outer sampling accessor */
void QuantileSimulation::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  copyOnWrite();
  getImplementation()->setMaximumOuterSampling(maximumOuterSampling);
}

UnsignedInteger QuantileSimulation::getMaximumOuterSampling() const
{
  return getImplementation()->getMaximumOuterSampling();
}

/* Maximum coefficient of variation accessor */
void QuantileSimulation::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  copyOnWrite();
  getImplementation()->setMaximumCoefficientOfVariation(maximumCoefficientOfVariation);
}

Scalar QuantileSimulation::getMaximumCoefficientOfVariation() const
{
  return getImplementation()->getMaximumCoefficientOfVariation();
}

/* Maximum standard deviation accessor */
void QuantileSimulation::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)
{
  copyOnWrite();
  getImplementation()->setMaximumStandardDeviation(maximumStandardDeviation);
}

Scalar QuantileSimulation::getMaximumStandardDeviation() const
{
  return getImplementation()->getMaximumStandardDeviation();
}

/* Block size accessor */
void QuantileSimulation::setBlockSize(const UnsignedInteger blockSize)
{
  copyOnWrite();
  getImplementation()->setBlockSize(blockSize);
}

UnsignedInteger QuantileSimulation::getBlockSize() const
{
  return getImplementation()->getBlockSize();
}

/* Maximum time duration accessor */
void QuantileSimulation::setMaximumTimeDuration(const Scalar maximumTimeDuration)
{
  copyOnWrite();
  getImplementation()->setMaximumTimeDuration(maximumTimeDuration);
}

Scalar QuantileSimulation::getMaximumTimeDuration() const
{
  return getImplementation()->getMaximumTimeDuration();
}

/* Convergence strategy accessor */
void QuantileSimulation::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)
{
  copyOnWrite();
  getImplementation()->setConvergenceStrategy(convergenceStrategy);
}

HistoryStrategy QuantileSimulation::getConvergenceStrategy() const
{
  return getImplementation()->getConvergenceStrategy();
}

/* Progress callback */
void QuantileSimulation::setProgressCallback(ProgressCallback callBack, void * state)
{
  copyOnWrite();
  getImplementation()->setProgressCallback(callBack, state);
}

/* Stop callback */
void QuantileSimulation::setStopCallback(StopCallback callBack, void * state)
{
  copyOnWrite();
  getImplementation()->setStopCallback(callBack, state);
}

/* String converter */
String QuantileSimulation::__repr__() const
{
  return getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
