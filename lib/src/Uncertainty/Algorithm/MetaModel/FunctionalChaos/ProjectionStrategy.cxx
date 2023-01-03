//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for projection strategies
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
#include "openturns/ProjectionStrategy.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(ProjectionStrategy)

/* Default constructor */
ProjectionStrategy::ProjectionStrategy()
  : TypedInterfaceObject<ProjectionStrategyImplementation>(new ProjectionStrategyImplementation)
{
  // Nothing to do
}


/* Parameter constructor */
ProjectionStrategy::ProjectionStrategy(const Distribution & measure)
  : TypedInterfaceObject<ProjectionStrategyImplementation>(new ProjectionStrategyImplementation(measure))
{
  // Nothing to do
}


/* Constructor from implementation */
ProjectionStrategy::ProjectionStrategy(const ProjectionStrategyImplementation & implementation)
  : TypedInterfaceObject<ProjectionStrategyImplementation>(implementation.clone())
{
  // Nothing to do
}


/* Measure accessor */
void ProjectionStrategy::setMeasure(const Distribution & measure)
{
  copyOnWrite();
  getImplementation()->setMeasure(measure);
}

Distribution ProjectionStrategy::getMeasure() const
{
  return getImplementation()->getMeasure();
}


/* Experiment accessors */
void ProjectionStrategy::setExperiment(const WeightedExperiment & weightedExperiment)
{
  copyOnWrite();
  getImplementation()->setExperiment(weightedExperiment);
}

WeightedExperiment ProjectionStrategy::getExperiment() const
{
  return getImplementation()->getExperiment();
}

/* Sample accessors */
void ProjectionStrategy::setInputSample(const Sample & inputSample)
{
  copyOnWrite();
  getImplementation()->setInputSample(inputSample);
}

Sample ProjectionStrategy::getInputSample() const
{
  return getImplementation()->getInputSample();
}

void ProjectionStrategy::setOutputSample(const Sample & outputSample)
{
  copyOnWrite();
  getImplementation()->setOutputSample(outputSample);
}

Sample ProjectionStrategy::getOutputSample() const
{
  return getImplementation()->getOutputSample();
}

/* Weights accessor */
void ProjectionStrategy::setWeights(const Point & weights)
{
  copyOnWrite();
  getImplementation()->setWeights(weights);
}

Point ProjectionStrategy::getWeights() const
{
  return getImplementation()->getWeights();
}

/* Residual accessor */
Scalar ProjectionStrategy::getResidual() const
{
  return getImplementation()->getResidual();
}

/* Relative error accessor */
Scalar ProjectionStrategy::getRelativeError() const
{
  return getImplementation()->getRelativeError();
}

/* Relative error accessor */
Point ProjectionStrategy::getCoefficients() const
{
  return getImplementation()->getCoefficients();
}


/* Compute the components alpha_k_p_ by projecting the model on the partial L2 basis */
void ProjectionStrategy::computeCoefficients(const Function & function,
    const FunctionCollection & basis,
    const Indices & indices,
    const Indices & addedRanks,
    const Indices & conservedRanks,
    const Indices & removedRanks,
    const UnsignedInteger marginalIndex)
{
  getImplementation()->computeCoefficients(function, basis, indices, addedRanks, conservedRanks, removedRanks, marginalIndex);
}


/* String converter */
String ProjectionStrategy::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}


/* String converter */
String ProjectionStrategy::__str__(const String & ) const
{
  return __repr__();
}



END_NAMESPACE_OPENTURNS
