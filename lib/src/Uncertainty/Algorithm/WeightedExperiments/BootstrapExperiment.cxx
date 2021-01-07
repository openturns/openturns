//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an bootstrapExperiment plane
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/BootstrapExperiment.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BootstrapExperiment)

static const Factory<BootstrapExperiment> Factory_BootstrapExperiment;

/* Default constructor */
BootstrapExperiment::BootstrapExperiment():
  WeightedExperimentImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
BootstrapExperiment::BootstrapExperiment(const Sample & sample):
  WeightedExperimentImplementation(UserDefined(), sample.getSize())
  , sample_(sample)
{
  // Nothing to do
}

/* Virtual constructor */
BootstrapExperiment * BootstrapExperiment::clone() const
{
  return new BootstrapExperiment(*this);
}

/* String converter */
String BootstrapExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " sample=" << sample_
      << " size=" << size_;
  return oss;
}

/* Sample generation */
Sample BootstrapExperiment::generateWithWeights(Point & weights) const
{
  weights = Point(size_, 1.0 / size_);
  return sample_.select(GenerateSelection(size_, size_));
}

/* Selection generation */
Indices BootstrapExperiment::GenerateSelection(const UnsignedInteger size,
    const UnsignedInteger length)
{
  const RandomGenerator::UnsignedIntegerCollection selection(RandomGenerator::IntegerGenerate(size, length));
  return Indices(selection.begin(), selection.end());
}

/* Distribution accessor */
void BootstrapExperiment::setDistribution(const Distribution & distribution)
{
  if (!distribution.isDiscrete()) throw InvalidArgumentException(HERE) << "Error: the distribution must be discrete in BootstrapExperiment.";
  const Point probabilities(distribution.getProbabilities());
  // Here we know that size>0 as no discrete distribution can have an empty probability table
  const UnsignedInteger size = probabilities.getSize();
  for (UnsignedInteger i = 1; i < size; ++ i)
    if (probabilities[i] != probabilities[0]) throw InvalidArgumentException(HERE) << "Error: the distribution must be uniform over its support in BootstrapEperiment.";
  distribution_ = distribution;
  sample_ = distribution.getSupport();
}

Distribution BootstrapExperiment::getDistribution() const
{
  return UserDefined(sample_);
}


END_NAMESPACE_OPENTURNS
