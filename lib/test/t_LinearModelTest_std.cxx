//                                               -*- C++ -*-
/**
 *  @brief The test file of class Test
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
  Indices indices(dim - 1);
  indices.fill(1);
  Sample sampleX(sample.getMarginal(indices));
  Sample sampleY(sample.getMarginal(0));

  Indices selection(5);
  for (UnsignedInteger i = 0; i < 5; i++)
  {
    selection[i] = i;
  }

  Indices selection2(1, 0);

  Sample sampleX0(sampleX.getMarginal(0));

  Sample sampleZ(size, 1);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    sampleZ(i, 0) = sampleY(i, 0) * sampleY(i, 0);
  }
  fullprint << "LinearModelFisher pvalue=" << std::setprecision(2) << LinearModelTest::LinearModelFisher(sampleY, sampleZ).getPValue() << std::endl;
  fullprint << "LinearModelResidualMean pvalue=" << std::setprecision(2) << LinearModelTest::LinearModelResidualMean(sampleY, sampleZ).getPValue() << std::endl;

  // Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialRegression performs the Regression test simultaneously on all firstSample[i] and secondSample, for i in the selection. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The t-test is used.

  // The two tests must be equal
  fullprint << "PartialRegressionX0Y=" << LinearModelTest::PartialRegression(sampleX, sampleY, selection2, 0.10) << std::endl;
  fullprint << "FullRegressionX0Y=" << LinearModelTest::FullRegression(sampleX0, sampleY, 0.10) << std::endl;

  fullprint << "PartialRegressionXY=" << LinearModelTest::PartialRegression(sampleX, sampleY, selection, 0.10) << std::endl;

  // Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullRegression performs the Regression test simultaneously on all firstSample[i] and secondSample. The Regression test tests if the regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The t-test is used.

  fullprint << "FullRegressionXZ=" << LinearModelTest::FullRegression(sampleX, sampleZ, 0.10) << std::endl;
  //fullprint << "FullRegressionZZ=" << LinearModelTest::FullRegression(sampleZ, sampleZ, 0.10) << std::endl;

  return ExitCode::Success;
}
