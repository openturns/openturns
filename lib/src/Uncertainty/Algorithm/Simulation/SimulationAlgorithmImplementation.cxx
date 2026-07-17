//                                               -*- C++ -*-
/**
 *  @brief SimulationAlgorithm implementation base class
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SimulationAlgorithmImplementation.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Compact.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class SimulationAlgorithmImplementation
 */

CLASSNAMEINIT(SimulationAlgorithmImplementation)

static const Factory<SimulationAlgorithmImplementation> Factory_SimulationAlgorithmImplementation;

/** For save/load mechanism */
SimulationAlgorithmImplementation::SimulationAlgorithmImplementation()
  : PersistentObject()
  , blockSize_(ResourceMap::GetAsUnsignedInteger( "SimulationAlgorithm-DefaultBlockSize" ))
  , progressCallback_(std::make_pair<ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<StopCallback, void *>(0, 0))
  , convergenceStrategy_(Compact())
  , maximumOuterSampling_(ResourceMap::GetAsUnsignedInteger( "SimulationAlgorithm-DefaultMaximumOuterSampling" ))
  , maximumCoefficientOfVariation_(ResourceMap::GetAsScalar( "SimulationAlgorithm-DefaultMaximumCoefficientOfVariation" ))
  , maximumStandardDeviation_(ResourceMap::GetAsScalar( "SimulationAlgorithm-DefaultMaximumStandardDeviation" ))
  , maximumTimeDuration_(ResourceMap::GetAsScalar("SimulationAlgorithm-DefaultMaximumTimeDuration"))
{
  // Nothing to do
}

/* Virtual constructor */
SimulationAlgorithmImplementation * SimulationAlgorithmImplementation::clone() const
{
  return new SimulationAlgorithmImplementation(*this);
}

/* Maximum sample size accessor */
void SimulationAlgorithmImplementation::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  maximumOuterSampling_ = maximumOuterSampling;
}

/* Maximum sample size accessor */
UnsignedInteger SimulationAlgorithmImplementation::getMaximumOuterSampling() const
{
  return maximumOuterSampling_;
}

/* Maximum coefficient of variation accessor */
void SimulationAlgorithmImplementation::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  // Check if the given coefficient of variation is >= 0
  //      if (!(maximumCoefficientOfVariation >= 0.0)) throw InvalidArgumentException(HERE) << "The maximum coefficient of variation must be >= 0.0";
  maximumCoefficientOfVariation_ = maximumCoefficientOfVariation;
}

/* Maximum coefficient of variation accessor */
Scalar SimulationAlgorithmImplementation::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}

/* Maximum standard deviation accessor */
void SimulationAlgorithmImplementation::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)
{
  maximumStandardDeviation_ = maximumStandardDeviation;
}

Scalar SimulationAlgorithmImplementation::getMaximumStandardDeviation() const
{
  return maximumStandardDeviation_;
}

/* Block size accessor */
void SimulationAlgorithmImplementation::setBlockSize(const UnsignedInteger blockSize)
{
  // Check if the given block size is >= 1
  if (blockSize < 1) throw InvalidArgumentException(HERE) << "The block size must be >= 1";
  blockSize_ = blockSize;
}

/* Block size accessor */
UnsignedInteger SimulationAlgorithmImplementation::getBlockSize() const
{
  return blockSize_;
}

/* String converter */
String SimulationAlgorithmImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << SimulationAlgorithmImplementation::GetClassName()
      << " maximumOuterSampling=" << maximumOuterSampling_
      << " maximumCoefficientOfVariation=" << maximumCoefficientOfVariation_
      << " maximumStandardDeviation=" << maximumStandardDeviation_
      << " blockSize=" << blockSize_;
  return oss;
}

/* Performs the actual computation. */
void SimulationAlgorithmImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In SimulationAlgorithmImplementation::run()";
}

/* Comparison operator */
Bool SimulationAlgorithmImplementation::operator ==(const PersistentObject & other) const
{
  const auto * p_other = dynamic_cast<const SimulationAlgorithmImplementation *>(&other);
  if (p_other == nullptr) return false;
  if (this == p_other) return true;
  return (maximumOuterSampling_ == p_other->maximumOuterSampling_)
      && (maximumCoefficientOfVariation_ == p_other->maximumCoefficientOfVariation_)
      && (maximumStandardDeviation_ == p_other->maximumStandardDeviation_)
      && (maximumTimeDuration_ == p_other->maximumTimeDuration_)
      && (blockSize_ == p_other->blockSize_);
}

/* Comparison operator */
Bool SimulationAlgorithmImplementation::operator !=(const PersistentObject & other) const
{
  return !operator==(other);
}

/* Convergence strategy accessor */
void SimulationAlgorithmImplementation::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)
{
  convergenceStrategy_ = convergenceStrategy;
}

HistoryStrategy SimulationAlgorithmImplementation::getConvergenceStrategy() const
{
  return convergenceStrategy_;
}


/* Method save() stores the object through the StorageManager */
void SimulationAlgorithmImplementation::save(Advocate & adv) const
{

  PersistentObject::save(adv);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.saveAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.saveAttribute("maximumStandardDeviation_", maximumStandardDeviation_);
  adv.saveAttribute("maximumTimeDuration_", maximumTimeDuration_);
  adv.saveAttribute("convergenceStrategy_", convergenceStrategy_);
}

/* Method load() reloads the object from the StorageManager */
void SimulationAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.loadAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.loadAttribute("maximumStandardDeviation_", maximumStandardDeviation_);
  if (adv.hasAttribute("maximumTimeDuration_")) // OT>=1.23
    adv.loadAttribute("maximumTimeDuration_", maximumTimeDuration_);
  adv.loadAttribute("convergenceStrategy_", convergenceStrategy_);
}


void SimulationAlgorithmImplementation::setProgressCallback(ProgressCallback callBack, void * state)
{
  progressCallback_ = std::pair<ProgressCallback, void *>(callBack, state);
}


void SimulationAlgorithmImplementation::setStopCallback(StopCallback callBack, void * state)
{
  stopCallback_ = std::pair<StopCallback, void *>(callBack, state);
}

/* Maximum time accessor */
void SimulationAlgorithmImplementation::setMaximumTimeDuration(const Scalar maximumTimeDuration)
{
  maximumTimeDuration_ = maximumTimeDuration;
}

Scalar SimulationAlgorithmImplementation::getMaximumTimeDuration() const
{
  return maximumTimeDuration_;
}

END_NAMESPACE_OPENTURNS
