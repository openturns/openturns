//                                               -*- C++ -*-
/**
 *  @brief @brief Experiment to compute Sobol' indices
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SobolIndicesExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/LowDiscrepancyExperiment.hxx"
#include "openturns/LHSExperiment.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/ComposedDistribution.hxx"

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
    throw InvalidArgumentException(HERE) << "In SobolIndicesExperiment the distribution must have an independent copula";
  if (!experiment.hasUniformWeights())
    throw InvalidArgumentException(HERE) << "In SobolIndicesExperiment the underlyng weighted experiment must have uniform weights";

  const UnsignedInteger size = experiment.getSize();
  const UnsignedInteger dimension = experiment.getDistribution().getDimension();
  size_ = size * (2 + dimension);// A,B,E samples for first, total order
  // C sample for second order, except for d=2
  if (computeSecondOrder_ && (dimension != 2))
    size_ += size * dimension;
}


/* Constructor with parameters */
SobolIndicesExperiment::SobolIndicesExperiment(const Distribution & distribution,
    const UnsignedInteger size,
    const Bool computeSecondOrder)
  : WeightedExperimentImplementation()
{
  if (!distribution.hasIndependentCopula())
    throw InvalidArgumentException(HERE) << "In SobolIndicesExperiment the distribution must have an independent copula";
  const UnsignedInteger dimension = distribution.getDimension();
  WeightedExperiment experiment;
  // The default method is to use MonteCarloExperiment in order to use the
  // asymptotic distribution of the estimate in SobolIndicesAlgorithm
  short method = 0;
  if (ResourceMap::GetAsString("SobolIndicesExperiment-SamplingMethod") == "LHS")
  {
    method = 1;
  } // LHS
  else if (ResourceMap::GetAsString("SobolIndicesExperiment-SamplingMethod") == "QMC")
  {
    if (dimension <= SobolSequence::MaximumNumberOfDimension)
    {
      method = 2;
    } // QMC
    else
    {
      LOGWARN(OSS() << "Can use Sobol sequence in SobolIndicesExperiment only for dimension not greater than " << SobolSequence::MaximumNumberOfDimension << ", here dimension=" << dimension << ". Using LHS instead.");
      method = 1;
    } // QMC->LHS
  } // QMC
  switch (method)
  {
    // MonteCarlo
    case 0:
      {
        experiment = MonteCarloExperiment(distribution, size);
      }
      break;
    // LHS
    case 1:
      {
        LHSExperiment lhsExperiment(distribution, size);
        lhsExperiment.setRandomShift(false);
        experiment = lhsExperiment;
      }
      break;
    // QMC
    case 2:
      {
        experiment = LowDiscrepancyExperiment(SobolSequence(dimension), distribution, size);
      }
      break;
    default:
      throw InternalException(HERE) << "Error: unknown sampling method=" << method << " in SobolIndicesExperiment.";
  }
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

void SobolIndicesExperiment::setSize(const UnsignedInteger )
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


/* WeightedExperiment accessor */
WeightedExperiment SobolIndicesExperiment::getWeightedExperiment() const
{
  return experiment_;
}


Bool SobolIndicesExperiment::hasUniformWeights() const
{
  return true;
}

/* Sample generation */
Sample SobolIndicesExperiment::generateWithWeights(Point & weights) const
{
  const UnsignedInteger size = experiment_.getSize();
  // Here we generate the sample by doubling the distribution
  // in order to allow for low discrepancy experiments
  Distribution distribution(experiment_.getDistribution());
  const UnsignedInteger dimension = distribution.getDimension();
  ComposedDistribution::DistributionCollection marginals(2 * dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      marginals[i] = distribution.getMarginal(i);
      marginals[dimension + i] = marginals[i];
    }
  const ComposedDistribution doubleDistribution(marginals);
  // Generate a 2xdim sample of needed size
  WeightedExperiment doubleExperiment(experiment_);
  doubleExperiment.setDistribution(doubleDistribution);
  const Sample doubleDesign(doubleExperiment.generate());
  // Then reorganize the sample into a dim sample of twice the size
  // A part
  Sample design(2 * size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      design(i, j) = doubleDesign(i, j);
  // B part
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      design(i + size, j) = doubleDesign(i, dimension + j);
  // Compute designs of type Saltelli/Martinez for 1st order
  for (UnsignedInteger p = 0; p < dimension; ++ p)
  {
    Sample E(design, 0, size);// E=A
    for (UnsignedInteger k = 0; k < size; ++ k) E(k, p) = design(k + size, p);
    design.add(E);
  }
  // Special case for dim=2: do not add the C sample
  if (computeSecondOrder_ && (dimension != 2))
  {
    for (UnsignedInteger p = 0; p < dimension; ++ p)
    {
      Sample C(design, size, 2 * size);// C=B
      for (UnsignedInteger k = 0; k < size; ++ k) C(k, p) = design(k, p);
      design.add(C);
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
