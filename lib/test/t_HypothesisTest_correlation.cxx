//                                               -*- C++ -*-
/**
 *  @brief The test file of class Test
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

int main(int , char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  UnsignedInteger size = 100;
  UnsignedInteger dim = 10;
  CorrelationMatrix R(dim);
  for (UnsignedInteger i = 0; i < dim; i++)
  {
    for (UnsignedInteger j = 0; j < i; j++)
    {
      R(i, j) = (i + j + 1.0) / (2.0 * dim);
    }
  }
  Point mean(dim, 2.0);
  Point sigma(dim, 3.0);
  Normal distribution(mean, sigma, R);
  Sample sample(distribution.getSample(size));
  Sample sampleX(size, dim - 1);
  Sample sampleY(size, 1);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    sampleY[i][0] = sample[i][0];
    for (UnsignedInteger j = 1; j < dim; j++)
    {
      sampleX[i][j - 1] = sample[i][j];
    }
  }

  Indices selection(5);
  for (UnsignedInteger i = 0; i < 5; i++)
  {
    selection[i] = i;
  }

  Indices selection2(1, 0);

  Sample sampleX0(size, 1);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    sampleX0[i][0] = sampleX[i][0];
  }

  Sample sampleZ(size, 1);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    sampleZ[i][0] = sampleY[i][0] * sampleY[i][0];
  }

  // PartialPearson : Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialPearson performs the Independence Pearson test simultaneously on firstSample[i] and secondSample, for i in the selection. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector.
  // Probability of the H0 reject zone : 1-0.90

  fullprint << "PartialPearsonXY=" << HypothesisTest::PartialPearson(sampleX, sampleY, selection, 0.90) << std::endl;

  // The three tests must be equal
  fullprint << "PartialPearsonX0Y=" << HypothesisTest::PartialPearson(sampleX, sampleY, selection2, 0.90) << std::endl;
  fullprint << "PearsonX0Y=" << HypothesisTest::Pearson(sampleX0, sampleY, 0.90) << std::endl;
  fullprint << "FullPearsonX0Y=" << HypothesisTest::FullPearson(sampleX0, sampleY, 0.90) << std::endl;

  // FullPearson : Independence Pearson test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullPearson performs the Independence Pearson test simultaneously on firstSample[i] and secondSample. For all i, it is supposed that the couple (firstSample[i] and secondSample) is issued from a gaussian  vector.
  // Probability of the H0 reject zone : 1-0.90

  fullprint << "FullPearsonX0Y=" << HypothesisTest::FullPearson(sampleX, sampleY, 0.90) << std::endl;
  fullprint << "FullPearsonYY=" << HypothesisTest::FullPearson(sampleY, sampleY, 0.90) << std::endl;

  // Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialRegression performs the Regression test simultaneously on all firstSample[i] and secondSample, for i in the selection. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used.

  // The two tests must be equal
  fullprint << "PartialRegressionX0Y=" << HypothesisTest::PartialRegression(sampleX, sampleY, selection2, 0.90) << std::endl;
  fullprint << "FullRegressionX0Y=" << HypothesisTest::FullRegression(sampleX0, sampleY, 0.90) << std::endl;

  fullprint << "PartialRegressionXY=" << HypothesisTest::PartialRegression(sampleX, sampleY, selection, 0.90) << std::endl;

  // Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullRegression performs the Regression test simultaneously on all firstSample[i] and secondSample. The Regression test tests if the regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The Fisher distribution is used.

  fullprint << "FullRegressionXZ=" << HypothesisTest::FullRegression(sampleX, sampleZ, 0.90) << std::endl;
  fullprint << "FullRegressionZZ=" << HypothesisTest::FullRegression(sampleZ, sampleZ, 0.90) << std::endl;


  return ExitCode::Success;
}
