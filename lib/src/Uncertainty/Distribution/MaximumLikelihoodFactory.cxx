//                                               -*- C++ -*-
/**
 *  @brief Maximum likelihood estimation
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
#include "MaximumLikelihoodFactory.hxx"
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
#include "TNC.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumLikelihoodFactory);

/* Default constructor */
MaximumLikelihoodFactory::MaximumLikelihoodFactory()
  : DistributionImplementationFactory()
{

}


/* Default constructor */
MaximumLikelihoodFactory::MaximumLikelihoodFactory(const DistributionFactory & factory)
  : DistributionImplementationFactory()
  , factory_(factory)
  , solver_(new TNC())
{
  // Initialize optimization solver parameter using the ResourceMap 
  solver_.setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("MaximumLikelihoodFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-MaximumConstraintError"));
}

/* Virtual constructor */
MaximumLikelihoodFactory * MaximumLikelihoodFactory::clone() const
{
  return new MaximumLikelihoodFactory(*this);
}

/* String converter */
String MaximumLikelihoodFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << this->getClassName();
  return oss;
}

/* String converter */
String MaximumLikelihoodFactory::__str__(const String & offset) const
{
  return this->getClassName();
}


/* Here is the interface that all derived class must implement */

/* Build a distribution based on a sample */
// MaximumLikelihoodFactory::Implementation MaximumLikelihoodFactory::build() const
// {
//   return factory_.getImplementation()->build();
// }
// 
// MaximumLikelihoodFactory::Implementation MaximumLikelihoodFactory::build(const NumericalPoint & parameter) const
// {
//   return factory_.getImplementation()->build(parameter);
// }


struct MaximumLikelihoodFactoryLogLikelihood
{
  MaximumLikelihoodFactoryLogLikelihood(const NumericalSample & sample,
                                        const DistributionFactory & factory,
                                        const NumericalPoint & knownParameterValues,
                                        const Indices & knownParameterIndices)
  : sample_(sample)
  , factory_(factory)
  , knownParameterValues_(knownParameterValues)
  , knownParameterIndices_(knownParameterIndices)
  {
  }

  NumericalScalar computeLogLikelihood(const NumericalPoint & parameter) const
  {
    NumericalScalar result = SpecFunc::LogMaxNumericalScalar;
    try
    {
      NumericalPoint effectiveParameter(parameter);
      UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
      for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
      {
        effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
      }
      const Distribution distribution(factory_.build(effectiveParameter));
      // parallel version breaks TrapezoidalFactory test:
//       const NumericalScalar logLikelihood = -distribution.computeLogPDF(sample_).computeMean()[0];
//       if (SpecFunc::IsNormal(logLikelihood)) result = logLikelihood;
      const UnsignedInteger size = sample_.getSize();
      result = 0.0;
      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        const NumericalScalar logPdf = distribution.computeLogPDF(sample_[i]);
        if (logPdf == -SpecFunc::MaxNumericalScalar) result -= SpecFunc::LogMinNumericalScalar;
        else result -= logPdf;
      }
    }
    catch (...)
    {
    }
    return result;
  }

  const NumericalSample & sample_;
  const DistributionFactory factory_;
  const NumericalPoint knownParameterValues_;
  const Indices knownParameterIndices_;
};


NumericalPoint MaximumLikelihoodFactory::buildParameter(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  UnsignedInteger parameterDimension = factory_.build().getParameterDimension();
  MaximumLikelihoodFactoryLogLikelihood logLikelihoodWrapper(sample, factory_, knownParameterValues_, knownParameterIndices_);

  NumericalMathFunction logLikelihood(bindMethod<MaximumLikelihoodFactoryLogLikelihood, NumericalScalar, NumericalPoint>(logLikelihoodWrapper, &MaximumLikelihoodFactoryLogLikelihood::computeLogLikelihood, parameterDimension, 1));
  CenteredFiniteDifferenceGradient gradient(1.0e-5, logLikelihood.getEvaluation());
  logLikelihood.setGradient(gradient);
  OptimizationProblem problem(problem_);
  problem.setMinimization(true);
  problem.setObjective(logLikelihood);
  OptimizationSolver solver(solver_);
  if (solver.getStartingPoint().getDimension() != parameterDimension) {
    throw InvalidArgumentException(HERE) << "Invalid starting point";
  }
  solver.setProblem(problem);
  solver.run();
  NumericalPoint parameter(solver.getResult().getOptimalPoint());
  UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
  for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
  {
    parameter[knownParameterIndices_[j]] = knownParameterValues_[j];
  }
  return parameter;
}


DistributionImplementationFactory::Implementation MaximumLikelihoodFactory::build(const NumericalSample & sample) const
{
  Distribution result(factory_.build(buildParameter(sample)));
  result.setDescription(sample.getDescription());
  return result.getImplementation();
}


void MaximumLikelihoodFactory::setOptimizationProblem(const OptimizationProblem& problem)
{
  problem_ = problem;
}

OptimizationProblem MaximumLikelihoodFactory::getOptimizationProblem() const
{
  return problem_;
}

void MaximumLikelihoodFactory::setOptimizationSolver(const OptimizationSolver& solver)
{
  solver_ = solver;
}

OptimizationSolver MaximumLikelihoodFactory::getOptimizationSolver() const
{
  return solver_;
}




END_NAMESPACE_OPENTURNS
