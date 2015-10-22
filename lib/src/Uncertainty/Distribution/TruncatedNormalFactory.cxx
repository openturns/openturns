//                                               -*- C++ -*-
/**
 *  @brief Factory for TruncatedNormal distribution
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
#include "TruncatedNormalFactory.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "CenteredFiniteDifferenceGradient.hxx"
#include "SpecFunc.hxx"
#include "TNC.hxx"
#include "OptimizationSolver.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TruncatedNormalFactory);

/* Default constructor */
TruncatedNormalFactory::TruncatedNormalFactory()
  : DistributionImplementationFactory()
  , solver_(new TNC())
{
  // Initialize optimization solver parameter using the ResourceMap 
  solver_.setMaximumIterationsNumber(ResourceMap::GetAsUnsignedInteger("TruncatedNormalFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("TruncatedNormalFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsNumericalScalar("TruncatedNormalFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsNumericalScalar("TruncatedNormalFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsNumericalScalar("TruncatedNormalFactory-MaximumConstraintError"));
}

/* Virtual constructor */
TruncatedNormalFactory * TruncatedNormalFactory::clone() const
{
  return new TruncatedNormalFactory(*this);
}


/* Here is the interface that all derived class must implement */

TruncatedNormalFactory::Implementation TruncatedNormalFactory::build(const NumericalSample & sample) const
{
  return buildAsTruncatedNormal(sample).clone();
}

TruncatedNormalFactory::Implementation TruncatedNormalFactory::build(const NumericalPoint & parameters) const
{
  return buildAsTruncatedNormal(parameters).clone();
}

TruncatedNormalFactory::Implementation TruncatedNormalFactory::build() const
{
  return buildAsTruncatedNormal().clone();
}

struct TruncatedNormalFactoryLogLikelihood
{
  /** Constructor from a sample */
  TruncatedNormalFactoryLogLikelihood(const NumericalSample & sample)
    : sample_(sample)
  {
    // Nothing to do
  };

  NumericalPoint computeLogLikelihood(const NumericalPoint & parameters) const
  {
    try
    {
      const TruncatedNormal distribution(parameters[0], parameters[1], -1.0 - 1.0 / sample_.getSize(), 1.0 + 1.0 / sample_.getSize());
      const NumericalScalar logLikelihood(-distribution.computeLogPDF(sample_).computeMean()[0]);
      if (SpecFunc::IsNormal(logLikelihood)) return NumericalPoint(1, logLikelihood);
      return NumericalPoint(1, SpecFunc::LogMaxNumericalScalar);
    }
    catch (...)
    {
      return NumericalPoint(1, SpecFunc::LogMaxNumericalScalar);
    }
  }

  const NumericalSample & sample_;
};

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal(const NumericalSample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution from an empty sample";
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a TruncatedNormal distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  // In order to avoid numerical stability issues, we normalize the data to [-1, 1]
  const NumericalScalar xMin(sample.getMin()[0]);
  const NumericalScalar xMax(sample.getMax()[0]);
  if (xMin == xMax) throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution based on a constant sample.";
  // X_norm = alpha * (X - beta)
  const NumericalScalar alpha(2.0 / (xMax - xMin));
  const NumericalScalar beta(0.5 * (xMin + xMax));
  NumericalSample normalizedSample(sample);
  normalizedSample -= NumericalPoint(1, beta);
  normalizedSample *= NumericalPoint(1, alpha);
  TruncatedNormalFactoryLogLikelihood logLikelihoodWrapper(normalizedSample);
  NumericalMathFunction logLikelihood(bindMethod<TruncatedNormalFactoryLogLikelihood, NumericalPoint, NumericalPoint>(logLikelihoodWrapper, &TruncatedNormalFactoryLogLikelihood::computeLogLikelihood, 2, 1));
  CenteredFiniteDifferenceGradient gradient(1.0e-5, logLikelihood.getEvaluation());
  logLikelihood.setGradient(gradient);

  NumericalPoint parametersLowerBound(1, -SpecFunc::MaxNumericalScalar);
  parametersLowerBound.add(ResourceMap::GetAsNumericalScalar( "TruncatedNormalFactory-SigmaLowerBound"));
  NumericalPoint parametersUpperBound(2, SpecFunc::MaxNumericalScalar);
  Interval::BoolCollection parametersLowerFlags(1, false);
  parametersLowerFlags.add(true);
  Interval::BoolCollection parametersUpperFlags(2, false);
  NumericalPoint startingPoint(2);
  startingPoint[0] = normalizedSample.computeMean()[0];
  startingPoint[1] = normalizedSample.computeStandardDeviationPerComponent()[0];

  // Define Optimization problem
  OptimizationProblem problem;
  problem.setObjective(logLikelihood);
  problem.setBounds(Interval(parametersLowerBound, parametersUpperBound, parametersLowerFlags, parametersUpperFlags));
  solver_.setProblem(problem);
  solver_.setStartingPoint(startingPoint);
  solver_.run();
  const NumericalPoint parameters(solver_.getResult().getOptimalPoint());

  // The parameters are scaled back
  // X_norm = alpha * (X - beta)
  // X = beta + X_norm / alpha
  const NumericalScalar mu(beta + parameters[0] / alpha);
  const NumericalScalar sigma(parameters[1] / alpha);
  const NumericalScalar oneEps(1.0 + 1.0 / size);
  const NumericalScalar a(beta - oneEps / alpha);
  const NumericalScalar b(beta + oneEps / alpha);
  // Next, we try to build a TruncatedNormal distribution, which checks the parameters validity
  try
  {
    TruncatedNormal result(mu, sigma, a, b);
    result.setDescription(sample.getDescription());
    return result;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: the estimated parameters values are not valid, mu=" << mu << ", sigma=" << sigma << ", a=" << a << ", b=" << b;
  }
}

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal(const NumericalPoint & parameters) const
{
  try
  {
    TruncatedNormal distribution;
    distribution.setParameters(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a TruncatedNormal distribution from the given parameters";
  }
}

TruncatedNormal TruncatedNormalFactory::buildAsTruncatedNormal() const
{
  return TruncatedNormal();
}

/* Optimization solver accessor */
OptimizationSolver TruncatedNormalFactory::getOptimizationSolver() const
{
  return solver_;
}

void TruncatedNormalFactory::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
}
END_NAMESPACE_OPENTURNS
