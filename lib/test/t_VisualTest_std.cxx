//                                               -*- C++ -*-
/**
 *  @brief The test file of class VisualTest
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

  /* EmpiricalCDF tests */
  {
    /* Generate a Normal sample */
    Normal normal(1);
    UnsignedInteger size = 100;
    Sample sample(normal.getSample(size));
    Graph sampleCDF(VisualTest::DrawEmpiricalCDF(sample, sample.getMin()[0] - 1.0, sample.getMax()[0] + 1.0));
    fullprint << "sampleCDF = " << sampleCDF << std::endl;

    sampleCDF = VisualTest::DrawEmpiricalCDF(sample, -0.5, 0.5);
    fullprint << "EmpiricalCDF graph = " << sampleCDF << std::endl;
  }

  /* Histogram tests */
  {
    Normal normal(1);
    UnsignedInteger size = 100;
    Sample sample(normal.getSample(size));
    Graph sampleHist(VisualTest::DrawHistogram(sample, 10));
    fullprint << "sampleHist = " << sampleHist << std::endl;

    sampleHist = VisualTest::DrawHistogram(sample);
    fullprint << "Histogram graph = " << sampleHist << std::endl;
  }

  /* QQPlot tests */
  {
    UnsignedInteger size = 100;
    Normal normal(1);
    Sample sample(normal.getSample(size));
    Sample sample2(Gamma(3.0, 4.0, 0.0).getSample(size));
    Graph twoSamplesQQPlot(VisualTest::DrawQQplot(sample, sample2, 100));
    fullprint << "twoSamplesQQPlot = " << twoSamplesQQPlot << std::endl;

    Graph sampleDistributionQQPlot(VisualTest::DrawQQplot(sample, normal));
    fullprint << "sampleDistributionQQPlot = " << sampleDistributionQQPlot << std::endl;
  }

  /* HenryLine test */
  {
    UnsignedInteger size = 100;
    Normal normal(1);
    Sample sample(normal.getSample(size));
    Graph henryPlot(VisualTest::DrawHenryLine(sample));
    fullprint << "Henry graph = " << henryPlot << std::endl;
  }

  /* Clouds tests */
  {
    UnsignedInteger dimension = 2;
    CorrelationMatrix R(dimension);
    R(0, 1) = 0.8;
    Normal distribution(Point(dimension, 3.0), Point(dimension, 2.0), R);
    UnsignedInteger size = 100;
    Sample sample2D(distribution.getSample(size));
    Sample firstSample(size, 1);
    Sample secondSample(size, 1);
    for (UnsignedInteger i = 0; i < size; i++)
    {
      firstSample[i][0] = sample2D[i][0];
      secondSample[i][0] = sample2D[i][1];
    }

    Graph sampleSampleClouds(VisualTest::DrawClouds(sample2D, Normal(Point(dimension, 2.0), Point(dimension, 3.0), R).getSample(size / 2)));
    fullprint << "sampleSampleClouds = " << sampleSampleClouds << std::endl;

    Graph sampleDistributionClouds(VisualTest::DrawClouds(sample2D, Normal(Point(dimension, 2.5), Point(dimension, 1.0), R)));
    fullprint << "sampleDistributionClouds = " << sampleDistributionClouds << std::endl;
  }

  /* LinearModel tests */
  {
    UnsignedInteger dimension = 2;
    CorrelationMatrix R(dimension);
    R(0, 1) = 0.8;
    Normal distribution(Point(dimension, 3.0), Point(dimension, 2.0), R);
    UnsignedInteger size = 100;
    Sample sample2D(distribution.getSample(size));
    Sample firstSample(size, 1);
    Sample secondSample(size, 1);
    for (UnsignedInteger i = 0; i < size; i++)
    {
      firstSample[i][0] = sample2D[i][0];
      secondSample[i][0] = sample2D[i][1];
    }

    LinearModelResult lmtest(LinearModelAlgorithm(firstSample, secondSample).getResult());
    Graph drawLinearModelVTest(VisualTest::DrawLinearModel(firstSample, secondSample, lmtest));
    fullprint << "LinearModelV = " << drawLinearModelVTest << std::endl;

    Graph drawLinearModelResidualTest(VisualTest::DrawLinearModelResidual(firstSample, secondSample, lmtest));
    fullprint << "LinearModelR = " << drawLinearModelResidualTest << std::endl;
  }

  /* CobWeb tests */
  {
    UnsignedInteger size = 100;
    UnsignedInteger inputDimension = 6;
    Sample inputSample(Normal(inputDimension).getSample(size));
    Description inputVar(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++i) inputVar[i] = (OSS() << "X" << i);
    Description formula(1);
    OSS oss;
    for (UnsignedInteger i = 0; i < inputDimension; ++i) oss << (i > 0 ? "+" : "") << "cos(" << i + 1 << "*" << inputVar[i] << ")";
    formula[0] = oss;
    SymbolicFunction model(inputVar, formula);
    Sample outputSample(model(inputSample));
    Graph cobwebValue(VisualTest::DrawCobWeb(inputSample, outputSample, 2.5, 3.0, "red", false));
    fullprint << "cobwebValue = " << cobwebValue << std::endl;
    Graph cobwebQuantile(VisualTest::DrawCobWeb(inputSample, outputSample, 0.7, 0.9, "red", false));
    fullprint << "cobwebQuantile = " << cobwebQuantile << std::endl;
  }

  /* KendallPlot tests */
  {
    UnsignedInteger size = 100;
    FrankCopula copula1(1.5);
    GumbelCopula copula2(4.5);
    Sample sample1(copula1.getSample(size));
    sample1.setName("data 1");
    Sample sample2(copula2.getSample(size));
    sample2.setName("data 2");
    Graph kendallPlot1(VisualTest::DrawKendallPlot(sample1, copula2));
    fullprint << "KendallPlot1 = " << kendallPlot1 << std::endl;
    Graph kendallPlot2(VisualTest::DrawKendallPlot(sample2, sample1));
    fullprint << "KendallPlot2 = " << kendallPlot2 << std::endl;
  }

  return ExitCode::Success;
}
