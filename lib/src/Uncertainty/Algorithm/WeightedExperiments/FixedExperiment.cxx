//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an fixedExperiment plane
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/FixedExperiment.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FixedExperiment)

static const Factory<FixedExperiment> Factory_FixedExperiment;

/* Default constructor */
FixedExperiment::FixedExperiment()
  : WeightedExperimentImplementation()
  , sample_(0, 0)
{
  // Nothing to do
}

/* Constructor with parameters */
FixedExperiment::FixedExperiment(const Sample & sample)
  : WeightedExperimentImplementation(UserDefined(sample), sample.getSize())
  , sample_(sample)
  , weights_(sample.getSize(), 1.0 / sample.getSize())
{
}

/* Constructor with parameters */
FixedExperiment::FixedExperiment(const Sample & sample,
                                 const Point & weights)
  : WeightedExperimentImplementation(UserDefined(sample, weights), sample.getSize())
  , sample_(sample)
  , weights_(weights)
{
}

/* Virtual constructor */
FixedExperiment * FixedExperiment::clone() const
{
  return new FixedExperiment(*this);
}

/* String converter */
String FixedExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " sample=" << sample_
      << " weights=" << weights_;
  return oss;
}

/* Distribution accessor */
void FixedExperiment::setDistribution(const Distribution & )
{
  // The distribution of a FixedExperiment cannot be changed
}

Bool FixedExperiment::hasUniformWeights() const
{
  return weights_ == Point(sample_.getSize(), 1.0 / sample_.getSize());
}

/* Sample generation */
Sample FixedExperiment::generateWithWeights(Point & weights) const
{
  weights = weights_;
  return sample_;
}

/* Method save() stores the object through the StorageManager */
void FixedExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("sample_", sample_);
  adv.saveAttribute("weights_", weights_);
}

/* Method load() reloads the object from the StorageManager */
void FixedExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("sample_", sample_);
  adv.loadAttribute("weights_", weights_);
}

END_NAMESPACE_OPENTURNS
