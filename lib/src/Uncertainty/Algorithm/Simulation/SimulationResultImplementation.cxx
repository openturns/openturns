//                                               -*- C++ -*-
/**
 *  @brief Implementation of SimulationResultImplementation
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
#include "openturns/SimulationResultImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SimulationResultImplementation)

static const Factory<SimulationResultImplementation> Factory_SimulationResultImplementation;

/* Default constructor */
SimulationResultImplementation::SimulationResultImplementation()
  : PersistentObject()
  , outerSampling_(0)
  , blockSize_(0)
{
  // Nothing to do
}

/* Standard constructor */
SimulationResultImplementation::SimulationResultImplementation(const UnsignedInteger outerSampling,
                                   const UnsignedInteger blockSize)
  : PersistentObject()
  , outerSampling_(outerSampling)
  , blockSize_(blockSize)
{
  // Nothing to do
}

/* Virtual constructor */
SimulationResultImplementation * SimulationResultImplementation::clone() const
{
  return new SimulationResultImplementation(*this);
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

/* Elapsed time accessor */
void SimulationResultImplementation::setTimeDuration(const Scalar time)
{
  timeDuration_ = time;
}

Scalar SimulationResultImplementation::getTimeDuration() const
{
  return timeDuration_;
}

/* Comparison operator */
Bool SimulationResultImplementation::operator ==(const PersistentObject & other) const
{
  const auto * p_other = dynamic_cast<const SimulationResultImplementation *>(&other);
  if (p_other == nullptr) return false;
  if (this == p_other) return true;
  return (outerSampling_ == p_other->outerSampling_)
      && (blockSize_ == p_other->blockSize_)
      && (timeDuration_ == p_other->timeDuration_);
}

/* Comparison operator */
Bool SimulationResultImplementation::operator !=(const PersistentObject & other) const
{
  return !operator==(other);
}

/* String converter */
String SimulationResultImplementation::__repr__() const
{
  OSS oss;
  oss << " outerSampling=" << outerSampling_
      << " blockSize=" << blockSize_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SimulationResultImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("outerSampling_", outerSampling_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("timeDuration_", timeDuration_);
}

/* Method load() reloads the object from the StorageManager */
void SimulationResultImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("outerSampling_", outerSampling_);
  adv.loadAttribute("blockSize_", blockSize_);
  if (adv.hasAttribute("timeDuration_"))
    adv.loadAttribute("timeDuration_", timeDuration_);
}

END_NAMESPACE_OPENTURNS
