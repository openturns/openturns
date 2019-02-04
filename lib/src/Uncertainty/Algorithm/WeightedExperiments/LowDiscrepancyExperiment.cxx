//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an monteCarloExperiment plane
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
#include "openturns/LowDiscrepancyExperiment.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/Exception.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/DistributionTransformation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LowDiscrepancyExperiment)

static const Factory<LowDiscrepancyExperiment> Factory_LowDiscrepancyExperiment;

/* Default constructor */
LowDiscrepancyExperiment::LowDiscrepancyExperiment()
  : WeightedExperimentImplementation()
  , sequence_(SobolSequence())
  , restart_(true)
  , randomize_(false)
{
  // Build the iso-probabilistic transformation
  setDistribution(distribution_);
}

/* Constructor with parameters */
LowDiscrepancyExperiment::LowDiscrepancyExperiment(const UnsignedInteger size,
    const Bool restart)
  : WeightedExperimentImplementation(size)
  , sequence_(SobolSequence())
  , restart_(restart)
  , randomize_(false)
{
  // Build the iso-probabilistic transformation
  setDistribution(distribution_);
}

/* Constructor with parameters */
LowDiscrepancyExperiment::LowDiscrepancyExperiment(const LowDiscrepancySequence & sequence,
    const Distribution & distribution,
    const UnsignedInteger size,
    const Bool restart)
  : WeightedExperimentImplementation(size)
  , sequence_(sequence)
  , restart_(restart)
  , randomize_(false)
{
  // Warning! The distribution must not be given to the upper class directly
  // because the correct initialization of the sequence depends on a test on
  // its dimension
  setDistribution(distribution);
}

/* Constructor with parameters */
LowDiscrepancyExperiment::LowDiscrepancyExperiment(const LowDiscrepancySequence & sequence,
    const UnsignedInteger size,
    const Bool restart)
  : WeightedExperimentImplementation(size)
  , sequence_(sequence)
  , restart_(restart)
  , randomize_(false)
{
  // Warning! The distribution must not be given to the upper class directly
  // because the correct initialization of the sequence depends on a test on
  // its dimension
  setDistribution(IndependentCopula(sequence.getDimension()));
}

/* Virtual constructor */
LowDiscrepancyExperiment * LowDiscrepancyExperiment::clone() const
{
  return new LowDiscrepancyExperiment(*this);
}

/* String converter */
String LowDiscrepancyExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " sequence=" << sequence_
      << " distribution=" << distribution_
      << " size=" << size_
      << " restart=" << restart_
      << " randomize=" << randomize_;
  return oss;
}

String LowDiscrepancyExperiment::__str__(const String & ) const
{
  OSS oss;
  oss << GetClassName()
      << "(sequence=" << sequence_
      << ", distribution=" << distribution_
      << ", size" << size_
      << ", restart=" << restart_
      << ", randomize=" << randomize_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LowDiscrepancyExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("sequence_", sequence_);
  adv.saveAttribute("restart_", restart_);
  adv.saveAttribute("randomize_", randomize_);
}

/* Method load() reloads the object from the StorageManager */
void LowDiscrepancyExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("sequence_", sequence_);
  adv.loadAttribute("restart_", restart_);
  adv.loadAttribute("randomize_", randomize_);
  setDistribution(distribution_);
}

/* Distribution accessor */
void LowDiscrepancyExperiment::setDistribution(const Distribution & distribution)
{
  const UnsignedInteger dimension = distribution.getDimension();
  // restart the low-discrepancy sequence if asked for or mandatory (dimension changed)
  if (restart_ || (dimension != getDistribution().getDimension()))
    sequence_.initialize(dimension);
  // Build the iso-probabilistic transformation
  // For distributions with non-indepedent copula, it resorts to using the method
  // described in:
  // Mathieu Cambou, Marius Hofert, Christiane Lemieux, "Quasi-Random numbers for copula models", Statistics and Computing, September 2017, Volume 27, Issue 5, pp 1307–1329
  // preprint here: https://arxiv.org/pdf/1508.03483.pdf
  transformation_ = DistributionTransformation(IndependentCopula(dimension), distribution);
  WeightedExperimentImplementation::setDistribution(distribution);
}

/* Sequence accessor */
LowDiscrepancySequence LowDiscrepancyExperiment::getSequence() const
{
  return sequence_;
}

/* Restart accessor */
Bool LowDiscrepancyExperiment::getRestart() const
{
  return restart_;
}

void LowDiscrepancyExperiment::setRestart(const Bool restart)
{
  restart_ = restart;
}


/* Randomization accessor */
Bool LowDiscrepancyExperiment::getRandomize() const
{
  return randomize_;
}

void LowDiscrepancyExperiment::setRandomize(const Bool randomize)
{
  randomize_ = randomize;
}


/* Sample generation */
Sample LowDiscrepancyExperiment::generateWithWeights(Point & weights) const
{
  Sample sample(sequence_.generate(size_));
  sample.setDescription(distribution_.getDescription());
  const UnsignedInteger dimension = distribution_.getDimension();
  Scalar tmp = -1.0;
  if (randomize_)
  {
    const Point shift(RandomGenerator::Generate(dimension));
    for (UnsignedInteger i = 0; i < size_; ++ i)
    {
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        // with a cyclic scrambling of the low discrepancy point as in
        // L’Ecuyer P., Lemieux C. (2005) Recent Advances in Randomized Quasi-Monte Carlo Methods. In: Dror M., L’Ecuyer P., Szidarovszky F. (eds) Modeling Uncertainty. International Series in Operations Research & Management Science, vol 46. Springer, Boston, MA
        sample(i, j) = std::modf(sample(i, j) + shift[j], &tmp);
      } // j
    } // i
  } // randomize
  // In-place transformation to reduce memory consumption
  sample = transformation_(sample);
  weights = Point(size_, 1.0 / size_);
  return sample;
}

END_NAMESPACE_OPENTURNS
