//                                               -*- C++ -*-
/**
 *  @brief The test file of class HSICEstimatorTargetSensitivity
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

    /* We choose an estimator type :
       - unbiased: HSICUStat;
       - biased: HSICVStat.
    */
    HSICUStat estimatorType;

    /* We define a distance function for the weights
       For the TSA, the critical domain is [5,+inf].

       We set the interval as [5,1000] and make the upper bound
       not finite.
    */
    Interval interval(5, 1000);
    Interval::BoolCollection bcoll;
    bcoll.add(false);
    interval.setFiniteUpperBound(bcoll);
    Function g = DistanceToDomainFunction(interval);


    /* */
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

    /* The filter function */
    Function filter = ComposedFunction(g2, g );

    /* We eventually build the HSIC object! */
    HSICEstimatorTargetSensitivity TSA(covarianceModelCollection, X, Y, estimatorType, filter);

    /* We get the R2-HSIC */
    Point referenceR2HSIC = {0.268637, 0.00468423, 0.00339962};
    Point R2HSIC = TSA.getR2HSICIndices();
    assert_almost_equal(R2HSIC, referenceR2HSIC);

    /* and the HSIC indices */
    Point referenceHSICIndices = {0.00107494, 1.8681e-05, 1.41125e-05};
    Point HSICIndices = TSA.getHSICIndices();
    assert_almost_equal(HSICIndices, referenceHSICIndices);

    /* We get the asymptotic pvalue */
    Point referencePValuesAs = {2.83534e-11, 0.262015, 0.282271};
    Point pvaluesAs = TSA.getPValuesAsymptotic();
    assert_almost_equal(pvaluesAs, referencePValuesAs);

    /* We set the number of permutations for the pvalue estimate. */
    UnsignedInteger b = 1000 ;
    TSA.setPermutationSize(b);

    /* We get the pvalue estimate by permutations */
    Point referencePValuesPerm = {0, 0.233766, 0.265734};
    Point pvaluesPerm = TSA.getPValuesPermutation();
    assert_almost_equal(pvaluesPerm, referencePValuesPerm);

    /* Change filter function and recompute everything */
    SymbolicFunction squaredExponential("x", "exp(-0.1 * x^2)");
    ComposedFunction alternateFilter(squaredExponential, g);
    TSA.setFilterFunction(alternateFilter);
    assert_almost_equal(TSA.getR2HSICIndices(), {0.263026, 0.0041902, 0.00309598});
    assert_almost_equal(TSA.getHSICIndices(), {1.54349e-05, 2.45066e-07, 1.88477e-07}, 1e-4, 0.0);
    assert_almost_equal(TSA.getPValuesPermutation(), {0.0, 0.264735, 0.279720});
    assert_almost_equal(TSA.getPValuesAsymptotic(), {0.0,0.270278,0.288026});

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
