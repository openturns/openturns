//                                               -*- C++ -*-
/**
 *  @brief @brief Experiment to compute Sobol' indices
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/SobolIndicesExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MonteCarloExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SobolIndicesExperiment)

static const Factory<SobolIndicesExperiment> Factory_SobolIndicesExperiment;

/* Default constructor */
SobolIndicesExperiment::SobolIndicesExperiment()
  : WeightedExperimentImplementation()
  , computeSecondOrder_(true)
{
  // Nothing to do
}


/* Constructor with parameters */
SobolIndicesExperiment::SobolIndicesExperiment(const WeightedExperiment & experiment,
    const Bool computeSecondOrder)
  : WeightedExperimentImplementation()
  , experiment_(experiment)
  , computeSecondOrder_(computeSecondOrder)
{
  if (!experiment.getDistribution().hasIndependentCopula())
    throw InvalidArgumentException(HERE) << "In SobolIndicesExperiment weighted's distribution should have independent copula";
  if (!experiment.hasUniformWeights())
    throw InvalidArgumentException(HERE) << "In SobolIndicesExperiment experiment should have uniform weights";

  const UnsignedInteger size = experiment.getSize();
  const UnsignedInteger dimension = experiment.getDistribution().getDimension();
  size_ = size * (2 + dimension * (computeSecondOrder_ ? 2 : 1));
}


/* Constructor with parameters */
SobolIndicesExperiment::SobolIndicesExperiment(const Distribution & distribution,
    const UnsignedInteger size,
    const Bool computeSecondOrder)
  : WeightedExperimentImplementation()
{
  const MonteCarloExperiment experiment(distribution, size);
  *this = SobolIndicesExperiment(experiment, computeSecondOrder);
}


/* Virtual constructor */
SobolIndicesExperiment * SobolIndicesExperiment::clone() const
{
  return new SobolIndicesExperiment(*this);
}


/* String converter */
String SobolIndicesExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " experiment=" << experiment_
      << " computeSecondOrder=" << computeSecondOrder_
      << " size=" << size_;
  return oss;
}

void SobolIndicesExperiment::setSize(const UnsignedInteger size)
{
  throw InternalException(HERE) << "Cannot set the size of SobolIndicesExperiment";
}

/* Distribution accessor */
void SobolIndicesExperiment::setDistribution(const Distribution & distribution)
{
  experiment_.setDistribution(distribution);
}

Distribution SobolIndicesExperiment::getDistribution() const
{
  return experiment_.getDistribution();
}


Bool SobolIndicesExperiment::hasUniformWeights() const
{
  return true;
}

/* Sample generation */
Sample SobolIndicesExperiment::generateWithWeights(Point & weights) const
{
  const UnsignedInteger size = experiment_.getSize();
  Sample design(experiment_.generate());
  design.add(experiment_.generate());
  const UnsignedInteger dimension = design.getDimension();

  // Compute designs of type Saltelli/Martinez for 1st order
  for (UnsignedInteger p = 0; p < dimension; ++ p)
  {
    Sample x(design, 0, size);// first chunk
    for (UnsignedInteger k = 0; k < size; ++ k) x[k][p] = design[k + size][p];
    design.add(x);
  }
  if (computeSecondOrder_)
  {
    for (UnsignedInteger p = 0; p < dimension; ++ p)
    {
      Sample x(design, size, 2 * size);// second chunk
      for (UnsignedInteger k = 0; k < size; ++ k) x[k][p] = design[k][p];
      design.add(x);
    }
  }
  weights = Point(getSize(), 1.0 / getSize());
  design.setDescription(experiment_.getDistribution().getDescription());
  return design;
}

/* Method save() stores the object through the StorageManager */
void SobolIndicesExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("computeSecondOrder_", computeSecondOrder_);
}

/* Method load() reloads the object from the StorageManager */
void SobolIndicesExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("computeSecondOrder_", computeSecondOrder_);
}

END_NAMESPACE_OPENTURNS
