//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all distribution factories
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/BootstrapExperiment.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ParametrizedDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionFactoryImplementation)

static const Factory<DistributionFactoryImplementation> Factory_DistributionFactoryImplementation;

/* Default constructor */
DistributionFactoryImplementation::DistributionFactoryImplementation(const UnsignedInteger bootstrapSize)
  : PersistentObject()
  , bootstrapSize_(bootstrapSize)
{
  // Nothing to do
}

/* Virtual constructor */
DistributionFactoryImplementation * DistributionFactoryImplementation::clone() const
{
  return new DistributionFactoryImplementation(*this);
}

/* String converter */
String DistributionFactoryImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName();
  return oss;
}

/* String converter */
String DistributionFactoryImplementation::__str__(const String & ) const
{
  return this->getClassName();
}


/* Here is the interface that all derived class must implement */

/* Build a distribution based on a sample */
Distribution DistributionFactoryImplementation::build(const Sample & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionFactoryImplementation::build(const Sample & sample) const";
}

/* Build a distribution based on a set of parameters */
Distribution DistributionFactoryImplementation::build(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DistributionFactoryImplementation::build(const PointCollection & parameters) const";
}

/* Build a distribution using its default constructor */
Distribution DistributionFactoryImplementation::build() const
{
  throw NotYetImplementedException(HERE) << "In DistributionFactoryImplementation::build() const";
}

DistributionFactoryResult DistributionFactoryImplementation::buildEstimator(const Sample & sample) const
{
  return buildBootStrapEstimator(sample);
}

DistributionFactoryResult DistributionFactoryImplementation::buildEstimator(const Sample & sample,
    const DistributionParameters & parameters) const
{
  String parametersDistributionName(parameters.getDistribution().getImplementation()->getClassName());
  String referenceDistributionName(build().getImplementation()->getClassName());
  if (parametersDistributionName != referenceDistributionName)
    throw InvalidArgumentException(HERE) << "Cannot use a " << parametersDistributionName
                                         << " parametrization to build a " << referenceDistributionName;

  DistributionFactoryResult nativeResult(buildEstimator(sample));
  Distribution nativeDistribution(nativeResult.getDistribution());
  DistributionParameters parameters2(parameters);
  Point newParameter(parameters.inverse(nativeDistribution.getParameter()));
  parameters2.setValues(newParameter);
  ParametrizedDistribution distribution(parameters2);
  Distribution parameterDistribution;
  Distribution nativeParameterDistribution(nativeResult.getParameterDistribution());
  if (nativeParameterDistribution.getImplementation()->getClassName() == "Normal")
  {
    Matrix delta(parameters2.gradient());
    CovarianceMatrix sigma(nativeParameterDistribution.getCovariance());
    parameterDistribution = Normal(newParameter, CovarianceMatrix((delta * sigma * delta.transpose()).getImplementation()));
  }
  else
  {
    UnsignedInteger bootstrapSize = getBootstrapSize();
    BootstrapExperiment experiment(sample);
    Sample parameterSample(0, distribution.getParameterDimension());
    for (UnsignedInteger i = 0; i < bootstrapSize; ++ i)
    {
      Sample bootstrapSample(experiment.generate());
      try
      {
        Distribution estimatedDistribution(build(bootstrapSample));
        Point newEstimatedParameter(parameters.inverse(estimatedDistribution.getParameter()));
        parameterSample.add(newEstimatedParameter);
      }
      catch (const Exception &)
      {
        // pass
      }
    }
    const Scalar error = (1.0 * bootstrapSize - parameterSample.getSize()) / bootstrapSize;
    if (error > ResourceMap::GetAsScalar("DistributionFactory-BootstrapErrorTolerance"))
    {
      throw NotDefinedException(HERE) << "Too much bootstrap samples errored (" << error << ") in buildEstimator.";
    }
    else if (error > 0.0)
      LOGWARN(OSS() << "Some bootstrap samples errored (" << error << ") in buildEstimator");
    KernelSmoothing factory;
    parameterDistribution = factory.build(parameterSample);
  }
  DistributionFactoryResult result(distribution, parameterDistribution);
  return result;
}

DistributionFactoryResult DistributionFactoryImplementation::buildBootStrapEstimator(const Sample & sample,
    const Bool isGaussian) const
{
  Distribution distribution(build(sample));
  UnsignedInteger bootstrapSize = getBootstrapSize();
  BootstrapExperiment experiment(sample);
  Sample parameterSample(0, distribution.getParameterDimension());
  for (UnsignedInteger i = 0; i < bootstrapSize; ++ i)
  {
    Sample bootstrapSample(experiment.generate());
    try
    {
      Distribution estimatedDistribution(build(bootstrapSample));
      parameterSample.add(estimatedDistribution.getParameter());
    }
    catch (const Exception &)
    {
      // pass
    }
  }
  const Scalar error = (1.0 * bootstrapSize - parameterSample.getSize()) / bootstrapSize;
  if (error > ResourceMap::GetAsScalar("DistributionFactory-BootstrapErrorTolerance"))
  {
    throw NotDefinedException(HERE) << "Too much bootstrap samples errored (" << error << ") in buildEstimator.";
  }
  else if (error > 0.0)
    LOGWARN(OSS() << "Some bootstrap samples errored (" << error << ") in buildEstimator");
  Distribution parameterDistribution;
  if (isGaussian)
  {
    NormalFactory factory;
    parameterDistribution = factory.build(parameterSample);
  }
  else
  {
    KernelSmoothing factory;
    parameterDistribution = factory.build(parameterSample);
  }
  DistributionFactoryResult result(distribution, parameterDistribution);
  return result;
}

/* Bootstrap size accessor */
UnsignedInteger DistributionFactoryImplementation::getBootstrapSize() const
{
  return bootstrapSize_;
}

void DistributionFactoryImplementation::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  if (bootstrapSize == 0) throw InvalidArgumentException(HERE) << "Error: the bootstrap size must be > 0.";
  bootstrapSize_ = bootstrapSize;
}


void DistributionFactoryImplementation::setKnownParameter(const Point & values, const Indices & indices)
{
  LOGWARN("DistributionFactory.setKnownParameter(Point, Indices) is deprecated");
  setKnownParameter(indices, values);
}

void DistributionFactoryImplementation::setKnownParameter(const Indices & indices, const Point & values)
{
  if (values.getSize() != indices.getSize())
    throw InvalidArgumentException(HERE) << "Known parameters values and indices must have the same size";
  knownParameterValues_ = values;
  knownParameterIndices_ = indices;
}

Indices DistributionFactoryImplementation::getKnownParameterIndices() const
{
  return knownParameterIndices_;
}

Point DistributionFactoryImplementation::getKnownParameterValues() const
{
  return knownParameterValues_;
}

void DistributionFactoryImplementation::adaptToKnownParameter(const Sample & sample, DistributionImplementation * distribution) const
{
  if (knownParameterValues_.getSize() > 0)
  {
    MaximumLikelihoodFactory factory(*distribution);
    factory.setKnownParameter(knownParameterIndices_, knownParameterValues_);
    distribution->setParameter(factory.build(sample).getParameter());
  }
}

/* Method save() stores the object through the StorageManager */
void DistributionFactoryImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
  adv.saveAttribute("knownParameterValues_", knownParameterValues_);
  adv.saveAttribute("knownParameterIndices_", knownParameterIndices_);
}

/* Method load() reloads the object from the StorageManager */
void DistributionFactoryImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
  if (adv.hasAttribute("knownParameterValues_"))
    adv.loadAttribute("knownParameterValues_", knownParameterValues_);
  if (adv.hasAttribute("knownParameterIndices_"))
    adv.loadAttribute("knownParameterIndices_", knownParameterIndices_);
}

END_NAMESPACE_OPENTURNS
