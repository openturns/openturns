//                                               -*- C++ -*-
/**
 *  @brief Factory for Pareto distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ParetoFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MaximumLikelihoodFactory.hxx"
#include "openturns/MethodOfMomentsFactory.hxx"
#include "openturns/Brent.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/LinearLeastSquares.hxx"
#include "openturns/LeastSquaresProblem.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ParetoFactory)

static const Factory<ParetoFactory> Factory_ParetoFactory;

/* Default constructor */
ParetoFactory::ParetoFactory()
  : DistributionFactoryImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
ParetoFactory * ParetoFactory::clone() const
{
  return new ParetoFactory(*this);
}

/* Here is the interface that all derived class must implement */

Distribution ParetoFactory::build(const Sample & sample) const
{
  return buildAsPareto(sample).clone();
}

Distribution ParetoFactory::build(const Point & parameters) const
{
  return buildAsPareto(parameters).clone();
}

Distribution ParetoFactory::build() const
{
  return buildAsPareto().clone();
}

Pareto ParetoFactory::buildMethodOfMoments(const Sample & sample) const
{
  // numerically inverse the skewness
  const Scalar sigma = sample.computeStandardDeviation()[0];
  if (!SpecFunc::IsNormal(sigma)) throw InvalidArgumentException(HERE) << "Error: cannot build a Pareto distribution if data contains NaN or Inf";
  if (sigma == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a Pareto distribution from a constant sample.";
  const Scalar skewness = sample.computeSkewness()[0];
  const SymbolicFunction constraint("alpha", OSS() << "2*(1+alpha)/(alpha-3)*sqrt((alpha-2)/alpha)");
  if (skewness < constraint(Point(1, ResourceMap::GetAsScalar("ParetoFactory-AlphaUpperBound")))[0])
    throw InvalidArgumentException(HERE) << "alpha is not defined";
  const Brent solver;
  // alpha> 3.0 so as the skewness to be defined
  const Scalar alpha = solver.solve(constraint, skewness, 3.0 + std::sqrt(SpecFunc::Precision), ResourceMap::GetAsScalar("ParetoFactory-AlphaUpperBound"));
  const Scalar beta = (alpha - 1.0) * std::sqrt((alpha - 2.0) / alpha) * sample.computeStandardDeviation()[0];
  const Scalar gamma = sample.computeMean()[0] - alpha * beta / (alpha - 1.0);

  Pareto result(beta, alpha, gamma);
  result.setDescription(sample.getDescription());
  return result;
}

Pareto ParetoFactory::buildMethodOfLikelihoodMaximization(const Sample & sample) const
{
  const MaximumLikelihoodFactory factory(buildMethodOfMoments(sample));
  return buildAsPareto(factory.build(sample).getParameter());
}

Pareto ParetoFactory::buildMethodOfLeastSquares(const Sample & sample, const Scalar gamma) const
{
  const UnsignedInteger size = sample.getSize();
  Sample y(size, 1);
  Sample z(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    y(i, 0) = std::log(sample(i, 0) - gamma);
    const Scalar survival = sample.computeEmpiricalCDF(sample[i], true);
    if (survival > 0.0)
      z(i, 0) = std::log(survival);
  }
  LinearLeastSquares lls(y, z);
  lls.run();
  const Scalar a0 = lls.getConstant()[0];
  const Scalar a1 = lls.getLinear()(0, 0);
  const Scalar beta = std::exp(-a0 / a1);
  const Scalar alpha = -a1;
  return Pareto(beta, alpha, gamma);
}


class ParetoFactoryResidualEvaluation : public EvaluationImplementation
{
public:
  ParetoFactoryResidualEvaluation(const Sample & sample)
    : EvaluationImplementation()
    , sample_(sample)
  {
    const UnsignedInteger size = sample.getSize();
    dataOut_ = Sample(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      const Scalar survival = sample.computeEmpiricalCDF(sample[i], true);
      if (survival > 0.0)
        dataOut_(i, 0) = std::log(survival);
    }
  }

  ParetoFactoryResidualEvaluation * clone() const
  {
    return new ParetoFactoryResidualEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return sample_.getSize();
  }

  Description getInputDescription() const
  {
    return Description(1, "gamma");
  }

  Description getOutputDescription() const
  {
    return Description(sample_.getSize(), "r");
  }

  Description getDescription() const
  {
    Description description(getInputDescription());
    description.add(getOutputDescription());
    return description;
  }

  Point operator() (const Point & parameter) const
  {
    const Scalar gamma = parameter[0];
    const UnsignedInteger size = sample_.getSize();
    Sample dataIn(size, 1);
    for (UnsignedInteger i = 0; i < size; ++ i)
      dataIn(i, 0) = std::log(sample_(i, 0) - gamma);
    LinearLeastSquares leastSquares(dataIn, dataOut_);
    leastSquares.run();
    const Scalar a0 = leastSquares.getConstant()[0];
    const Scalar a1 = leastSquares.getLinear()(0, 0);
    Point result(size);
    for (UnsignedInteger i = 0; i < size; ++ i)
      result[i] = dataOut_(i, 0) - (a1 * dataIn(i, 0) + a0);
    return result;
  }

private:
  Sample sample_;
  Sample dataOut_;
};


Pareto ParetoFactory::buildMethodOfLeastSquares(const Sample & sample) const
{
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: can only build a LogNormal distribution from a sample of dimension 1, here dimension=" << sample.getDimension();
  const UnsignedInteger size = sample.getSize();
  const Scalar xMin = sample.getMin()[0];
  const Scalar sigma = sample.computeStandardDeviation()[0];
  if (!SpecFunc::IsNormal(sigma)) throw InvalidArgumentException(HERE) << "Error: cannot build a Pareto distribution if data contains NaN or Inf";
  if (sigma == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot estimate a Pareto distribution from a constant sample.";
  Scalar gamma = xMin - std::abs(xMin) / (2 + size);
  ParetoFactoryResidualEvaluation residualEvaluation(sample);
  const Function residualFunction(residualEvaluation.clone());
  LeastSquaresProblem problem(residualFunction);
  OptimizationAlgorithm solver(OptimizationAlgorithm::Build(problem));
  solver.setStartingPoint(Point(1, gamma));
  solver.setVerbose(Log::HasInfo());
  solver.run();
  gamma = solver.getResult().getOptimalPoint()[0];
  return buildMethodOfLeastSquares(sample, gamma);
}


Pareto ParetoFactory::buildAsPareto(const Sample & sample) const
{
  return buildMethodOfLeastSquares(sample);
}

Pareto ParetoFactory::buildAsPareto(const Point & parameters) const
{
  try
  {
    Pareto distribution;
    distribution.setParameter(parameters);
    return distribution;
  }
  catch (InvalidArgumentException &)
  {
    throw InvalidArgumentException(HERE) << "Error: cannot build a Pareto distribution from the given parameters";
  }
}

Pareto ParetoFactory::buildAsPareto() const
{
  return Pareto();
}

END_NAMESPACE_OPENTURNS
