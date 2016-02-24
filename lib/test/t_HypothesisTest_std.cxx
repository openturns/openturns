//                                               -*- C++ -*-
/**
 *  @brief The test file of class Test
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  UnsignedInteger size(100);
  UnsignedInteger dim(10);
  CorrelationMatrix R(dim);
  for (UnsignedInteger i = 0; i < dim; i++)
  {
    for (UnsignedInteger j = 0; j < i; j++)
    {
      R(i, j) = (i + j + 1.0) / (2.0 * dim);
    }
  }
  NumericalPoint mean(dim, 2.0);
  NumericalPoint sigma(dim, 3.0);
  Normal distribution(mean, sigma, R);
  NumericalSample sample(distribution.getSample(size));
  NumericalSample sampleX(size, dim - 1);
  NumericalSample sampleY(size, 1);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    sampleY[i][0] = sample[i][0];
    for (UnsignedInteger j = 1; j < dim; j++)
    {
      sampleX[i][j - 1] = sample[i][j];
    }
  }
  NumericalSample sampleZ(size, 1);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    sampleZ[i][0] = sampleY[i][0] * sampleY[i][0];
  }

  NumericalSample discreteSample1(Poisson(0.1).getSample(size));
  NumericalSample discreteSample2(Geometric(0.4).getSample(size));

  // ChiSquared Independance test : test if two samples (of sizes not necessarily equal) are independant ?
  // Care : discrete samples only
  // H0 = independent samples
  // p-value threshold : probability of the H0 reject zone : 1-0.90
  // p-value : probability (test variable decision > test variable decision evaluated on the samples)
  // Test = True <=> p-value > p-value threshold
  fullprint << "ChiSquared=" << HypothesisTest::ChiSquared(discreteSample1, discreteSample2, 0.90) << std::endl;
  fullprint << "ChiSquared2=" << HypothesisTest::ChiSquared(discreteSample1, discreteSample1, 0.90) << std::endl;


  // Pearson Test : test if two gaussian samples are independent (based on the evaluation of the linear correlation coefficient)
  // H0 : independent samples (linear correlation coefficient = 0)
  // Test = True <=> independent samples (linear correlation coefficient = 0)
  // p-value threshold : probability of the H0 reject zone : 1-0.90
  // p-value : probability (test variable decision > test variable decision evaluated on the samples)
  // Test = True <=> p-value > p-value threshold

  fullprint << "Pearson=" << HypothesisTest::Pearson(sampleY, sampleZ, 0.90) << std::endl;


  // Smirnov Test : test if two samples (of sizes not necessarily equal) follow the same distribution
  // Care : continuous distributions only
  // H0 = same continuous distribution
  // Test = True <=> same distribution
  // p-value threshold : probability of the H0 reject zone : 1-0.90
  // p-value : probability (test variable decision > test variable decision evaluated on the samples)
  // Test = True <=> p-value > p-value threshold

  fullprint << "Smirnov=" << HypothesisTest::Smirnov(sampleY, sampleZ, 0.90) << std::endl;

  // Spearman Test : test if two samples have a monotonous relation
  // H0 = no monotonous relation between both samples
  // Test = True <=> no monotonous relation
  // p-value threshold : probability of the H0 reject zone : 1-0.90
  // p-value : probability (test variable decision > test variable decision evaluated on the samples)
  // Test = True <=> p-value > p-value threshold

  fullprint << "Spearman=" << HypothesisTest::Spearman(sampleY, sampleZ, 0.90) << std::endl;
  return ExitCode::Success;
}
