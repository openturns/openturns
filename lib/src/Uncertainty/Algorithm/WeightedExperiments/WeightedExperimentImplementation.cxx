//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an weightedExperiment plane
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTprivate.hxx"
#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeightedExperimentImplementation)

static const Factory<WeightedExperimentImplementation> Factory_WeightedExperimentImplementation;


/* Default constructor */
WeightedExperimentImplementation::WeightedExperimentImplementation()
  : ExperimentImplementation()
  , distribution_()
  , size_(ResourceMap::GetAsUnsignedInteger("WeightedExperiment-DefaultSize"))
{
  // Nothing to do
}

/* Constructor with parameters */
WeightedExperimentImplementation::WeightedExperimentImplementation(const UnsignedInteger size)
  : ExperimentImplementation()
  , distribution_()
  , size_(0)
{
  // Check if the size is valid
  setSize(size);
}

/* Constructor with parameters */
WeightedExperimentImplementation::WeightedExperimentImplementation(const Distribution & distribution,
    const UnsignedInteger size)
  : ExperimentImplementation()
  , distribution_(distribution)
  , size_(0)
{
  // Check if the size is valid
  setSize(size);
}

/* Virtual constructor */
WeightedExperimentImplementation * WeightedExperimentImplementation::clone() const
{
  return new WeightedExperimentImplementation(*this);
}

/* String converter */
String WeightedExperimentImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName ()
      << " distribution=" << distribution_
      << " size=" << size_;
  return oss;
}

/* Distribution accessor */
void WeightedExperimentImplementation::setDistribution(const Distribution & distribution)
{
  distribution_ = distribution;
}

Distribution WeightedExperimentImplementation::getDistribution() const
{
  return distribution_;
}

/* Size accessor */
void WeightedExperimentImplementation::setSize(const UnsignedInteger size)
{
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the size must be > 0.";
  size_ = size;
}

UnsignedInteger WeightedExperimentImplementation::getSize() const
{
  return size_;
}

Bool WeightedExperimentImplementation::hasUniformWeights() const
{
  return true;
}

/* Sample generation */
Sample WeightedExperimentImplementation::generate() const
{
  Point weights;
  return generateWithWeights(weights);
}

/* Sample generation with weights */
Sample WeightedExperimentImplementation::generateWithWeights(Point & ) const
{
  throw NotYetImplementedException(HERE) << "In WeightedExperimentImplementation::generateWithWeights()";
}

/* Method save() stores the object through the StorageManager */
void WeightedExperimentImplementation::save(Advocate & adv) const
{
  ExperimentImplementation::save(adv);
  adv.saveAttribute("distribution_", distribution_);
  adv.saveAttribute("size_", size_);
}

/* Method load() reloads the object from the StorageManager */
void WeightedExperimentImplementation::load(Advocate & adv)
{
  ExperimentImplementation::load(adv);
  adv.loadAttribute("distribution_", distribution_);
  adv.loadAttribute("size_", size_);
}

END_NAMESPACE_OPENTURNS
