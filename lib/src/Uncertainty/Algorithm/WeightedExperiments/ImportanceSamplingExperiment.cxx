//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an monteCarloExperiment plane
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
#include "openturns/ImportanceSamplingExperiment.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ImportanceSamplingExperiment)

static const Factory<ImportanceSamplingExperiment> Factory_ImportanceSamplingExperiment;

/* Default constructor */
ImportanceSamplingExperiment::ImportanceSamplingExperiment():
  WeightedExperimentImplementation()
{
  // Take the default distribution as the importance distribution
  importanceDistribution_ = distribution_;
}

/* Constructor with parameters */
ImportanceSamplingExperiment::ImportanceSamplingExperiment(const Distribution & importanceDistribution)
  : WeightedExperimentImplementation(),
    importanceDistribution_(importanceDistribution)
{
  // Take the default distribution as the importance distribution
}

/* Constructor with parameters */
ImportanceSamplingExperiment::ImportanceSamplingExperiment(const Distribution & importanceDistribution,
    const UnsignedInteger size):
  WeightedExperimentImplementation(size),
  importanceDistribution_(importanceDistribution)
{
  // Take the default distribution as the importance distribution
}

/* Constructor with parameters */
ImportanceSamplingExperiment::ImportanceSamplingExperiment(const Distribution & distribution,
    const Distribution & importanceDistribution,
    const UnsignedInteger size):
  WeightedExperimentImplementation(distribution, size),
  importanceDistribution_(importanceDistribution)
{
  // Check if the distributions have compatible dimensions
  if (distribution.getDimension() != importanceDistribution.getDimension()) throw InvalidArgumentException(HERE) << "Error: the distribution and the importance distribution must have the same dimension.";
}

/* Virtual constructor */
ImportanceSamplingExperiment * ImportanceSamplingExperiment::clone() const
{
  return new ImportanceSamplingExperiment(*this);
}

/* Importance distribution accessor */
Distribution ImportanceSamplingExperiment::getImportanceDistribution() const
{
  return importanceDistribution_;
}

/* String converter */
String ImportanceSamplingExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " distribution=" << distribution_
      << " importance distribution=" << importanceDistribution_
      << " size=" << size_;
  return oss;
}

Bool ImportanceSamplingExperiment::hasUniformWeights() const
{
  return false;
}

/* Sample generation with weights */
Sample ImportanceSamplingExperiment::generateWithWeights(Point & weights) const
{
  Sample result(size_, distribution_.getDimension());
  result.setDescription(distribution_.getDescription());
  weights = Point(size_);
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    result[i] = importanceDistribution_.getRealization();
    weights[i] = distribution_.computePDF(result[i]) / importanceDistribution_.computePDF(result[i]);
  }
  return result;
}

/* Method save() stores the object through the StorageManager */
void ImportanceSamplingExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("importanceDistribution_", importanceDistribution_);
}

/* Method load() reloads the object from the StorageManager */
void ImportanceSamplingExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("importanceDistribution_", importanceDistribution_);
}


END_NAMESPACE_OPENTURNS
