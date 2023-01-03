//                                               -*- C++ -*-
/**
 *  @brief This is a abstract class for projection strategy implementations
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
#include "openturns/ProjectionStrategyImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/FixedExperiment.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProjectionStrategyImplementation)

static const Factory<ProjectionStrategyImplementation> Factory_ProjectionStrategyImplementation;


/* Default constructor */
ProjectionStrategyImplementation::ProjectionStrategyImplementation()
  : PersistentObject()
  , alpha_k_p_(0)
  , residual_p_(0.0)
  , relativeError_p_(0.0)
  , measure_()
  , inputSample_(0, 0)
  , weights_(0)
  , outputSample_(0, 0)
{
  // The ProjectionStrategyImplementation imposes its distribution to the weighted experiment
  weightedExperiment_.setDistribution(measure_);
}


/* Parameter constructor */
ProjectionStrategyImplementation::ProjectionStrategyImplementation(const Distribution & measure)
  : PersistentObject()
  , alpha_k_p_(0)
  , residual_p_(0.0)
  , relativeError_p_(0.0)
  , measure_(measure)
  , inputSample_(0, 0)
  , weights_(0)
  , outputSample_(0, 0)
{
  // The ProjectionStrategyImplementation imposes the distribution of the weighted experiment
  weightedExperiment_.setDistribution(measure_);
}


/* Parameter constructor */
ProjectionStrategyImplementation::ProjectionStrategyImplementation(const WeightedExperiment & weightedExperiment)
  : PersistentObject()
  , alpha_k_p_(0)
  , residual_p_(0.0)
  , relativeError_p_(0.0)
  , measure_(weightedExperiment.getDistribution())
  , weightedExperiment_(weightedExperiment)
  , inputSample_(0, 0)
  , weights_(0)
  , outputSample_(0, 0)
{
  // Nothing to do
}

/* Parameter constructor */
ProjectionStrategyImplementation::ProjectionStrategyImplementation(const Sample & inputSample,
    const Point & weights,
    const Sample & outputSample)
  : PersistentObject()
  , alpha_k_p_(0)
  , residual_p_(0.0)
  , measure_(UserDefined(inputSample))
  , weightedExperiment_(FixedExperiment(inputSample, weights))
  , inputSample_(0, 0)
  , weights_(0)
  , outputSample_(0, 0)
{
  if (inputSample.getSize() != weights.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot build a ProjectionStrategyImplementation with an input sample and weights of different size. Here, input sample size=" << inputSample.getSize() << ", weights size=" << weights.getSize();
  if (inputSample.getSize() != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: cannot build a ProjectionStrategyImplementation with samples of different size. Here, input sample size=" << inputSample.getSize() << ", output sample size=" << outputSample.getSize();
  if (inputSample.getDimension() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a ProjectionStrategyImplementation with an input sample of dimension 0.";
  if (outputSample.getDimension() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a ProjectionStrategyImplementation with an output sample of dimension 0.";
  weights_ = weights;
  inputSample_ = inputSample;
  outputSample_ = outputSample;
}

/* Parameter constructor */
ProjectionStrategyImplementation::ProjectionStrategyImplementation(const Distribution & measure,
    const WeightedExperiment & weightedExperiment)
  : PersistentObject()
  , alpha_k_p_(0)
  , residual_p_(0.0)
  , relativeError_p_(0.0)
  , measure_(measure)
  , weightedExperiment_(weightedExperiment)
  , inputSample_(0, 0)
  , weights_(0)
  , outputSample_(0, 0)
{
  // The ProjectionStrategyImplementation imposes the distribution of the weighted experiment
  weightedExperiment_.setDistribution(measure_);
}


/* Virtual constructor */
ProjectionStrategyImplementation * ProjectionStrategyImplementation::clone() const
{
  return new ProjectionStrategyImplementation(*this);
}


/* String converter */
String ProjectionStrategyImplementation::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << " measure=" << measure_;
}


/* Measure accessor */
void ProjectionStrategyImplementation::setMeasure(const Distribution & measure)
{
  if (!(measure == measure_))
  {
    measure_ = measure;
    // Set the measure as the distribution of the weighted experiment
    weightedExperiment_.setDistribution(measure);
    inputSample_ = Sample(0, 0);
  }
}

Distribution ProjectionStrategyImplementation::getMeasure() const
{
  return measure_;
}

/* Experiment accessors */
void ProjectionStrategyImplementation::setExperiment(const WeightedExperiment & weightedExperiment)
{
  // TODO: implement experiments comparison
//   if (!(weightedExperiment == weightedExperiment_))
//   {
  weightedExperiment_ = weightedExperiment;
  weightedExperiment_.setDistribution(getMeasure());
  inputSample_ = Sample(0, 0);
//   }
}

WeightedExperiment ProjectionStrategyImplementation::getExperiment() const
{
  return weightedExperiment_;
}

/* Sample accessors */
void ProjectionStrategyImplementation::setInputSample(const Sample & inputSample)
{
  inputSample_ = inputSample;
}

Sample ProjectionStrategyImplementation::getInputSample() const
{
  return inputSample_;
}

void ProjectionStrategyImplementation::setOutputSample(const Sample & outputSample)
{
  outputSample_ = outputSample;
}

Sample ProjectionStrategyImplementation::getOutputSample() const
{
  return outputSample_;
}

/* Weights accessor */
void ProjectionStrategyImplementation::setWeights(const Point & weights)
{
  weights_ = weights;
}

Point ProjectionStrategyImplementation::getWeights() const
{
  return weights_;
}

/* Residual accessor */
Scalar ProjectionStrategyImplementation::getResidual() const
{
  return residual_p_;
}

/* Relative error accessor */
Scalar ProjectionStrategyImplementation::getRelativeError() const
{
  return relativeError_p_;
}

/* Relative error accessor */
Point ProjectionStrategyImplementation::getCoefficients() const
{
  return alpha_k_p_;
}

/* Compute the components alpha_k_p_ by projecting the model on the partial L2 basis */
void ProjectionStrategyImplementation::computeCoefficients(const Function &,
    const FunctionCollection &,
    const Indices &,
    const Indices &,
    const Indices &,
    const Indices &,
    const UnsignedInteger )
{
  throw NotYetImplementedException(HERE) << "In ProjectionStrategyImplementation::computeCoefficients(const Function & function, const FunctionCollection & basis, const Indices & indices, const Indices & addedRanks, const Indices & conservedRanks, const Indices & removedRanks, const UnsignedInteger marginalIndex)";
}


/* Method save() stores the object through the StorageManager */
void ProjectionStrategyImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void ProjectionStrategyImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
