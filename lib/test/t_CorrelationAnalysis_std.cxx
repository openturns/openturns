//                                               -*- C++ -*-
/**
 *  @brief Test file fo the correlation coefficients computation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

typedef Collection<Distribution> DistributionCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    UnsignedInteger dimension = 2;
    UnsignedInteger sampleSize = 100000;

    // we create an analytical function
    Description input(dimension);
    input[0] = "x0";
    input[1] = "x1";
    Description formulas(1, "10+3*x0+x1");
    SymbolicFunction analytical(input, formulas);

    // we create a collection of Normal centered distributions
    DistributionCollection aCollection;
    for(UnsignedInteger i = 0; i < dimension; ++i)
    {
      aCollection.add(Normal());
    }

    // we create one distribution object
    JointDistribution aDistribution(aCollection, IndependentCopula(dimension));

    RandomVector randomVector(aDistribution);
    CompositeRandomVector composite(analytical, randomVector);

    // we create two input samples for the function
    Sample inputSample(randomVector.getSample(sampleSize));
    Sample outputSample(analytical(inputSample));

    // Create the CorrelationAnalysis object
    CorrelationAnalysis corr_analysis(inputSample, outputSample);

    Point squared_src = corr_analysis.computeSquaredSRC();
    assert_almost_equal(squared_src, Point({0.9, 0.1}), 0.0, 1e-2); // theoretical value

    // Squared SRC with normalize
    Point squared_src_normalize(corr_analysis.computeSquaredSRC(true));
    assert_almost_equal(squared_src_normalize, Point({0.9, 0.1}), 0.0, 1e-2); // theoretical value

    Point src(corr_analysis.computeSRC());
    assert_almost_equal(src, Point({0.9486832980505138, 0.31622776601683794}), 0.0, 1e-2); // sqrt of squared_src

    Point srrc(corr_analysis.computeSRRC());
    assert_almost_equal(srrc, Point({0.94, 0.30}), 0.0, 1e-2); // approximate value

    Point pcc(corr_analysis.computePCC());
    assert_almost_equal(pcc, Point({1.0, 1.0}), 1e-5, 0.0); // theoretical value

    Point prcc(corr_analysis.computePRCC());
    assert_almost_equal(prcc, Point({0.99, 0.92}), 0.0, 1e-2); // approximate value

    Point pearson(corr_analysis.computeLinearCorrelation());
    assert_almost_equal(pearson, Point({0.95, 0.31}), 0.0, 1e-2); // approximate value

    Point spearman(corr_analysis.computeSpearmanCorrelation());
    assert_almost_equal(spearman, Point({0.94, 0.30}), 0.0, 1e-2); // approximate value

    Point kendalltau(corr_analysis.computeKendallTau());
    assert_almost_equal(kendalltau, Point({0.79, 0.20}), 0.0, 1e-2);

    // Check collinearity indices on correlated inputs
    const Scalar beta1 = 2.5;
    const Scalar beta2 = 0.3;
    const Scalar sigma1 = 1.6;
    const Scalar sigma2 = 0.8;
    const Scalar sigmaEps = 0.1;
    const Scalar r = 0.5;
    const Scalar b1 = beta1 * sigma1;
    const Scalar b2 = beta2 * sigma2;
    const Scalar a = b1 * b1 * (1 - r * r);
    const Scalar c = b2 * b2 * (1 - r * r);
    const Scalar b = (b1 * b1 * r * r) + (2 * b1 * b2 * r) + (b2 * b2 * r * r);
    const Scalar lmg1 = (a + b / 2) / (a + b + c + sigmaEps * sigmaEps);
    const Scalar lmg2 = (c + b / 2) / (a + b + c + sigmaEps * sigmaEps);
    const Scalar pmvd1 = a * (1 + b / (a + c)) / (a + b + c + sigmaEps * sigmaEps);
    const Scalar pmvd2 = c * (1 + b / (a + c)) / (a + b + c + sigmaEps * sigmaEps);
    const Scalar vif12 = 1 / (1 - r * r);

    const UnsignedInteger correlatedSampleSize = 100000;
    RandomGenerator::SetSeed(0);
    const CorrelationMatrix corMatrix(2, {1.0, r, r, 1.0});
    const Normal inputDistribution(Point({0.0, 0.0}), Point({sigma1, sigma2}), corMatrix);
    const Sample correlatedInputSample(inputDistribution.getSample(correlatedSampleSize));
    const LinearFunction linearFunction(Point({0.0, 0.0}), Point({0.0}), Matrix(1, 2, {beta1, beta2}));
    const Normal noiseDistribution(0.0, sigmaEps);
    const Sample noiseSample(noiseDistribution.getSample(correlatedSampleSize));
    const Sample correlatedOutputSample(linearFunction(correlatedInputSample) + noiseSample);

    CorrelationAnalysis analysis(correlatedInputSample, correlatedOutputSample);

    PointWithDescription lmg_computed, pmvd_computed;
    analysis.computeLMGAndPMVD(lmg_computed, pmvd_computed);
    PointWithDescription lmg_estimated, pmvd_estimated;
    analysis.computeLMGAndPMVDMonteCarlo(lmg_estimated, pmvd_estimated, 1000);
    assert_almost_equal(lmg_computed, Point({lmg1, lmg2}), 2e-3, 0.0);
    assert_almost_equal(lmg_estimated, lmg_computed, 6e-3, 0.0);
    assert_almost_equal(pmvd_computed, Point({pmvd1, pmvd2}), 2e-3, 0.0);
    assert_almost_equal(pmvd_estimated, pmvd_computed, 4e-3, 0.0);

    const PointWithDescription johnson_computed(analysis.computeJohnson());
    assert_almost_equal(johnson_computed, lmg_computed, 1e-10, 0.0);

    const PointWithDescription vif_computed(CorrelationAnalysis::ComputeVIF(correlatedInputSample));
    assert_almost_equal(vif_computed, Point({vif12, vif12}), 7e-4, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
