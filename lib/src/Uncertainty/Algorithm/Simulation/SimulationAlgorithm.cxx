//                                               -*- C++ -*-
/**
 *  @brief SimulationAlgorithm algorithms base class
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Compact.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class SimulationAlgorithm
 */

CLASSNAMEINIT(SimulationAlgorithm)

static const Factory<SimulationAlgorithm> Factory_SimulationAlgorithm;

/** For save/load mechanism */
SimulationAlgorithm::SimulationAlgorithm()
  : PersistentObject()
  , blockSize_(ResourceMap::GetAsUnsignedInteger( "SimulationAlgorithm-DefaultBlockSize" ))
  , progressCallback_(std::make_pair<ProgressCallback, void *>(0, 0))
  , stopCallback_(std::make_pair<StopCallback, void *>(0, 0))
  , convergenceStrategy_(Compact())
  , maximumOuterSampling_(ResourceMap::GetAsUnsignedInteger( "SimulationAlgorithm-DefaultMaximumOuterSampling" ))
  , maximumCoefficientOfVariation_(ResourceMap::GetAsScalar( "SimulationAlgorithm-DefaultMaximumCoefficientOfVariation" ))
  , maximumStandardDeviation_(ResourceMap::GetAsScalar( "SimulationAlgorithm-DefaultMaximumStandardDeviation" ))
  , verbose_(false)
{
  // Nothing to do
}

/* Virtual constructor */
SimulationAlgorithm * SimulationAlgorithm::clone() const
{
  return new SimulationAlgorithm(*this);
}

/* Maximum sample size accessor */
void SimulationAlgorithm::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  maximumOuterSampling_ = maximumOuterSampling;
}

/* Maximum sample size accessor */
UnsignedInteger SimulationAlgorithm::getMaximumOuterSampling() const
{
  return maximumOuterSampling_;
}

/* Maximum coefficient of variation accessor */
void SimulationAlgorithm::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  // Check if the given coefficient of variation is >= 0
  //      if (!(maximumCoefficientOfVariation >= 0.0)) throw InvalidArgumentException(HERE) << "The maximum coefficient of variation must be >= 0.0";
  maximumCoefficientOfVariation_ = maximumCoefficientOfVariation;
}

/* Maximum coefficient of variation accessor */
Scalar SimulationAlgorithm::getMaximumCoefficientOfVariation() const
{
  return maximumCoefficientOfVariation_;
}

/* Maximum standard deviation accessor */
void SimulationAlgorithm::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)
{
  maximumStandardDeviation_ = maximumStandardDeviation;
}

Scalar SimulationAlgorithm::getMaximumStandardDeviation() const
{
  return maximumStandardDeviation_;
}

/* Block size accessor */
void SimulationAlgorithm::setBlockSize(const UnsignedInteger blockSize)
{
  // Check if the given block size is >= 1
  if (blockSize < 1) throw InvalidArgumentException(HERE) << "The block size must be >= 1";
  blockSize_ = blockSize;
}

/* Block size accessor */
UnsignedInteger SimulationAlgorithm::getBlockSize() const
{
  return blockSize_;
}

/* Verbosity accessor */
void SimulationAlgorithm::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

/* Verbosity accessor */
Bool SimulationAlgorithm::getVerbose() const
{
  return verbose_;
}

/* String converter */
String SimulationAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << SimulationAlgorithm::GetClassName()
      << " maximumOuterSampling=" << maximumOuterSampling_
      << " maximumCoefficientOfVariation=" << maximumCoefficientOfVariation_
      << " maximumStandardDeviation=" << maximumStandardDeviation_
      << " blockSize=" << blockSize_;
  return oss;
}

/* Performs the actual computation. */
void SimulationAlgorithm::run()
{
  throw NotYetImplementedException(HERE) << "In SimulationAlgorithm::run()";
}

/* Convergence strategy accessor */
void SimulationAlgorithm::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)
{
  convergenceStrategy_ = convergenceStrategy;
}

HistoryStrategy SimulationAlgorithm::getConvergenceStrategy() const
{
  return convergenceStrategy_;
}


/* Method save() stores the object through the StorageManager */
void SimulationAlgorithm::save(Advocate & adv) const
{

  PersistentObject::save(adv);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.saveAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.saveAttribute("maximumStandardDeviation_", maximumStandardDeviation_);
  adv.saveAttribute("convergenceStrategy_", convergenceStrategy_);
  adv.saveAttribute("verbose_", verbose_);
}

/* Method load() reloads the object from the StorageManager */
void SimulationAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("blockSize_", blockSize_);
  adv.loadAttribute("maximumOuterSampling_", maximumOuterSampling_);
  adv.loadAttribute("maximumCoefficientOfVariation_", maximumCoefficientOfVariation_);
  adv.loadAttribute("maximumStandardDeviation_", maximumStandardDeviation_);
  adv.loadAttribute("convergenceStrategy_", convergenceStrategy_);
  adv.loadAttribute("verbose_", verbose_);
}


void SimulationAlgorithm::setProgressCallback(ProgressCallback callBack, void * state)
{
  progressCallback_ = std::pair<ProgressCallback, void *>(callBack, state);
}


void SimulationAlgorithm::setStopCallback(StopCallback callBack, void * state)
{
  stopCallback_ = std::pair<StopCallback, void *>(callBack, state);
}


END_NAMESPACE_OPENTURNS
