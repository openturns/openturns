//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all distribution factories
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
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include "DistributionImplementationFactory.hxx"
#include "Description.hxx"
#include "Path.hxx"
#include "Exception.hxx"
#include "OTconfig.hxx"
#include "Log.hxx"
#include "Os.hxx"
#include "BootstrapExperiment.hxx"
#include "NormalFactory.hxx"
#include "KernelSmoothing.hxx"
#include "SpecFunc.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "CenteredFiniteDifferenceGradient.hxx"
#include "Normal.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DistributionImplementationFactory);

/* Default constructor */
DistributionImplementationFactory::DistributionImplementationFactory(const UnsignedInteger bootstrapSize)
  : PersistentObject()
  , bootstrapSize_(bootstrapSize)
{
  // Nothing to do
}

/* Virtual constructor */
DistributionImplementationFactory * DistributionImplementationFactory::clone() const
{
  return new DistributionImplementationFactory(*this);
}

/* String converter */
String DistributionImplementationFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName();
  return oss;
}

/* String converter */
String DistributionImplementationFactory::__str__(const String & offset) const
{
  return this->getClassName();
}


/* Here is the interface that all derived class must implement */

/* Build a distribution based on a sample */
DistributionImplementationFactory::Implementation DistributionImplementationFactory::build(const NumericalSample & sample) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementationFactory::build(const NumericalSample & sample) const";
}

/* Build a distribution based on a sample and gives the covariance matrix of the estimate */
DistributionImplementationFactory::Implementation DistributionImplementationFactory::build(const NumericalSample & sample,
    CovarianceMatrix & covariance) const
{
  /* The bootstrap sampler */
  BootstrapExperiment bootstrap(sample);
  /* Build the distribution based on the given sample */
  DistributionImplementationFactory::Implementation distribution(build(sample));
  const UnsignedInteger bootstrapSize(ResourceMap::GetAsUnsignedInteger("DistributionImplementationFactory-DefaultBootstrapSize"));
  NumericalSample parametersSample(bootstrapSize, ParametersAsNumericalPoint(distribution->getParametersCollection()));
  for (UnsignedInteger i = 1; i < bootstrapSize; ++i)
  {
    /* Draw a bootstrap sample */
    const NumericalSample bootstrapSample(bootstrap.generate());
    /* Build the associated distribution */
    const DistributionImplementation::Implementation newDistribution(build(bootstrapSample));
    /* Add the parameters to the parameters sample */
    parametersSample.add(ParametersAsNumericalPoint(newDistribution->getParametersCollection()));
  }
  /* Compute the bootstrap covariance */
  covariance = parametersSample.computeCovariance();
  return distribution;
}


/* Build a distribution based on a set of parameters */
DistributionImplementationFactory::Implementation DistributionImplementationFactory::build(const NumericalPoint & parameters) const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementationFactory::build(const NumericalPointCollection & parameters) const";
}

/* Build a distribution using its default constructor */
DistributionImplementationFactory::Implementation DistributionImplementationFactory::build() const
{
  throw NotYetImplementedException(HERE) << "In DistributionImplementationFactory::build() const";
}

DistributionFactoryResult DistributionImplementationFactory::buildEstimator(const NumericalSample & sample) const
{
  return buildBootStrapEstimator(sample);
}

DistributionFactoryResult DistributionImplementationFactory::buildBootStrapEstimator(const NumericalSample & sample,
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
  if (isGaussian) {
    NormalFactory factory;
    parameterDistribution = factory.build(parameterSample);
  } else {
    KernelSmoothing factory;
    parameterDistribution = factory.build(parameterSample);
  }
  DistributionFactoryResult result(distribution, parameterDistribution);
  return result;
}

DistributionFactoryResult DistributionImplementationFactory::buildMaximumLikelihoodEstimator (const NumericalSample & sample,
                                                                                              const Bool isRegular) const
{
  const UnsignedInteger size = sample.getSize();
  const Distribution distribution(build(sample));
  const UnsignedInteger parameterDimension = distribution.getParameterDimension();
  Distribution parameterDistribution;
  if (isRegular) {
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
  } else {
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

void DistributionImplementationFactory::setKnownParameter(const NumericalPoint & values,
                                                          const Indices & indices)
{
  if (knownParameterValues_.getSize() != knownParameterIndices_.getSize()) throw InvalidArgumentException(HERE);
  knownParameterValues_ = values;
  knownParameterIndices_ = indices;
}

Indices DistributionImplementationFactory::getKnownParameterIndices() const
{
  return knownParameterIndices_;
}

NumericalPoint DistributionImplementationFactory::getKnownParameterValues() const
{
  return knownParameterValues_;
}


/* Convert a NumericalPointWithDescriptionCollection into a NumericalPointCollection */
DistributionImplementationFactory::NumericalPointCollection DistributionImplementationFactory::RemoveDescriptionFromCollection(const NumericalPointWithDescriptionCollection & coll)
{
  const UnsignedInteger size(coll.getSize());
  NumericalPointCollection newColl(size);
  for (UnsignedInteger i = 0; i < size; ++i) newColl[i] = coll[i];
  return newColl;
}

/* Convert a NumericalPointCollection into a NumericalPointWithDescriptionCollection */
DistributionImplementationFactory::NumericalPointWithDescriptionCollection DistributionImplementationFactory::AddDescriptionToCollection(const NumericalPointCollection & coll)
{
  const UnsignedInteger size(coll.getSize());
  NumericalPointWithDescriptionCollection newColl(size);
  for (UnsignedInteger i = 0; i < size; ++i) newColl[i] = coll[i];
  return newColl;
}

/* Convert a parameters collection into a NumericalPoint */
NumericalPoint DistributionImplementationFactory::ParametersAsNumericalPoint(const NumericalPointWithDescriptionCollection & parameters)
{
  const UnsignedInteger size(parameters.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot convert an empty collection of parameters.";
  // Early exit if dimension 1
  if (size == 1) return parameters[0];
  // Concatenate the parameters
  NumericalPoint allParameters(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint marginalParameters(parameters[i]);
    const UnsignedInteger marginalSize(marginalParameters.getSize());
    for (UnsignedInteger j = 0; j < marginalSize; ++j) allParameters.add(marginalParameters[j]);
  }
  return allParameters;
}

/* Bootstrap size accessor */
UnsignedInteger DistributionImplementationFactory::getBootstrapSize() const
{
  return bootstrapSize_;
}

void DistributionImplementationFactory::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  if (bootstrapSize == 0) throw InvalidArgumentException(HERE) << "Error: the bootstrap size must be > 0.";
  bootstrapSize_ = bootstrapSize;
}

END_NAMESPACE_OPENTURNS
