//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an LHSExperiment plane
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "LHSExperiment.hxx"
#include "RandomGenerator.hxx"
#include "Collection.hxx"
#include "NumericalPoint.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



typedef Collection<Distribution> DistributionCollection;

CLASSNAMEINIT(LHSExperiment);

/* Default constructor */
LHSExperiment::LHSExperiment():
  WeightedExperiment(),
  shuffle_(0, 0),
  isAlreadyComputedShuffle_(false)
{
  // Nothing to do
}

/* Constructor with parameters */
LHSExperiment::LHSExperiment(const UnsignedInteger size):
  WeightedExperiment(size),
  shuffle_(0, 0),
  isAlreadyComputedShuffle_(false)
{
  // Nothing to do
}

/* Constructor with parameters */
LHSExperiment::LHSExperiment(const Distribution & distribution,
                             const UnsignedInteger size):
  WeightedExperiment(distribution, size),
  shuffle_(0, 0),
  isAlreadyComputedShuffle_(false)
{
  // Check if the distribution has an independent copula
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
      << " size=" << size_;
  return oss;
}

/* Sample generation */
NumericalSample LHSExperiment::generate()
{
  const UnsignedInteger dimension(distribution_.getDimension());
  // To insure that the shuffle has been initialized
  shuffle_ = getShuffle();
  NumericalSample sample(size_, dimension);
  sample.setDescription(distribution_.getDescription());
  DistributionCollection marginals(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) marginals[i] = distribution_.getMarginal(i);
  for(UnsignedInteger index = 0; index < size_; ++index)
  {
    const NumericalPoint u(RandomGenerator::Generate(dimension));
    for(UnsignedInteger component = 0; component < dimension; ++component)
    {
      const NumericalScalar xi((shuffle_(component, index) + u[component]) / size_);
      sample[index][component] = marginals[component].computeQuantile(xi)[0];
    }
  }
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
      const UnsignedInteger newPosition(index + u[component]);
      const NumericalScalar newValue(result(component, newPosition));
      result(component, newPosition) = result(component, index);
      result(component, index) = newValue;
    }
  }
  return result;
}

/* Shuffle accessor */
Matrix LHSExperiment::getShuffle() const
{
  if (!isAlreadyComputedShuffle_)
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
  WeightedExperiment::setDistribution(distribution);
}

END_NAMESPACE_OPENTURNS
