//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResult
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SimulationResult.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SimulationResult)

static const Factory<SimulationResult> Factory_SimulationResult;

/* Default constructor */
SimulationResult::SimulationResult()
  : PersistentObject()
  , outerSampling_(0)
  , blockSize_(0)
{
  // Nothing to do
}

/* Standard constructor */
SimulationResult::SimulationResult(const UnsignedInteger outerSampling,
                                   const UnsignedInteger blockSize)
  : PersistentObject()
  , outerSampling_(outerSampling)
  , blockSize_(blockSize)
{
  // Nothing to do
}

/* Virtual constructor */
SimulationResult * SimulationResult::clone() const
{
  return new SimulationResult(*this);
}

/* Outer sampling accessor */
UnsignedInteger SimulationResult::getOuterSampling() const
{
  return outerSampling_;
}

void SimulationResult::setOuterSampling(const UnsignedInteger outerSampling)
{
  outerSampling_ = outerSampling;
}

/* Block size accessor */
UnsignedInteger SimulationResult::getBlockSize() const
{
  return blockSize_;
}

void SimulationResult::setBlockSize(const UnsignedInteger blockSize)
{
  blockSize_ = blockSize;
}

/* String converter */
String SimulationResult::__repr__() const
{
  OSS oss;
  oss << " outerSampling=" << outerSampling_
      << " blockSize=" << blockSize_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SimulationResult::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("outerSampling_", outerSampling_);
  adv.saveAttribute("blockSize_", blockSize_);
}

/* Method load() reloads the object from the StorageManager */
void SimulationResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("outerSampling_", outerSampling_);
  adv.loadAttribute("blockSize_", blockSize_);
}

END_NAMESPACE_OPENTURNS
