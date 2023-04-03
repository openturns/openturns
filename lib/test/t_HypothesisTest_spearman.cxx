//                                               -*- C++ -*-
/**
 *  @brief The test file of class Test
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
  setRandomGenerator();

  UnsignedInteger size = 100;
  UnsignedInteger dim = 2;

  Sample sampleX(Normal(dim).getSample(size));
  Sample sampleX0(sampleX.getMarginal(0));
  Sample sampleY(sampleX.getMarginal(1));
  Sample sampleZ(SymbolicFunction("x", "exp(x)")(sampleX0));
  // Spearman Test : test if two samples have a zero rank correlation
  // H0 = zero rank correlation
  // Test = True <=> zero rank correlation
  // p-value threshold : probability of the critical region, ie P_H0(reject H0)
  // p-value : P_H0(test statistics > observed test statistics)
  // Test = True <=> p-value > p-value threshold

  fullprint << "Spearman=" << HypothesisTest::Spearman(sampleY, sampleZ, 0.10) << std::endl;

  // Full Spearman Test : collection of tests of zero correlation between the 1D marginals of a sample and another 1D sample
  // H0 = zero rank correlation
  // Test = True <=> zero rank correlation
  // p-value threshold : probability of the critical region, ie P_H0(reject H0)
  // p-value : P_H0(test statistics > observed test statistics)
  // Test = True <=> p-value > p-value threshold
  
  // Expected result is SpecFunc::MaxScalar
  TestResult fullSpearman(HypothesisTest::FullSpearman(sampleX, sampleZ, 0.10)[0]);
  Scalar testStatistic = fullSpearman.getStatistic();
  assert (testStatistic == SpecFunc::MaxScalar);

  fullSpearman = HypothesisTest::FullSpearman(sampleX, sampleZ, 0.10)[1];
  Scalar pValue = fullSpearman.getPValue();
  testStatistic = fullSpearman.getStatistic();
  assert_almost_equal(pValue, 0.903, 1e-4, 0.0);
  assert_almost_equal(testStatistic, 0.1219, 1e-4, 0.0);  

  // Expected result is SpecFunc::MaxScalar
  fullSpearman = HypothesisTest::FullSpearman(sampleY, sampleY, 0.10)[0];
  testStatistic = fullSpearman.getStatistic();
  assert (testStatistic == SpecFunc::MaxScalar);

  // Partial Spearman Test : collection of tests of zero correlation between a selection of the 1D marginals of a sample and another 1D sample
  // H0 = zero rank correlation
  // Test = True <=> zero rank correlation
  // p-value threshold : probability of the critical region, ie P_H0(reject H0)
  // p-value : P_H0(test statistics > observed test statistics)
  // Test = True <=> p-value > p-value threshold

  Indices selection(1, 0);
  // The three tests must be equal
  fullprint << "PartialSpearmanX0Y=" << HypothesisTest::PartialSpearman(sampleX, sampleY, selection, 0.10) << std::endl;
  fullprint << "SpearmanX0Y=" << HypothesisTest::Spearman(sampleX0, sampleY, 0.10) << std::endl;
  fullprint << "FullSpearmanX0Y=" << HypothesisTest::FullSpearman(sampleX0, sampleY, 0.10) << std::endl;

  fullprint << "PartialSpearmanXY=" << HypothesisTest::PartialSpearman(sampleX, sampleY, selection, 0.10) << std::endl;

  return ExitCode::Success;
}
