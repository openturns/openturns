//                                               -*- C++ -*-
/**
 *  @brief Factory for FisherSnedecor distribution
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
#include "FisherSnedecorFactory.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "CenteredFiniteDifferenceGradient.hxx"
#include "SpecFunc.hxx"
#include "TNCObsolete.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FisherSnedecorFactory);

/* Default constructor */
FisherSnedecorFactory::FisherSnedecorFactory():
  DistributionImplementationFactory()
{
  // Nothing to do
}

/* Virtual constructor */
FisherSnedecorFactory * FisherSnedecorFactory::clone() const
{
  return new FisherSnedecorFactory(*this);
}

/* Here is the interface that all derived class must implement */

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build(const NumericalSample & sample) const
{
  return buildAsFisherSnedecor(sample).clone();
}

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build(const NumericalPointCollection & parameters) const
{
  return buildAsFisherSnedecor(parameters).clone();
}

FisherSnedecorFactory::Implementation FisherSnedecorFactory::build() const
{
  return buildAsFisherSnedecor().clone();
}

struct FisherSnedecorFactoryLogLikelihood
{
  /** Constructor from a sample */
  FisherSnedecorFactoryLogLikelihood(const NumericalSample & sample)
    : sample_(sample)
  {
    // Nothing to do
  };

  NumericalPoint computeLogLikelihood(const NumericalPoint & parameters) const
  {
    try
    {
      const FisherSnedecor distribution(parameters[0], parameters[1]);
      const NumericalScalar logLikelihood(-distribution.computeLogPDF(sample_).computeMean()[0]);
      if (SpecFunc::IsNormal(logLikelihood)) return NumericalPoint(1, logLikelihood);
      return NumericalPoint(1, SpecFunc::MaxNumericalScalar);
    }
    catch (...)
    {
      return NumericalPoint(1, SpecFunc::MaxNumericalScalar);
    }
  }

  const NumericalSample & sample_;
};

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a FisherSnedecor distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build an FisherSnedecor distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  FisherSnedecorFactoryLogLikelihood logLikelihoodWrapper(sample);
  NumericalMathFunction logLikelihood(bindMethod<FisherSnedecorFactoryLogLikelihood, NumericalPoint, NumericalPoint>(logLikelihoodWrapper, &FisherSnedecorFactoryLogLikelihood::computeLogLikelihood, 2, 1));
  CenteredFiniteDifferenceGradient gradient(1.0e-5, logLikelihood.getEvaluation());
  logLikelihood.setGradient(gradient);
  // Solver
  TNCObsolete optimizationAlgorithm(logLikelihood);
  optimizationAlgorithm.setMaximumEvaluationsNumber(ResourceMap::GetAsUnsignedInteger( "FisherSnedecorFactory-MaximumEvaluationNumber"));
  optimizationAlgorithm.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar( "FisherSnedecorFactory-MaximumAbsoluteError"));
  optimizationAlgorithm.setMaximumRelativeError(ResourceMap::GetAsNumericalScalar( "FisherSnedecorFactory-MaximumRelativeError"));
  optimizationAlgorithm.setMaximumObjectiveError(ResourceMap::GetAsNumericalScalar( "FisherSnedecorFactory-MaximumObjectiveError"));
  optimizationAlgorithm.setMaximumConstraintError(ResourceMap::GetAsNumericalScalar( "FisherSnedecorFactory-MaximumConstraintError"));
  NumericalPoint parametersLowerBound(0);
  parametersLowerBound.add(ResourceMap::GetAsNumericalScalar( "FisherSnedecorFactory-D1LowerBound"));
  parametersLowerBound.add(ResourceMap::GetAsNumericalScalar( "FisherSnedecorFactory-D2LowerBound"));
  optimizationAlgorithm.setBoundConstraints(Interval(parametersLowerBound, NumericalPoint(2, SpecFunc::MaxNumericalScalar), Interval::BoolCollection(2, true), Interval::BoolCollection(2, false)));
  optimizationAlgorithm.run();
  const NumericalPoint parameters(optimizationAlgorithm.getResult().getOptimizer());
  return FisherSnedecor(parameters[0], parameters[1]);
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const NumericalPointWithDescriptionCollection & parameters) const
{
  return buildAsFisherSnedecor(RemoveDescriptionFromCollection(parameters));
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor(const NumericalPointCollection & parameters) const
{
  try
  {
    FisherSnedecor distribution;
    distribution.setParametersCollection(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a FisherSnedecor distribution from the given parameters";
  }
}

FisherSnedecor FisherSnedecorFactory::buildAsFisherSnedecor() const
{
  return FisherSnedecor();
}


END_NAMESPACE_OPENTURNS
