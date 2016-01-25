//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an monteCarloExperiment plane
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "LowDiscrepancyExperiment.hxx"
#include "SobolSequence.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LowDiscrepancyExperiment);


/* Default constructor */
LowDiscrepancyExperiment::LowDiscrepancyExperiment():
  WeightedExperiment(),
  marginals_(),
  sequence_(SobolSequence())
{
  // Nothing to do
}

/* Constructor with parameters */
LowDiscrepancyExperiment::LowDiscrepancyExperiment(const UnsignedInteger size):
  WeightedExperiment(size),
  marginals_(),
  sequence_(SobolSequence())
{
  // Nothing to do
}

/* Constructor with parameters */
LowDiscrepancyExperiment::LowDiscrepancyExperiment(const LowDiscrepancySequence & sequence,
    const Distribution & distribution,
    const UnsignedInteger size):
  WeightedExperiment(distribution, size),
  marginals_(0),
  sequence_(sequence)
{
  setDistribution(distribution);
}

/* Constructor with parameters */
LowDiscrepancyExperiment::LowDiscrepancyExperiment(const LowDiscrepancySequence & sequence,
    const UnsignedInteger size):
  WeightedExperiment(size),
  marginals_(0),
  sequence_(sequence)
{
  // Nothing to do
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
      << " size=" << size_;
  return oss;
}

/* Distribution accessor */
void LowDiscrepancyExperiment::setDistribution(const Distribution & distribution)
{
  if (!distribution.hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error: the LowDiscrepancyExperiment can only be used with distributions having an independent copula.";
  const UnsignedInteger dimension = distribution.getDimension();
  marginals_ = DistributionCollection(dimension);
  // Get the marginal distributions
  for (UnsignedInteger i = 0; i < dimension; ++ i) marginals_[i] = distribution.getMarginal(i);
  // initialize the low-discrepancy sequence
  sequence_.initialize(dimension);
  WeightedExperiment::setDistribution(distribution);
}

/* Sequence accessor */
LowDiscrepancySequence LowDiscrepancyExperiment::getSequence() const
{
  return sequence_;
}

/* Sample generation */
NumericalSample LowDiscrepancyExperiment::generate()
{
  // In-place transformation to reduce memory consumption
  NumericalSample sample(sequence_.generate(size_));
  sample.setDescription(distribution_.getDescription());
  const UnsignedInteger dimension(marginals_.getSize());
  for (UnsignedInteger i = 0; i < size_; ++ i)
    for (UnsignedInteger j = 0; j < dimension; ++ j) sample[i][j] = marginals_[j].computeQuantile(sample[i][j])[0];
  return sample;
}

END_NAMESPACE_OPENTURNS
