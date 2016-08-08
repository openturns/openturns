//                                               -*- C++ -*-
/**
 *  @brief Maximum likelihood estimation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/Description.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/Normal.hxx"
#include "openturns/TNC.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumLikelihoodFactory);

/* Default constructor */
MaximumLikelihoodFactory::MaximumLikelihoodFactory()
  : DistributionFactoryImplementation()
  , isParallel_(ResourceMap::GetAsBool("MaximumLikelihoodFactory-Parallel"))
{
  // Nothing to do
}

/* Parameters constructor */
MaximumLikelihoodFactory::MaximumLikelihoodFactory(const Distribution & distribution)
  : DistributionFactoryImplementation()
  , distribution_(distribution)
  , solver_(new TNC())
  , isParallel_(ResourceMap::GetAsBool("MaximumLikelihoodFactory-Parallel"))
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
  oss << "class=" << this->getClassName()
      << " distribution=" << distribution_
      << " solver=" << solver_;
  return oss;
}

/* String converter */
String MaximumLikelihoodFactory::__str__(const String & offset) const
{
  return this->getClassName();
}


/* Here is the interface that all derived class must implement */


struct MaximumLikelihoodFactoryLogLikelihood
{
  MaximumLikelihoodFactoryLogLikelihood(const NumericalSample & sample,
                                        const Distribution & distribution,
                                        const NumericalPoint & knownParameterValues,
                                        const Indices & knownParameterIndices,
                                        const Bool & isParallel)
    : sample_(sample)
    , distribution_(distribution)
    , knownParameterValues_(knownParameterValues)
    , knownParameterIndices_(knownParameterIndices)
    , isParallel_(isParallel)
  {
    // Nothing to do
  }

  NumericalScalar computeLogLikelihood(const NumericalPoint & parameter) const
  {
    NumericalScalar result = 0.0;
    try
    {
      NumericalPoint effectiveParameter(parameter);
      UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
      for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
      {
        effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
      }
      Distribution distribution(distribution_);
      distribution.setParameter(effectiveParameter);

      if (isParallel_)
      {
        const NumericalScalar logLikelihood = distribution.computeLogPDF(sample_).computeMean()[0];
        result = SpecFunc::IsNormal(logLikelihood) ? logLikelihood : -SpecFunc::MaxNumericalScalar;
      } // Parallel computeLogPDF
      else
      {
        const UnsignedInteger size = sample_.getSize();
        for (UnsignedInteger i = 0; i < size; ++ i)
        {
          const NumericalScalar logPdf = distribution.computeLogPDF(sample_[i]);
          if (logPdf == -SpecFunc::MaxNumericalScalar)
          {
            result = -SpecFunc::MaxNumericalScalar;
            break;
          }
          else
          {
            result += logPdf;
          }
        } // Loop over the realizations
      } // Sequential computeLogPDF
    } // try
    catch (...)
    {
      result = -SpecFunc::MaxNumericalScalar;
    }
    return result;
  }

  const NumericalSample & sample_;
  const Distribution distribution_;
  const NumericalPoint knownParameterValues_;
  const Indices knownParameterIndices_;
  const Bool isParallel_;
};


NumericalPoint MaximumLikelihoodFactory::buildParameter(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  UnsignedInteger parameterDimension = distribution_.getParameterDimension();
  MaximumLikelihoodFactoryLogLikelihood logLikelihoodWrapper(sample, distribution_, knownParameterValues_, knownParameterIndices_, isParallel_);

  NumericalMathFunction logLikelihood(bindMethod<MaximumLikelihoodFactoryLogLikelihood, NumericalScalar, NumericalPoint>(logLikelihoodWrapper, &MaximumLikelihoodFactoryLogLikelihood::computeLogLikelihood, parameterDimension, 1));
  CenteredFiniteDifferenceGradient gradient(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-GradientStep"), logLikelihood.getEvaluation());
  logLikelihood.setGradient(gradient);
  OptimizationProblem problem(problem_);
  problem.setMinimization(false);
  problem.setObjective(logLikelihood);
  OptimizationSolver solver(solver_);
  if (solver.getStartingPoint().getDimension() != parameterDimension)
  {
    LOGINFO(OSS() << "Warning! The given starting point=" << solver.getStartingPoint() << " has a dimension=" << solver.getStartingPoint().getDimension() << " which is different from the expected parameter dimension=" << parameterDimension << ". Switching to the default parameter value=" << distribution_.getParameter());
    solver.setStartingPoint(distribution_.getParameter());
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


DistributionFactoryImplementation::Implementation MaximumLikelihoodFactory::build(const NumericalSample & sample) const
{
  Distribution result(distribution_);
  result.setParameter(buildParameter(sample));
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


void MaximumLikelihoodFactory::setParallel(const Bool parallel)
{
  isParallel_ = parallel;
}


END_NAMESPACE_OPENTURNS
