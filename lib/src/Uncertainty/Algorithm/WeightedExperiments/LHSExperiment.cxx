//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an LHSExperiment plane
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/LHSExperiment.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Point.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Distribution> DistributionCollection;

CLASSNAMEINIT(LHSExperiment)

static const Factory<LHSExperiment> Factory_LHSExperiment;

/* Default constructor */
LHSExperiment::LHSExperiment()
  : WeightedExperimentImplementation()
  , marginals_(1, distribution_)
  , shuffle_(0, 0)
{
  // Check if the distribution has an independent copula and build the transformation
  setDistribution(distribution_);
}

/* Constructor with parameters */
LHSExperiment::LHSExperiment(const UnsignedInteger size,
                             const Bool alwaysShuffle,
                             const Bool randomShift)
  : WeightedExperimentImplementation(size)
  , marginals_(1, distribution_)
  , shuffle_(0, 0)
  , alwaysShuffle_(alwaysShuffle)
  , randomShift_(randomShift)
{
  // Check if the distribution has an independent copula and build the transformation
  setDistribution(distribution_);
}

/* Constructor with parameters */
LHSExperiment::LHSExperiment(const Distribution & distribution,
                             const UnsignedInteger size,
                             const Bool alwaysShuffle,
                             const Bool randomShift)
  : WeightedExperimentImplementation(distribution, size)
  , marginals_(0)
  , shuffle_(0, 0)
  , alwaysShuffle_(alwaysShuffle)
  , randomShift_(randomShift)
{
  // Check if the distribution has an independent copula and build the transformation
  setDistribution(distribution);
}

/* Virtual constructor */
LHSExperiment * LHSExperiment::clone() const
{
  return new LHSExperiment(*this);
}

/* String converter */
String LHSExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " distribution=" << distribution_
      << " size=" << size_
      << " alwaysShuffle=" << alwaysShuffle_
      << " random shift=" << randomShift_;
  return oss;
}

String LHSExperiment::__str__(const String & ) const
{
  OSS oss;
  oss << GetClassName()
      << "(distribution=" << distribution_
      << ", size" << size_
      << ", always shuffle=" << alwaysShuffle_
      << ", random shift=" << randomShift_;
  return oss;
}

/* Sample generation */
Sample LHSExperiment::generateStandard() const
{
  const UnsignedInteger dimension = distribution_.getDimension();
  // To insure that the shuffle has been initialized
  (void) getShuffle();
  SampleImplementation sample(size_, dimension);
  const Point u(randomShift_ ? RandomGenerator::Generate(size_ * dimension) : Point(size_ * dimension, 0.5));
  sample.setData((Point(*shuffle_.getImplementation()) + u) / size_);
  return sample;
}

Sample LHSExperiment::generateWithWeights(Point & weights) const
{
  Sample sample(generateStandard());
  // In-place transformation
  sample = transformation_(sample);
  sample.setDescription(distribution_.getDescription());
  weights = Point(size_, 1.0 / size_);
  return sample;
}

/* Shuffle the cells. */
Matrix LHSExperiment::ComputeShuffle(const UnsignedInteger dimension,
                                     const UnsignedInteger totalSize)
{
  // Preallocate the shuffle
  Matrix result(dimension, totalSize);
  // Initialize the cells shuffle to the identity
  for (UnsignedInteger j = 0; j < totalSize; ++j)
    for (UnsignedInteger i = 0; i < dimension; ++i)
      result(i, j) = j;
  // Perform the shuffle. We use the same algorithm than the one used in the STL or in GSL
  for (UnsignedInteger index = 0; index < totalSize - 1; ++index)
  {
    // For each component of the current realization, compute its new position
    const RandomGenerator::UnsignedIntegerCollection u(RandomGenerator::IntegerGenerate(dimension, totalSize - index));
    // Then swap the corresponding components
    for (UnsignedInteger component = 0; component < dimension; ++component)
    {
      const UnsignedInteger newPosition = index + u[component];
      const Scalar newValue = result(component, newPosition);
      result(component, newPosition) = result(component, index);
      result(component, index) = newValue;
    }
  }
  return result;
}

/* Shuffle accessor */
Matrix LHSExperiment::getShuffle() const
{
  if (alwaysShuffle_ || !isAlreadyComputedShuffle_)
  {
    shuffle_ = ComputeShuffle(distribution_.getDimension(), size_);
    isAlreadyComputedShuffle_ = true;
  }
  return shuffle_;
}

/* Distribution accessor */
void LHSExperiment::setDistribution(const Distribution & distribution)
{
  if (!distribution.hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error: cannot use the LHS experiment with a non-independent copula.";
  const UnsignedInteger dimension = distribution.getDimension();
  DistributionCollection marginals(dimension);
  // Get the marginal distributions
  for (UnsignedInteger i = 0; i < dimension; ++ i) marginals[i] = distribution.getMarginal(i);
  if (dimension != getDistribution().getDimension())
    isAlreadyComputedShuffle_ = false;
  // Build the iso-probabilistic transformation
  transformation_ = MarginalTransformationEvaluation(marginals, MarginalTransformationEvaluation::TO);
  WeightedExperimentImplementation::setDistribution(distribution);
}

void LHSExperiment::setSize(const UnsignedInteger size)
{
  WeightedExperimentImplementation::setSize(size);
  isAlreadyComputedShuffle_ = false;
}

/* AlwaysShuffle accessor */
Bool LHSExperiment::getAlwaysShuffle() const
{
  return alwaysShuffle_;
}

void LHSExperiment::setAlwaysShuffle(const Bool alwaysShuffle)
{
  alwaysShuffle_ = alwaysShuffle;
}

/* Random shift accessor */
Bool LHSExperiment::getRandomShift() const
{
  return randomShift_;
}

void LHSExperiment::setRandomShift(const Bool randomShift)
{
  randomShift_ = randomShift;
}

/* Method save() stores the object through the StorageManager */
void LHSExperiment::save(Advocate & adv) const
{
  WeightedExperimentImplementation::save(adv);
  adv.saveAttribute("alwaysShuffle_", alwaysShuffle_);
  adv.saveAttribute("randomShift_", randomShift_);
}

/* Method load() reloads the object from the StorageManager */
void LHSExperiment::load(Advocate & adv)
{
  WeightedExperimentImplementation::load(adv);
  adv.loadAttribute("alwaysShuffle_", alwaysShuffle_);
  adv.loadAttribute("randomShift_", randomShift_);
  setDistribution(distribution_);
}

END_NAMESPACE_OPENTURNS
