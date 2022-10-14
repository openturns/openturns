//                                               -*- C++ -*-
/**
 *  @brief The test file of class HSICEstimatorConditionalSensitivity
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  RandomGenerator::SetSeed(0);
  try
  {
    /* Definition of the marginals */
    Uniform X1(-M_PI, M_PI);
    Uniform X2(-M_PI, M_PI);
    Uniform X3(-M_PI, M_PI);

    ComposedDistribution::DistributionCollection aCollection;
    aCollection.add(X1);
    aCollection.add(X2);
    aCollection.add(X3);

    /* 3d distribution made with independent marginals */
    Distribution distX  = ComposedDistribution(aCollection);

    /* Get a sample of it */
    SignedInteger size = 100;
    Sample X = distX.getSample(size);

    /* The Ishigami model */
    Description input = {"X1", "X2", "X3"};
    SymbolicFunction modelIshigami(input, Description(1, "sin(X1) + 5.0 * (sin(X2))^2 + 0.1 * X3^4 * sin(X1)"));

    /* Apply model: Y = m(X) */
    Sample Y = modelIshigami(X);

    /* We define the covariance models for the HSIC indices. */
    /* For the input, we consider a SquaredExponential covariance model. */
    HSICEstimator::CovarianceModelCollection covarianceModelCollection(0);

    /* Input sample */
    for (UnsignedInteger i = 0; i < 3; ++i)
    {
      Sample Xi = X.getMarginal(i);
      CovarianceModel Cov;
      Cov = SquaredExponential(1);
      Cov.setScale(Xi.computeStandardDeviation());
      covarianceModelCollection.add(Cov);
    }

    /* Output sample with squared exponential covariance */
    CovarianceModel Cov2;
    Cov2 = SquaredExponential(1);
    Cov2.setScale(Y.computeStandardDeviation());
    covarianceModelCollection.add(Cov2);

    /* We define a distance function for the weights
       For the CSA, the critical domain is [5,+inf].

       We set the interval as [5,1000] and make the upper bound
       not finite.
    */
    Interval interval(5, 1000);
    Interval::BoolCollection bcoll;
    bcoll.add(false);
    interval.setFiniteUpperBound(bcoll);
    Function g = DistanceToDomainFunction(interval);

    /* We create the weight function as a composition */
    Scalar stdDev = Y.computeStandardDeviation()[0];
    Description inVar(2);
    inVar[0] = "x";
    inVar[1] = "s";
    SymbolicFunction foo(inVar, Description(1, "exp(-x/s)"));
    Indices set(1);
    set[0] = 1;
    Bool parametersSet = true;
    Point referencePoint(1, 0.1 * stdDev);
    MemoizeFunction fooh(foo);
    ParametricFunction g2(fooh, set, referencePoint, parametersSet);

    /* The weight function */
    Function weight = ComposedFunction(g2, g );

    /* We eventually build the HSIC object!
      HSICVStat event is already embedded as it is the only one available
      for that kind of analysis
    */
    HSICEstimatorConditionalSensitivity CSA(covarianceModelCollection, X, Y, weight);

    /* We get the R2-HSIC */
    Point referenceR2HSIC = {0.0371735, 0.0052413, 0.235519};
    Point R2HSIC = CSA.getR2HSICIndices();
    assert_almost_equal(R2HSIC, referenceR2HSIC);

    /* and the HSIC indices */
    Point referenceHSIC = {0.000640331, 0.000257692, 0.0110516};
    Point HSICIndices = CSA.getHSICIndices();
    assert_almost_equal(HSICIndices, referenceHSIC);

    /* We set the number of permutations for the pvalue estimate. */
    UnsignedInteger b = 100 ;
    CSA.setPermutationSize(b);

    /* We get the pvalue estimate by permutations */
    Point referencePValues = {0.742574, 0.940594, 0};
    Point pvaluesPerm = CSA.getPValuesPermutation();
    assert_almost_equal(pvaluesPerm, referencePValues);

    /* Change weight function and recompute everything */
    SymbolicFunction squaredExponential("x", "exp(-x^2)");
    ComposedFunction alternateWeight(squaredExponential, g);
    CSA.setWeightFunction(alternateWeight);
    assert_almost_equal(CSA.getR2HSICIndices(), {0.0910527, 0.00738055, 0.166624});
    assert_almost_equal(CSA.getHSICIndices(), {0.00218376, 0.000419288, 0.00898721});
    assert_almost_equal(CSA.getPValuesPermutation(), {0.287129, 0.881188, 0.00000000});

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
