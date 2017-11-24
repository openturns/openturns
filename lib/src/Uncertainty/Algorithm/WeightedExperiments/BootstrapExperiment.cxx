//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an bootstrapExperiment plane
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/BootstrapExperiment.hxx"
#include "openturns/Categorical.hxx"
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
  WeightedExperimentImplementation(Categorical(sample), sample.getSize())
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
      << " distribution=" << distribution_
      << " size=" << size_;
  return oss;
}

/* Sample generation */
Sample BootstrapExperiment::generateWithWeights(Point & weights) const
{
  weights = Point(size_, 1.0 / size_);
  return distribution_.getSample(size_);
}

END_NAMESPACE_OPENTURNS
