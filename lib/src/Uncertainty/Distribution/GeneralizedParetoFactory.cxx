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
#include "openturns/MethodBoundEvaluation.hxx"
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
  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumAbsoluteError"));
  solver_.setMaximumRelativeError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumRelativeError"));
  solver_.setMaximumResidualError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumObjectiveError"));
  solver_.setMaximumConstraintError(ResourceMap::GetAsScalar("GeneralizedParetoFactory-MaximumConstraintError"));
}

/* Virtual constructor */
GeneralizedParetoFactory * GeneralizedParetoFactory::clone() const
{
  return new GeneralizedParetoFactory(*this);
}

/* Here is the interface that all derived class must implement */

GeneralizedParetoFactory::Implementation GeneralizedParetoFactory::build(const Sample & sample) const
{
  return buildAsGeneralizedPareto(sample).clone();
}

GeneralizedParetoFactory::Implementation GeneralizedParetoFactory::build(const Point & parameters) const
{
  return buildAsGeneralizedPareto(parameters).clone();
}

GeneralizedParetoFactory::Implementation GeneralizedParetoFactory::build() const
{
  return buildAsGeneralizedPareto().clone();
}

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto(const Sample & sample) const
{
  if (sample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: can build a GeneralizedPareto distribution only from a sample of dimension 1, here dimension=" << sample.getDimension();
  const UnsignedInteger size = sample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution from an empty sample";
  Scalar xMin = sample.getMin()[0];
  if (!(xMin > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot build a GeneralizedPareto distribution based on a sample with nonpositive values.";
  Scalar smallSize = ResourceMap::GetAsUnsignedInteger( "GeneralizedParetoFactory-SmallSize" );
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

GeneralizedPareto GeneralizedParetoFactory::buildAsGeneralizedPareto(const Point & parameters) const
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
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfMoments(const Sample & sample) const
{
  const Scalar mean = sample.computeMean()[0];
  const Scalar std = sample.computeStandardDeviationPerComponent()[0];
  const Scalar xi = 0.5 * (std::pow(mean / std, 2) - 1.0);
  // The moment estimator is valid only if the estimated xi parameter is greater than -1/4
  if (xi <= -0.25) throw InvalidArgumentException(HERE) << "Error: cannot estimate a GeneralizedPareto distribution with the method of moments when the estimated xi parameter=" << xi << " is less than -0.25";
  const Scalar sigma = 0.5 * mean * (std::pow(mean / std, 2) + 1.0);
  GeneralizedPareto result(sigma, xi);
  result.setDescription(sample.getDescription());
  return result;
}

struct GeneralizedParetoFactoryParameterConstraint
{
  /** Constructor from a sample and a derivative factor estimate */
  GeneralizedParetoFactoryParameterConstraint(const Sample & sample)
    : sampleY_(0, 1)
    , size_(sample.getSize())
  {
    const Sample sortedSample(sample.sort(0));
    sampleY_ = Sample(size_ - 2, 1);
    const Scalar xMin = sortedSample[0][0];
    for (UnsignedInteger j = 0; j < size_ - 2; ++j)
    {
      const Scalar xNm1mJ = sortedSample[size_ - 1 - j][0];
      const Scalar xNm2mJ = sortedSample[size_ - 2 - j][0];
      if ((xNm1mJ == xMin) || (xNm2mJ == xMin)) sampleY_[j][0] = (j + 1.0) * SpecFunc::LogMaxScalar;
      else sampleY_[j][0] = (j + 1.0) * std::log((xNm1mJ - xMin) / (xNm2mJ - xMin));
    }
  };

  Point computeConstraint(const Point & parameter) const
  {
    const Scalar gamma = parameter[0];
    // Separate the small gamma case for stability purpose
    if (std::abs(gamma) < 1.0e-4)
    {
      Scalar exponentialRegressionLogLikelihood = 0.0;
      for (UnsignedInteger j = 0; j < size_ - 2; ++j)
      {
        const Scalar logAlphaJ = std::log((j + 1.0) / size_);
        const Scalar gammaLogAlphaJ = gamma * logAlphaJ;
        const Scalar yLogAlphaJ = sampleY_[j][0] * logAlphaJ;
        exponentialRegressionLogLikelihood += std::log(-logAlphaJ) + yLogAlphaJ + 0.5 * gammaLogAlphaJ * (1.0 + yLogAlphaJ + gammaLogAlphaJ * (1.0 / 12.0 + yLogAlphaJ / 3.0 + gammaLogAlphaJ * yLogAlphaJ / 12.0));
      }
      return Point(1, -exponentialRegressionLogLikelihood);
    }
    // Large gamma case
    Scalar exponentialRegressionLogLikelihood = 0.0;
    for (UnsignedInteger j = 0; j < size_ - 2; ++j)
    {
      const Scalar alphaJ = (1.0 - std::pow((j + 1.0) / size_, gamma)) / gamma;
      exponentialRegressionLogLikelihood += std::log(alphaJ) - alphaJ * sampleY_[j][0];
    }
    return Point(1, -exponentialRegressionLogLikelihood);
  }

  Sample sampleY_;
  UnsignedInteger size_;
};

/* Algorithm associated with the method of exponential regression */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfExponentialRegression(const Sample & sample) const
{
  GeneralizedParetoFactoryParameterConstraint constraint(sample);
  Function f(bindMethod<GeneralizedParetoFactoryParameterConstraint, Point, Point>(constraint, &GeneralizedParetoFactoryParameterConstraint::computeConstraint, 1, 1));
  CenteredFiniteDifferenceGradient gradient(1.0e-5, f.getEvaluation());
  f.setGradient(gradient);

  // Define Optimization problem
  OptimizationProblem problem;
  problem.setObjective(f);

  const UnsignedInteger dimension = problem.getObjective().getInputDimension();
  Point parametersLowerBound(dimension, -1.0);
  Point parametersUpperBound(dimension,  1.0);
  problem.setBounds(Interval(parametersLowerBound, parametersUpperBound, Interval::BoolCollection(dimension, 0), Interval::BoolCollection(dimension, 0)));

  solver_.setProblem(problem);
  solver_.setStartingPoint(Point(dimension, 0.0));

  // run Optimization problem
  solver_.run();

  // optimal point
  const Scalar xi = solver_.getResult().getOptimalPoint()[0];

  const Scalar mean = sample.computeMean()[0];
  const Sample sortedSample(sample.sort(0));
  // Compute the first probability weighted moment
  Scalar m = 0.0;
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) m += (size - (i + 0.65)) * sortedSample[i][0];
  m /= size * size;
  const Scalar sigma = 2.0 * mean * m / (mean - 2.0 * m);
  GeneralizedPareto result(sigma, xi);
  result.setDescription(sample.getDescription());
  return result;
}

/* Algorithm associated with the method of modified moments */
GeneralizedPareto GeneralizedParetoFactory::buildMethodOfProbabilityWeightedMoments(const Sample & sample) const
{
  const Scalar mean = sample.computeMean()[0];
  const Sample sortedSample(sample.sort(0));
  // Compute the first probability weighted moment
  Scalar m = 0.0;
  const UnsignedInteger size = sample.getSize();
  for (UnsignedInteger i = 0; i < size; ++i) m += (size - (i + 0.65)) * sortedSample[i][0];
  m /= size * size;
  const Scalar xi = mean / (mean - 2.0 * m) - 2.0;
  if (xi <= -0.5) throw InvalidArgumentException(HERE) << "Error: cannot estimate a GeneralizedPareto distribution with the method of probability weighted moments when the estimated xi parameter=" << xi << " is less than -0.5";
  const Scalar sigma = 2.0 * mean * m / (mean - 2.0 * m);
  GeneralizedPareto result(sigma, xi);
  result.setDescription(sample.getDescription());
  return result;
}

/* Optimization solver accessor */
OptimizationAlgorithm GeneralizedParetoFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void GeneralizedParetoFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  Log::Warn(OSS() << "GeneralizedParetoFactory::setOptimizationSolver is deprecated");
  solver_ = solver;
}

OptimizationAlgorithm GeneralizedParetoFactory::getOptimizationSolver() const
{
  Log::Warn(OSS() << "GeneralizedParetoFactory::getOptimizationSolver is deprecated");
  return getOptimizationAlgorithm();
}

void GeneralizedParetoFactory::setOptimizationSolver(const OptimizationAlgorithm & solver)
{
  setOptimizationAlgorithm(solver);
}

END_NAMESPACE_OPENTURNS
