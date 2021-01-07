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
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MonteCarloExperiment)

static const Factory<MonteCarloExperiment> Factory_MonteCarloExperiment;

/* Default constructor */
MonteCarloExperiment::MonteCarloExperiment():
  WeightedExperimentImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
MonteCarloExperiment::MonteCarloExperiment(const UnsignedInteger size):
  WeightedExperimentImplementation(size)
{
  // Nothing to do
}

/* Constructor with parameters */
MonteCarloExperiment::MonteCarloExperiment(const Distribution & distribution,
    const UnsignedInteger size):
  WeightedExperimentImplementation(distribution, size)
{
  // Nothing to do
}

/* Virtual constructor */
MonteCarloExperiment * MonteCarloExperiment::clone() const
{
  return new MonteCarloExperiment(*this);
}

/* String converter */
String MonteCarloExperiment::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " distribution=" << distribution_
      << " size=" << size_;
  return oss;
}

/* Sample generation */
Sample MonteCarloExperiment::generateWithWeights(Point & weights) const
{
  weights = Point(size_, 1.0 / size_);
  return distribution_.getSample(size_);
}

END_NAMESPACE_OPENTURNS
