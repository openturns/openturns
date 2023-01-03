//                                               -*- C++ -*-
/**
 *  @brief OrderStatisticsMarginalChecker class
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OrderStatisticsMarginalChecker.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/CenteredFiniteDifferenceGradient.hxx"
#include "openturns/FiniteDifferenceStep.hxx"
#include "openturns/BlendedStep.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TNC.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(OrderStatisticsMarginalChecker)

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

  Point computeDelta(const Point & point) const
  {
    const Scalar delta = distributionI_.computeCDF(point) - distributionIp1_.computeCDF(point);
    return Point(1, delta);
  }

  const Distribution & distributionI_;
  const Distribution & distributionIp1_;
}; // struct OrderStatisticsMarginalCheckerWrapper

void OrderStatisticsMarginalChecker::check() const
{
  UnsignedInteger quantileIteration = ResourceMap::GetAsUnsignedInteger("OrderStatisticsMarginalChecker-QuantileIteration");
  Scalar epsilon = ResourceMap::GetAsScalar("OrderStatisticsMarginalChecker-OptimizationEpsilon");
  const UnsignedInteger size = collection_.getSize();
  // First test, check the ranges
  Scalar aIm1 = collection_[0].getRange().getLowerBound()[0];
  Scalar bIm1 = collection_[0].getRange().getUpperBound()[0];
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    // check that a_{i-1} <= a_i
    const Scalar aI = collection_[i].getRange().getLowerBound()[0];
    if (aIm1 > aI) throw InvalidArgumentException(HERE) << "margins are not compatible: the lower bound of margin " << i - 1 << " is greater than the lower bound of margin " << i;
    // check that b_{i-1} <= b_i
    const Scalar bI = collection_[i].getRange().getUpperBound()[0];
    if (bIm1 > bI) throw InvalidArgumentException(HERE) << "margins are not compatible: the upper bound of margin " << i - 1 << " is greater than the upper bound of margin " << i;
    aIm1 = aI;
    bIm1 = bI;
  }
  // Second test, check the quantiles at regular levels. Store the values for further tests
  Sample quantiles(size, quantileIteration);
  for (UnsignedInteger k = 0; k < quantileIteration; ++ k)
  {
    const Scalar prob = (k + 1.0) / (quantileIteration + 1.0);
    Scalar qIm1 = collection_[0].computeQuantile(prob)[0];
    quantiles(0, k) = qIm1;
    for (UnsignedInteger i = 1; i < size; ++ i)
    {
      const Scalar qI = collection_[i].computeQuantile(prob)[0];
      if (qIm1 >= qI) throw InvalidArgumentException(HERE) << "margins are not compatible: the quantile=" << qIm1 << " of margin " << i - 1 << " is greater than the quantile=" << qI << " of margin " << i << " at level " << prob;
      quantiles(i, k) = qI;
      qIm1 = qI;
    }
  }
  // Third test, find the minimum of F_i - F_{i+1}

  const FiniteDifferenceStep step(BlendedStep(Point(1, std::pow(SpecFunc::ScalarEpsilon, 1.0 / 3.0)), std::sqrt(SpecFunc::ScalarEpsilon)));
  for (UnsignedInteger i = 1; i < size; ++ i)
  {
    const OrderStatisticsMarginalCheckerWrapper wrapper(collection_[i - 1], collection_[i]);
    Function f(bindMethod<OrderStatisticsMarginalCheckerWrapper, Point, Point>(wrapper, &OrderStatisticsMarginalCheckerWrapper::computeDelta, 1, 1));
    f.setGradient(CenteredFiniteDifferenceGradient(step, f.getEvaluation()));

    for (UnsignedInteger k = 0; k < quantileIteration; ++ k)
    {
      const Scalar xMin = quantiles(i - 1, k);
      const Scalar xMax = quantiles(i, k);
      const Scalar xMiddle = 0.5 * (xMin + xMax);

      // Define Optimization problem
      OptimizationProblem problem(f);
      problem.setBounds(Interval(xMin, xMax));
      solver_.setStartingPoint(Point(1, xMiddle));
      solver_.setProblem(problem);
      solver_.setVerbose(Log::HasInfo());
      solver_.run();
      const Point minimizer(solver_.getResult().getOptimalPoint());
      const Scalar minValue = solver_.getResult().getOptimalValue()[0];

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
  catch (const InvalidArgumentException &)
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
OptimizationAlgorithm OrderStatisticsMarginalChecker::getOptimizationAlgorithm() const
{
  return solver_;
}

void OrderStatisticsMarginalChecker::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}

END_NAMESPACE_OPENTURNS
