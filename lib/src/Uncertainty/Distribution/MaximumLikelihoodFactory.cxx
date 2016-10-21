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
#include "openturns/Matrix.hxx"
#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/NumericalMathGradientImplementation.hxx"

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

class LogLikelihoodEvaluation : public NumericalMathEvaluationImplementation
{
public:
  LogLikelihoodEvaluation(const NumericalSample & sample,
                                        const Distribution & distribution,
                                        const NumericalPoint & knownParameterValues,
                                        const Indices & knownParameterIndices,
                                        const Bool & isParallel)
    : NumericalMathEvaluationImplementation()
    , sample_(sample)
    , distribution_(distribution)
    , knownParameterValues_(knownParameterValues)
    , knownParameterIndices_(knownParameterIndices)
  {
    distribution_.getImplementation()->setParallel(isParallel);
  }

  LogLikelihoodEvaluation * clone() const
  {
    return new LogLikelihoodEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return distribution_.getParameterDimension();
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(getInputDimension(), "theta");
  }

  Description getOutputDescription() const
  {
    return Description(1, "lh");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  NumericalPoint operator() (const NumericalPoint & parameter) const
  {
    NumericalScalar result = 0.0;
    NumericalPoint effectiveParameter(parameter);
    UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
    {
      effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
    }
    Distribution distribution(distribution_);
    distribution.setParameter(effectiveParameter);
    // Take into account the mean over sample
    // Parallelization (evaluation over a sample) is handeled by distribution_
    const NumericalScalar logLikelihood = distribution.computeLogPDF(sample_).computeMean()[0];
    result = SpecFunc::IsNormal(logLikelihood) ? logLikelihood : SpecFunc::LogMinNumericalScalar;
    return NumericalPoint(1, result);
  }

private:
  NumericalSample sample_;
  Distribution distribution_;
  NumericalPoint knownParameterValues_;
  Indices knownParameterIndices_;
};

class LogLikelihoodGradient : public NumericalMathGradientImplementation
{
public:
  LogLikelihoodGradient(const NumericalSample & sample,
                                        const Distribution & distribution,
                                        const NumericalPoint & knownParameterValues,
                                        const Indices & knownParameterIndices,
                                        const Bool & isParallel)
    : NumericalMathGradientImplementation()
    , sample_(sample)
    , distribution_(distribution)
    , knownParameterValues_(knownParameterValues)
    , knownParameterIndices_(knownParameterIndices)
  {
    distribution_.getImplementation()->setParallel(isParallel);
  }

  LogLikelihoodGradient * clone() const
  {
    return new LogLikelihoodGradient(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return distribution_.getParameterDimension();
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  Description getInputDescription() const
  {
    return Description::BuildDefault(getInputDimension(), "theta");
  }

  Description getOutputDescription() const
  {
    return Description(1, "lhG");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  Matrix gradient(const NumericalPoint & parameter) const
  {
    // Define conditionned distribution
    NumericalPoint effectiveParameter(parameter);
    UnsignedInteger knownParametersSize = knownParameterIndices_.getSize();
    for (UnsignedInteger j = 0; j < knownParametersSize; ++ j)
    {
      effectiveParameter[knownParameterIndices_[j]] = knownParameterValues_[j];
    }
    Distribution distribution(distribution_);
    distribution.setParameter(effectiveParameter);
    // Evaluate the gradient
    // Matrix result
    MatrixImplementation result(parameter.getSize(), 1);
    const UnsignedInteger size = sample_.getSize();
    // Compute sum(LogPDFGraident(sample)) ~> size * mean
    // parallelization handeled by distribution::computeLogPDFGradient
    const NumericalPoint logPdfGradient = distribution.computeLogPDFGradient(sample_).computeMean();
    result = MatrixImplementation(getInputDimension(), 1, logPdfGradient);
    return result;
  }

private:
  NumericalSample sample_;
  Distribution distribution_;
  NumericalPoint knownParameterValues_;
  Indices knownParameterIndices_;
};

NumericalPoint MaximumLikelihoodFactory::buildParameter(const NumericalSample & sample) const
{
  if (sample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();

  UnsignedInteger parameterDimension = distribution_.getParameterDimension();
  // Define NumericalMathEvaluation using the LogLikelihoodEvaluation wrapper
  LogLikelihoodEvaluation logLikelihoodWrapper(sample, distribution_, knownParameterValues_, knownParameterIndices_, isParallel_);
  NumericalMathFunction logLikelihood(logLikelihoodWrapper.clone());
  // Define NumericalMathGradient using the LogLikelihoodEvaluation wrapper
  LogLikelihoodGradient logLikelihoodWrapperGradient(sample, distribution_, knownParameterValues_, knownParameterIndices_, isParallel_);
  CenteredFiniteDifferenceGradient gradient(ResourceMap::GetAsNumericalScalar("MaximumLikelihoodFactory-GradientStep"), logLikelihood.getEvaluation());
  logLikelihood.setGradient(logLikelihoodWrapperGradient.clone());
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
