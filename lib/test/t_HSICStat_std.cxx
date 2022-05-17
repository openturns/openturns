//                                               -*- C++ -*-
/**
 *  @brief The test file of class HSICStat, HSICUStat and HSICVStat.
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
    /* All marginals of the Ishigami model */
    Uniform X1(-M_PI, M_PI);
    Uniform X2(-M_PI, M_PI);
    Uniform X3(-M_PI, M_PI);

    ComposedDistribution::DistributionCollection aCollection;
    aCollection.add(X1);
    aCollection.add(X2);
    aCollection.add(X3);
    Distribution distX  = ComposedDistribution(aCollection);

    /* Input sample of size 100 and dimension 3. */
    UnsignedInteger size = 100;
    Sample X = distX.getSample(size);

    /* The Ishigami model */
    Description input = {"X1", "X2", "X3"};
    SymbolicFunction modelIshigami(input, Description(1, "sin(X1) + 5.0 * (sin(X2))^2 + 0.1 * X3^4 * sin(X1)"));

    /* Output */
    Sample Y = modelIshigami(X);

    /* Using the same covariance model for each marginal */
    CovarianceModel Cov1;
    Cov1 = SquaredExponential(1);

    /* Output covariance model */
    CovarianceModel Cov2;
    Cov2 = SquaredExponential(1);

    /* Set output covariance scale */
    Cov2.setScale(Y.computeStandardDeviation());

    /* This is the GSA-type estimator: weight is 1. */
    SquareMatrix W(size);
    for(UnsignedInteger i = 0; i < size; ++i)
    {
      W(i, i) = 1.0;
    }

    /* Using a biased estimator */
    HSICVStat estimatorTypeV;
    std::cout << "Is CSA compatible ? " << estimatorTypeV.isCompatibleWithConditionalAnalysis() << std::endl;

    Point referenceV(3);
    referenceV[0] = 0.0233132;
    referenceV[1] = 0.0020535;
    referenceV[2] = 0.00791711;
    /* Loop over marginals */
    for(UnsignedInteger i = 0; i < 3; ++i)
    {
      Sample test = X.getMarginal(i);
      /* Set input covariance scale */
      Cov1.setScale(test.computeStandardDeviation());
	  CovarianceMatrix CovMat1(Cov1.discretize(test)
	  CovarianceMatrix CovMat2(Cov2.discretize(Y)
      Scalar hsicIndex = estimatorTypeV.computeHSICIndex(CovMat1, CovMat2, W);
      assert_almost_equal(hsicIndex, referenceV[i]);
    }

    /* Using an unbiased estimator */
    HSICUStat estimatorTypeU;
    std::cout << "Is CSA compatible ? " << estimatorTypeU.isCompatibleWithConditionalAnalysis() << std::endl;

    Point referenceU(3);
    referenceU[0] = 0.0222838;
    referenceU[1] = 0.000256681;
    referenceU[2] = 0.00599247;
    /* Loop over marginals */
    for(UnsignedInteger i = 0; i < 3; ++i)
    {
      Sample test = X.getMarginal(i);
      /* Set input covariance scale */
      Cov1.setScale(test.computeStandardDeviation());
	  CovarianceMatrix CovMat1(Cov1.discretize(test)
	  CovarianceMatrix CovMat2(Cov2.discretize(Y)
      Scalar hsicIndex = estimatorTypeV.computeHSICIndex(CovMat1, CovMat2, W);
      assert_almost_equal(hsicIndex, referenceU[i]);
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
