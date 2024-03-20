//                                               -*- C++ -*-
/**
 *  @brief The test file of class HSICEstimatorGlobalSensitivity
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

    JointDistribution::DistributionCollection aCollection;
    aCollection.add(X1);
    aCollection.add(X2);
    aCollection.add(X3);

    /* 3d distribution made with independent marginals */
    Distribution distX  = JointDistribution(aCollection);

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

    /* We eventually build the HSIC object! */
    HSICEstimatorGlobalSensitivity hsic(covarianceModelCollection, X, Y, estimatorType);

    /* We get the HSIC indices */
    Point referenceHSICIndices = {0.0222838, 0.000256681, 0.00599247};
    Point HSICIndices = hsic.getHSICIndices();
    assert_almost_equal(HSICIndices, referenceHSICIndices);

    /* and the R2-HSIC */
    Point referenceR2HSIC = {0.298073, 0.00344498, 0.0772657};
    Point R2HSIC = hsic.getR2HSICIndices();
    assert_almost_equal(R2HSIC, referenceR2HSIC);

    /* We set the bootstrap size for the pvalue estimate. */
    UnsignedInteger b = 1000 ;
    hsic.setPermutationSize(b);

    /* We get the pvalue estimate by permutations */
    Point referencePValuesPerm = {0, 0.296703, 0.00199800};
    Point pvaluesPerm = hsic.getPValuesPermutation();
    assert_almost_equal(pvaluesPerm, referencePValuesPerm);

    Point referencePValuesAs = {1.76546e-14, 0.33272, 0.0016562};
    Point pvaluesAs = hsic.getPValuesAsymptotic();
    assert_almost_equal(pvaluesAs, referencePValuesAs);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
