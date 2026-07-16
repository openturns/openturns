//                                               -*- C++ -*-
/**
 *  @brief Tests for MulticollinearityAnalysis
 *
 *  Copyright 2005-2026 Airbus-EDF-ONERA-Phimeca
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

#include "openturns/MulticollinearityAnalysis.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  // Define the model: Y = beta1 * X1 + beta2 * X2 + Noise
  const Scalar beta1 = 2.5;
  const Scalar beta2 = 0.3;
  const Scalar sigma1 = 1.6;
  const Scalar sigma2 = 0.8;
  const Scalar sigmaEps = 0.1;
  const Scalar r = 0.5;

  // Compute the theoretical values of the indices
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

  // Draw an input sample
  const UnsignedInteger sampleSize = 100000;
  const CorrelationMatrix corrMatrix(2, {1.0, r, r, 1.0});
  const Normal inputDistribution(Point({0.0, 0.0}), Point({sigma1, sigma2}), corrMatrix);
  const Sample inputSample(inputDistribution.getSample(sampleSize));

  // Build the output sample
  const LinearFunction linearFunction(Point({0.0, 0.0}), Point({0.0}), Matrix(1, 2, {beta1, beta2}));
  const Normal noiseDistribution(0.0, sigmaEps);
  const Sample noiseSample(noiseDistribution.getSample(sampleSize));
  const Sample outputSample(linearFunction(inputSample) + noiseSample);

  try
  {
    MulticollinearityAnalysis analysis(inputSample, outputSample);

    // LMG and PMVD indices
    PointWithDescription lmg_computed, pmvd_computed;
    analysis.computeLmgPmvd(lmg_computed, pmvd_computed);
    PointWithDescription lmg_estimated, pmvd_estimated;
    analysis.estimateLmgPmvdMonteCarlo(lmg_estimated, pmvd_estimated, 1000);
    fullprint << "Theoretical LMG = [" << lmg1 << ", " << lmg2 << "]" << std::endl;
    fullprint << "Computed LMG = [" << lmg_computed[0] << ", " << lmg_computed[1] << "]" << std::endl;
    assert_almost_equal(lmg_computed, Point({lmg1, lmg2}), 2e-3, 0.0);
    fullprint << "Estimated LMG = [" << lmg_estimated[0] << ", " << lmg_estimated[1] << "]" << std::endl;
    assert_almost_equal(lmg_estimated, lmg_computed, 6e-3, 0.0);
    fullprint << "Theoretical PMVD = [" << pmvd1 << ", " << pmvd2 << "]" << std::endl;
    fullprint << "Computed PMVD = [" << pmvd_computed[0] << ", " << pmvd_computed[1] << "]" << std::endl;
    assert_almost_equal(pmvd_computed, Point({pmvd1, pmvd2}), 2e-3, 0.0);
    fullprint << "Estimated PMVD = [" << pmvd_estimated[0] << ", " << pmvd_estimated[1] << "]" << std::endl;
    assert_almost_equal(pmvd_estimated, pmvd_computed, 4e-3, 0.0);

    // Johnson index
    const PointWithDescription johnson_computed(analysis.computeJohnson());
    fullprint << "Computed Johnson = [" << johnson_computed[0] << ", " << johnson_computed[1] << "]" << std::endl;
    assert_almost_equal(johnson_computed, lmg_computed, 1e-12, 0.0);  // In 2D, Johnson and LMG indices are identical
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  try
  {
    // Check that an exception is raised when outputSample is not provided
    MulticollinearityAnalysis analysis(inputSample);
    PointWithDescription lmg_computed, pmvd_computed;
    analysis.computeLmgPmvd(lmg_computed, pmvd_computed);
    std::cerr << "InvalidArgumentException should have been thrown" << std::endl;
    return ExitCode::Error;
  }
  catch (InvalidArgumentException & ex)
  {
    // Expected exception
  }

  try
  {
    // VIF metric
    MulticollinearityAnalysis analysis(inputSample);
    const PointWithDescription vif_computed(analysis.computeVIF());
    fullprint << "Theoretical VIF = [" << vif12 << ", " << vif12 << "]" << std::endl;
    fullprint << "Computed VIF = [" << vif_computed[0] << ", " << vif_computed[1] << "]" << std::endl;
    assert_almost_equal(vif_computed, Point({vif12, vif12}), 1e-2, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
