//                                               -*- C++ -*-
/**
 *  @brief Factory for GeneralizedPareto distribution
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
#include "openturns/GeneralizedParetoFactory.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TNC.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GeneralizedParetoFactory);

static const Factory<GeneralizedParetoFactory> Factory_GeneralizedParetoFactory;

/* Default constructor */
GeneralizedParetoFactory::GeneralizedParetoFactory()
  : DistributionFactoryImplementation()
  , solver_(new TNC())
{
  // Create the optimization solver parameters using the parameters in the ResourceMap
  solver_.setMaximumIterationNumber(ResourceMap::GetAsUnsignedInteger("GeneralizedParetoFactory-MaximumEvaluationNumber"));
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsNumericalScalar("GeneralizedParetoFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsNumericalScalar("GeneralizedParetoFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsNumericalScalar("GeneralizedParetoFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsNumericalScalar("GeneralizedParetoFactory-MaximumConstraintError"));
}

/* Virtual constructor */
GeneralizedParetoFactory * GeneralizedParetoFactory::clone() const
{
  return new GeneralizedParetoFactory(*this);
}

/* Here is the interface that all derived class must implement */

GeneralizedParetoFactory::Implementation GeneralizedParetoFactory::build(const NumericalSample & sample) const
{
  return buildAsGeneralizedPareto(sample).clone();
}

GeneralizedParetoFactory::Implementation GeneralizedParetoFactory::build(const NumericalPoint & parameters) const
{
  return buildAsGeneralizedPareto(parameters).clone();
}

GeneralizedParetoFactory::Implementation GeneralizedParetoFactory::build() const
{
  return buildAsGeneralizedPareto().clone();
}

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto(const NumericalSample & sample) const
{
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from an empty sample";
  NumericalScalar xMin = sample.getMin()[0];
  if (xMin <= 0.0) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution based on a sample with nonpositive values.";
  NumericalScalar smallSize = ResourceMap::GetAsUnsignedInteger( "GeneralizedParetoFactory-SmallSize" );
  // The strategy is to use the probability weighted moment method for small size and to switch to the maximum likelihood if the estimator is not defined. For large size, we only use the ML estimator.
  if (size <= smallSize)
  {
    try
    {
      return buildMethodOfProbabilityWeightedMoments(sample);
    }
    catch (InvalidArgumentException)
    {
      // Nothing to do, fallback on the exponential regression
    }
  } // small size
  return buildMethodOfExponentialRegression(sample);
}

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto(const NumericalPoint & parameters) const
{
  try
  {
    GeneralizedPareto distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from the given parameters";
  }
}

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto() const
{
  return GeneralizedPareto();
}

/* Algorithm associated with the method of moments */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfMoments(const NumericalSample & sample) const
{
  const NumericalScalar mean = sample.computeMean()[0];
  const NumericalScalar std = sample.computeStandardDeviationPerComponent()[0];
  const NumericalScalar xi = 0.5 * (std::pow(mean / std, 2) - 1.0);
  // The moment estimator is valid only if the estimated xi parameter is greater than -1/4
  if (xi <= -0.25) throw InvalidArgumentException(HERE) << "Error: cannot estimate a GeneralizedPareto distribution with the method of moments when the estimated xi parameter=" << xi << " is less than -0.25";
  const NumericalScalar sigma = 0.5 * mean * (std::pow(mean / std, 2) + 1.0);
  GeneralizedPareto result(sigma, xi);
  result.setDescription(sample.getDescription());
  return result;
}

struct GeneralizedParetoFactoryParameterConstraint
{
  /** Constructor from a sample and a derivative factor estimate */
  GeneralizedParetoFactoryParameterConstraint(const NumericalSample & sample)
    : sampleY_(0, 1)
    , size_(sample.getSize())
  {
    const NumericalSample sortedSample(sample.sort(0));
    sampleY_ = NumericalSample(size_ - 2, 1);
    const NumericalScalar xMin = sortedSample[0][0];
    for (UnsignedInteger j = 0; j < size_ - 2; ++j)
    {
      const NumericalScalar xNm1mJ = sortedSample[size_ - 1 - j][0];
      const NumericalScalar xNm2mJ = sortedSample[size_ - 2 - j][0];
      if ((xNm1mJ == xMin) || (xNm2mJ == xMin)) sampleY_[j][0] = (j + 1.0) * SpecFunc::LogMaxNumericalScalar;
      else sampleY_[j][0] = (j + 1.0) * std::log((xNm1mJ - xMin) / (xNm2mJ - xMin));
    }
  };

  NumericalPoint computeConstraint(const NumericalPoint & parameter) const
  {
    const NumericalScalar gamma = parameter[0];
    // Separate the small gamma case for stability purpose
    if (std::abs(gamma) < 1.0e-4)
    {
      NumericalScalar exponentialRegressionLogLikelihood = 0.0;
      for (UnsignedInteger j = 0; j < size_ - 2; ++j)
      {
        const NumericalScalar logAlphaJ = std::log((j + 1.0) / size_);
        const NumericalScalar gammaLogAlphaJ = gamma * logAlphaJ;
        const NumericalScalar yLogAlphaJ = sampleY_[j][0] * logAlphaJ;
        exponentialRegressionLogLikelihood += std::log(-logAlphaJ) + yLogAlphaJ + 0.5 * gammaLogAlphaJ * (1.0 + yLogAlphaJ + gammaLogAlphaJ * (1.0 / 12.0 + yLogAlphaJ / 3.0 + gammaLogAlphaJ * yLogAlphaJ / 12.0));
      }
      return NumericalPoint(1, -exponentialRegressionLogLikelihood);
    }
    // Large gamma case
    NumericalScalar exponentialRegressionLogLikelihood = 0.0;
    for (UnsignedInteger j = 0; j < size_ - 2; ++j)
    {
      const NumericalScalar alphaJ = (1.0 - std::pow((j + 1.0) / size_, gamma)) / gamma;
      exponentialRegressionLogLikelihood += std::log(alphaJ) - alphaJ * sampleY_[j][0];
    }
    return NumericalPoint(1, -exponentialRegressionLogLikelihood);
  }

  NumericalSample sampleY_;
  UnsignedInteger size_;
};

/* Algorithm associated with the method of exponential regression */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfExponentialRegression(const NumericalSample & sample) const
{
  GeneralizedParetoFactoryParameterConstraint constraint(sample);
  NumericalMathFunction f(bindMethod<GeneralizedParetoFactoryParameterConstraint, NumericalPoint, NumericalPoint>(constraint, &GeneralizedParetoFactoryParameterConstraint::computeConstraint, 1, 1));
  CenteredFiniteDifferenceGradient gradient(1.0e-5, f.getEvaluation());
  f.setGradient(gradient);

  // Define Optimization problem
  OptimizationProblem problem;
  problem.setObjective(f);

  const UnsignedInteger dimension = problem.getObjective().getInputDimension();
  NumericalPoint parametersLowerBound(dimension, -1.0);
  NumericalPoint parametersUpperBound(dimension,  1.0);
  problem.setBounds(Interval(parametersLowerBound, parametersUpperBound, Interval::BoolCollection(dimension, 0), Interval::BoolCollection(dimension, 0)));

  solver_.setProblem(problem);
  solver_.setStartingPoint(NumericalPoint(dimension, 0.0));

  // run Optimization problem
  solver_.run();

  // optimal point
  const NumericalScalar xi = solver_.getResult().getOptimalPoint()[0];

  const NumericalScalar mean = sample.computeMean()[0];
  const NumericalSample sortedSample(sample.sort(0));
  // Compute the first probability weighted moment
  NumericalScalar m = 0.0;
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) m += (size - (i + 0.65)) * sortedSample[i][0];
  m /= size * size;
  const NumericalScalar sigma = 2.0 * mean * m / (mean - 2.0 * m);
  GeneralizedPareto result(sigma, xi);
  result.setDescription(sample.getDescription());
  return result;
}

/* Algorithm associated with the method of modified moments */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfProbabilityWeightedMoments(const NumericalSample & sample) const
{
  const NumericalScalar mean = sample.computeMean()[0];
  const NumericalSample sortedSample(sample.sort(0));
  // Compute the first probability weighted moment
  NumericalScalar m = 0.0;
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) m += (size - (i + 0.65)) * sortedSample[i][0];
  m /= size * size;
  const NumericalScalar xi = mean / (mean - 2.0 * m) - 2.0;
  if (xi <= -0.5) throw InvalidArgumentException(HERE) << "Error: cannot estimate a GeneralizedPareto distribution with the method of probability weighted moments when the estimated xi parameter=" << xi << " is less than -0.5";
  const NumericalScalar sigma = 2.0 * mean * m / (mean - 2.0 * m);
  GeneralizedPareto result(sigma, xi);
  result.setDescription(sample.getDescription());
  return result;
}

/* Optimization solver accessor */
OptimizationSolver GeneralizedParetoFactory::getOptimizationSolver() const
{
  return solver_;
}

void GeneralizedParetoFactory::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
}


END_NAMESPACE_OPENTURNS
