//                                               -*- C++ -*-
/**
 *  @brief OrderStatisticsMarginalChecker class
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
#include "openturns/OrderStatisticsMarginalChecker.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/FiniteDifferenceStep.hxx"
#include "openturns/BlendedStep.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TNC.hxx"
#include "openturns/OptimizationSolver.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OrderStatisticsMarginalChecker);

/* Parameters constructor */
OrderStatisticsMarginalChecker::OrderStatisticsMarginalChecker(const DistributionCollection & collection)
  : Object()
  , solver_(new TNC())
{
  for (UnsignedInteger i = 0; i < collection.getSize(); ++ i)
  {
    if (collection[i].getDimension() != 1) throw InvalidDimensionException(HERE) << "Marginal " << i << " should be 1-d.";
    if (!collection[i].isContinuous()) throw InvalidArgumentException(HERE) << "Marginal " << i << " should be continuous.";
  }
  collection_ = collection;
}

struct OrderStatisticsMarginalCheckerWrapper
{
  OrderStatisticsMarginalCheckerWrapper(const Distribution & distributionI, const Distribution & distributionIp1)
    : distributionI_(distributionI)
    , distributionIp1_(distributionIp1)
  {
    // Nothing to do
  }

  NumericalPoint computeDelta(const NumericalPoint & point) const
  {
    const NumericalScalar delta = distributionI_.computeCDF(point) - distributionIp1_.computeCDF(point);
    return NumericalPoint(1, delta);
  }

  const Distribution & distributionI_;
  const Distribution & distributionIp1_;
}; // struct OrderStatisticsMarginalCheckerWrapper

void OrderStatisticsMarginalChecker::check() const
{
  UnsignedInteger quantileIteration = ResourceMap::GetAsUnsignedInteger("OrderStatisticsMarginalChecker-QuantileIteration");
  NumericalScalar epsilon = ResourceMap::GetAsNumericalScalar("OrderStatisticsMarginalChecker-OptimizationEpsilon");
  const UnsignedInteger size = collection_.getSize();
  // First test, check the ranges
  NumericalScalar aIm1 = collection_[0].getRange().getLowerBound()[0];
  NumericalScalar bIm1 = collection_[0].getRange().getUpperBound()[0];
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    // check that a_{i-1} <= a_i
    const NumericalScalar aI = collection_[i].getRange().getLowerBound()[0];
    if (aIm1 > aI) throw InvalidArgumentException(HERE) << "margins are not compatible: the lower bound of margin " << i - 1 << " is greater than the lower bound of margin " << i;
    // check that b_{i-1} <= b_i
    const NumericalScalar bI = collection_[i].getRange().getUpperBound()[0];
    if (bIm1 > bI) throw InvalidArgumentException(HERE) << "margins are not compatible: the lower bound of margin " << i - 1 << " is greater than the lower bound of margin " << i;
    aIm1 = aI;
    bIm1 = bI;
  }
  // Second test, check the quantiles at regular levels. Store the values for further tests
  NumericalSample quantiles(size, quantileIteration);
  for (UnsignedInteger k = 0; k < quantileIteration; ++ k)
  {
    const NumericalScalar prob = (k + 1.0) / (quantileIteration + 1.0);
    NumericalScalar qIm1 = collection_[0].computeQuantile(prob)[0];
    quantiles[0][k] = qIm1;
    for (UnsignedInteger i = 1; i < size; ++ i)
    {
      const NumericalScalar qI = collection_[i].computeQuantile(prob)[0];
      if (qIm1 >= qI) throw InvalidArgumentException(HERE) << "margins are not compatible: the quantile=" << qIm1 << " of margin " << i - 1 << " is greater than the quantile=" << qI << " of margin " << i << " at level " << prob;
      quantiles[i][k] = qI;
      qIm1 = qI;
    }
  }
  // Third test, find the minimum of F_i - F_{i+1}

  // Initilalyse Optimization problem
  OptimizationProblem problem;

  const FiniteDifferenceStep step(BlendedStep(NumericalPoint(1, std::pow(SpecFunc::NumericalScalarEpsilon, 1.0 / 3.0)), std::sqrt(SpecFunc::NumericalScalarEpsilon)));
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    const OrderStatisticsMarginalCheckerWrapper wrapper(collection_[i - 1], collection_[i]);
    NumericalMathFunction f(bindMethod<OrderStatisticsMarginalCheckerWrapper, NumericalPoint, NumericalPoint>(wrapper, &OrderStatisticsMarginalCheckerWrapper::computeDelta, 1, 1));
    f.setGradient(CenteredFiniteDifferenceGradient(step, f.getEvaluation()->clone()));

    for (UnsignedInteger k = 0; k < quantileIteration; ++ k)
    {
      const NumericalScalar xMin = quantiles[i - 1][k];
      const NumericalScalar xMax = quantiles[i][k];
      const NumericalScalar xMiddle = 0.5 * (xMin + xMax);

      // Define Optimization problem
      problem.setObjective(f);
      problem.setBounds(Interval(xMin, xMax));
      solver_.setStartingPoint(NumericalPoint(1, xMiddle));
      solver_.setProblem(problem);
      solver_.run();
      const NumericalPoint minimizer(solver_.getResult().getOptimalPoint());
      const NumericalScalar minValue = solver_.getResult().getOptimalValue()[0];

      LOGDEBUG(OSS() << "Optimisation on [" << xMin << ", " << xMax << "] gives " << solver_.getResult());
      if (minValue < epsilon) throw InvalidArgumentException(HERE) << "margins are not compatible: the CDF at x=" << minimizer[0] << " of margin " << i << " is not enough larger than the CDF of margin " << i + 1 << ". Gap is " << minValue << ".";
    } // k
  } // i
}


Bool OrderStatisticsMarginalChecker::isCompatible() const
{
  Bool compatible = true;
  try
  {
    check();
  }
  catch (InvalidArgumentException)
  {
    compatible = false;
  }
  return compatible;
}


Indices OrderStatisticsMarginalChecker::buildPartition() const
{
  Indices partition;
  const UnsignedInteger size = collection_.getSize();
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    // check if b_{i-1} <= a_i
    // so i is in partition <=> (X_0,...,X_{i-1}) is independent from (X_i,...,X_{d-1})
    if (collection_[i - 1].getRange().getUpperBound()[0] <= collection_[i].getRange().getLowerBound()[0]) partition.add(i - 1);
  }
  return partition;
}

/* Optimization solver accessor */
OptimizationSolver OrderStatisticsMarginalChecker::getOptimizationSolver() const
{
  return solver_;
}

void OrderStatisticsMarginalChecker::setOptimizationSolver(const OptimizationSolver & solver)
{
  solver_ = solver;
}

END_NAMESPACE_OPENTURNS
