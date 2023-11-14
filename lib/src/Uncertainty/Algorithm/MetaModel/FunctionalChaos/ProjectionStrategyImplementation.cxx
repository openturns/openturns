//                                               -*- C++ -*-
/**
 *  @brief This is a abstract class for projection strategy implementations
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Os.hxx"

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
  OSS oss(false);
  oss << "class=" << GetClassName()
      << " alpha_k_p=" << alpha_k_p_
      << " residual=" << residual_p_
      << " relativeError=" << relativeError_p_
      << " measure=" << measure_
      << " weightedExperiment=" << weightedExperiment_
      << " inputSample_=" << inputSample_
      << " outputSample=" << outputSample_
      << " weights_=" << weights_
      << " proxy=" << proxy_;
  return oss;
}

/* String converter */
String ProjectionStrategyImplementation::__str__(const String &) const
{
  return __repr_markdown__();
}

/* String converter */
String ProjectionStrategyImplementation::_repr_html_() const
{
  OSS oss(false);
  oss << GetClassName() << "\n"
      << "<ul>\n"
      << "  <li>coefficients: dimension=" << alpha_k_p_.getDimension() << "</li>\n"
      << "  <li>residual: " << residual_p_ << "</li>\n"
      << "  <li>relative error: " << relativeError_p_ << "</li>\n"
      << "  <li>measure: " << measure_.getClassName() << "</li>\n"
      << "  <li>weighted experiment: " << weightedExperiment_.getClassName() << "</li>\n"
      << "  <li>input sample: size= " << inputSample_.getSize() << " x dimension= " << inputSample_.getDimension() << "</li>\n"
      << "  <li>output sample: size= " << outputSample_.getSize() << " x dimension= " << outputSample_.getDimension() << "</li>\n"
      << "  <li>weights: dimension= " << weights_.getDimension() << "</li>\n"
      << "  <li>design: size= " << proxy_.getSampleSize() << "</li>\n"
      << "<ul>\n";
  return oss;
}

/* String converter */
String ProjectionStrategyImplementation::__repr_markdown__() const
{
  OSS oss(false);
  oss << GetClassName() << "\n"
      << "- coefficients: dimension=" << alpha_k_p_.getDimension() << "\n"
      << "- residual: " << residual_p_ << "\n"
      << "- relative error: " << relativeError_p_ << "\n"
      << "- measure: " << measure_.getClassName() << "\n"
      << "- weighted experiment: " << weightedExperiment_.getClassName() << "\n"
      << "- input sample: size= " << inputSample_.getSize() << " x dimension= " << inputSample_.getDimension() << "\n"
      << "- output sample: size= " << outputSample_.getSize() << " x dimension= " << outputSample_.getDimension() << "\n"
      << "- weights: dimension= " << weights_.getDimension() << "\n"
      << "- design: size= " << proxy_.getSampleSize() << "\n";
  return oss;
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

/* Design proxy accessor */
DesignProxy ProjectionStrategyImplementation::getDesignProxy() const
{
  return proxy_;
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

Collection<Indices> ProjectionStrategyImplementation::getSelectionHistory(Collection<Point> & /*coefficientsHistory*/) const
{
  throw NotYetImplementedException(HERE) << "in ProjectionStrategyImplementation::getSelectionHistory";
}

END_NAMESPACE_OPENTURNS
