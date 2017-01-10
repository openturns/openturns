//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all distribution factories
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

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/Exception.hxx"
#include "openturns/BootstrapExperiment.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ParametrizedDistribution.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionFactoryImplementation);

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
String DistributionFactoryImplementation::__str__(const String & offset) const
{
  return this->getClassName();
}


/* Here is the interface that all derived class must implement */

/* Build a distribution based on a sample */
DistributionFactoryImplementation::Implementation DistributionFactoryImplementation::build(const NumericalSample & sample) const
{
  throw NotYetImplementedException(HERE) << "In DistributionFactoryImplementation::build(const NumericalSample & sample) const";
}

/* Build a distribution based on a set of parameters */
DistributionFactoryImplementation::Implementation DistributionFactoryImplementation::build(const NumericalPoint & parameters) const
{
  throw NotYetImplementedException(HERE) << "In DistributionFactoryImplementation::build(const NumericalPointCollection & parameters) const";
}

/* Build a distribution using its default constructor */
DistributionFactoryImplementation::Implementation DistributionFactoryImplementation::build() const
{
  throw NotYetImplementedException(HERE) << "In DistributionFactoryImplementation::build() const";
}

DistributionFactoryResult DistributionFactoryImplementation::buildEstimator(const NumericalSample & sample) const
{
  return buildBootStrapEstimator(sample);
}

DistributionFactoryResult DistributionFactoryImplementation::buildEstimator(const NumericalSample & sample,
    const DistributionParameters & parameters) const
{
  String parametersDistributionName(parameters.getDistribution().getImplementation()->getClassName());
  String referenceDistributionName(build().getImplementation()->getClassName());
  if (parametersDistributionName != referenceDistributionName)
    throw InvalidArgumentException(HERE) << "Cannot use a " << parametersDistributionName
                                         <<" parametrization to build a " << referenceDistributionName;

  DistributionFactoryResult nativeResult(buildEstimator(sample));
  Distribution nativeDistribution(nativeResult.getDistribution());
  DistributionParameters parameters2(parameters);
  NumericalPoint newParameter(parameters.inverse(nativeDistribution.getParameter()));
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
    NumericalSample parameterSample(0, distribution.getParameterDimension());
    for (UnsignedInteger i = 0; i < bootstrapSize; ++ i)
    {
      NumericalSample bootstrapSample(experiment.generate());
      Distribution estimatedDistribution(build(bootstrapSample));
      NumericalPoint newEstimatedParameter(parameters.inverse(estimatedDistribution.getParameter()));
      parameterSample.add(newEstimatedParameter);
    }
    KernelSmoothing factory;
    parameterDistribution = factory.build(parameterSample);
  }
  DistributionFactoryResult result(distribution, parameterDistribution);
  return result;
}

DistributionFactoryResult DistributionFactoryImplementation::buildBootStrapEstimator(const NumericalSample & sample,
    const Bool isGaussian) const
{
  Distribution distribution(build(sample));
  UnsignedInteger bootstrapSize = getBootstrapSize();
  BootstrapExperiment experiment(sample);
  NumericalSample parameterSample(0, distribution.getParameterDimension());
  for (UnsignedInteger i = 0; i < bootstrapSize; ++ i)
  {
    NumericalSample bootstrapSample(experiment.generate());
    Distribution estimatedDistribution(build(bootstrapSample));
    parameterSample.add(estimatedDistribution.getParameter());
  }
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

DistributionFactoryResult DistributionFactoryImplementation::buildMaximumLikelihoodEstimator (const NumericalSample & sample,
    const Bool isRegular) const
{
  const UnsignedInteger size = sample.getSize();
  const Distribution distribution(build(sample));
  const UnsignedInteger parameterDimension = distribution.getParameterDimension();
  Distribution parameterDistribution;
  if (isRegular)
  {
    Matrix theta(parameterDimension, parameterDimension);
    const NumericalSample pdf(distribution.computePDF(sample));
    const NumericalSample dpdf(distribution.computePDFGradient(sample));
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      Matrix dpdfi(parameterDimension, 1, dpdf[i].getCollection());
      dpdfi = dpdfi / pdf[i][0];
      theta = theta + dpdfi * dpdfi.transpose() / size;
    }
    CovarianceMatrix covariance(SymmetricMatrix(theta.getImplementation()).solveLinearSystem(IdentityMatrix(parameterDimension) / size).getImplementation());
    parameterDistribution = Normal(distribution.getParameter(), covariance);
  }
  else
  {
    const UnsignedInteger bootstrapSize = getBootstrapSize();
    BootstrapExperiment experiment(sample);
    NumericalSample parameterSample(0, distribution.getParameterDimension());
    for (UnsignedInteger i = 0; i < bootstrapSize; ++ i)
    {
      NumericalSample bootstrapSample(experiment.generate());
      Distribution estimatedDistribution(build(bootstrapSample));
      parameterSample.add(estimatedDistribution.getParameter());
    }
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


/* Method save() stores the object through the StorageManager */
void DistributionFactoryImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
}

/* Method load() reloads the object from the StorageManager */
void DistributionFactoryImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
}

END_NAMESPACE_OPENTURNS
