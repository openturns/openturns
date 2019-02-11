//                                               -*- C++ -*-
/**
 *  @brief VisualTest implements statistical tests
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
#include <cmath>
#include "openturns/VisualTest.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Staircase.hxx"
#include "openturns/LinearBasisFactory.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Description.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/NormalFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

VisualTest::VisualTest()
{
}

/* Draw the empirical CDF of the Sample when its dimension is 1 */
Graph VisualTest::DrawEmpiricalCDF(const Sample & sample)
{
  if (sample.getDimension() != 1)
    throw InvalidDimensionException(HERE) << "In VisualTest::DrawEmpiricalCDF: sample should be of dimension 1, here dimension=" << sample.getDimension();
  return UserDefined(sample).drawCDF();
}

/* Draw the empirical CDF of the Sample when its dimension is 1 */
Graph VisualTest::DrawEmpiricalCDF(const Sample & sample,
                                   const Scalar xMin,
                                   const Scalar xMax)
{
  if (sample.getDimension() != 1)
    throw InvalidDimensionException(HERE) << "In VisualTest::DrawEmpiricalCDF: sample should be of dimension 1, here dimension=" << sample.getDimension();
  return UserDefined(sample).drawCDF(xMin, xMax);
}


/* Draw the Histogram of the Sample when its dimension is 1 */
Graph VisualTest::DrawHistogram(const Sample & sample,
                                const UnsignedInteger binNumber)
{
  // Create an empty graph
  Graph graphHist("sample histogram", "realizations", "frequency", true, "topright");
  graphHist.add(HistogramFactory().buildAsHistogram(sample, binNumber).drawPDF());
  // Create the barplot
  OSS oss;
  oss << sample.getName() << " histogram";
  graphHist.setLegends(Description(1, oss));
  return graphHist;
}

/* Draw the Histogram of the Sample when its dimension is 1, Normal empirical rule for bin number */
Graph VisualTest::DrawHistogram(const Sample & sample)
{
  // Create an empty graph
  Graph graphHist("sample histogram", "realizations", "frequency", true, "topright");
  graphHist.add(HistogramFactory().buildAsHistogram(sample).drawPDF());
  // Create the barplot
  OSS oss;
  oss << sample.getName() << " histogram";
  graphHist.setLegends(Description(1, oss));
  return graphHist;
}

/* Draw the QQplot of the two Samples when its dimension is 1 */
Graph VisualTest::DrawQQplot(const Sample & sample1,
                             const Sample & sample2,
                             const UnsignedInteger pointNumber)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << sample2.getDimension();
  Sample data(pointNumber, 2);
  const Scalar step = 1.0 / pointNumber;
  for (UnsignedInteger i = 0; i < pointNumber; ++i)
  {
    const Scalar q = (i + 0.5) * step;
    data(i, 0) = sample1.computeQuantilePerComponent(q)[0];
    data(i, 1) = sample2.computeQuantilePerComponent(q)[0];
  }
  Cloud cloudQQplot(data, "Data");
  if (pointNumber < 100) cloudQQplot.setPointStyle("fcircle");
  else if (pointNumber < 1000) cloudQQplot.setPointStyle("bullet");
  else cloudQQplot.setPointStyle("dot");
  Graph graphQQplot("Two sample QQ-plot", sample1.getDescription()[0], sample2.getDescription()[0], true, "topleft");
  // First, the bisectrice
  Sample diagonal(2, 2);
  Point point(2);
  diagonal(0, 0) = data(0, 0);
  diagonal(0, 1) = data(0, 0);
  diagonal(1, 0) = data(pointNumber - 1, 0);
  diagonal(1, 1) = data(pointNumber - 1, 0);
  Curve bisectrice(diagonal, "Test line");
  bisectrice.setColor("red");
  bisectrice.setLineStyle("dashed");
  graphQQplot.add(bisectrice);
  // Then the QQ plot
  graphQQplot.add(cloudQQplot);
  return graphQQplot;
}

/* Draw the QQplot of one Sample and one Distribution when its dimension is 1 */
Graph VisualTest::DrawQQplot(const Sample & sample,
                             const Distribution & dist)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << sample.getDimension();
  if (dist.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << dist.getDimension();
  const UnsignedInteger size = sample.getSize();
  const Sample sortedSample(sample.sort(0));
  Sample data(size, 2);
  const Scalar step = 1.0 / size;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data(i, 0) = sortedSample(i, 0);
    data(i, 1) = dist.computeQuantile((i + 0.5) * step)[0];
  }
  Cloud cloudQQplot(data, "Data");
  if (size < 100) cloudQQplot.setPointStyle("fcircle");
  else if (size < 1000) cloudQQplot.setPointStyle("bullet");
  else cloudQQplot.setPointStyle("dot");
  Graph graphQQplot("Sample versus model QQ-plot", sample.getDescription()[0], dist.__str__(), true, "topleft");
  // First, the bisectrice
  Sample diagonal(2, 2);
  Point point(2);
  diagonal(0, 0) = data(0, 0);
  diagonal(0, 1) = data(0, 0);
  diagonal(1, 0) = data(size - 1, 0);
  diagonal(1, 1) = data(size - 1, 0);
  Curve bisectrice(diagonal, "Test line");
  bisectrice.setColor("red");
  bisectrice.setLineStyle("dashed");
  graphQQplot.add(bisectrice);
  // Then the QQ plot
  graphQQplot.add(cloudQQplot);
  return graphQQplot;
}

/* Draw the Henry line for a sample when its dimension is 1 */
Graph VisualTest::DrawHenryLine(const Sample & sample)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a Henry line only if the sample dimension equals 1, here dimension=" << sample.getDimension();
  return DrawHenryLine(sample, NormalFactory().buildAsNormal(sample));
}

/* Draw the Henry line for a sample and a given normal distribution when its dimension is 1 */
Graph VisualTest::DrawHenryLine(const Sample & sample, const Distribution & normal)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a Henry plot only if the sample dimension equals 1, here dimension=" << sample.getDimension();
  if (normal.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a Henry plot only if the normal distribution dimension equals 1, here dimension=" << normal.getDimension();
  if (normal.getImplementation()->getClassName() != "Normal") throw InvalidArgumentException(HERE) << "Normal distribution expected";

  Graph graphHenry("Henry plot", "Sample", "Standard normal quantiles", true, "topleft");
  const UnsignedInteger size = sample.getSize();
  const Sample sortedSample(sample.sort(0));

  // First, the Henry line: y = (x - mu) / sigma
  const Scalar mu = normal.getMean()[0];
  const Scalar sigma = normal.getStandardDeviation()[0];
  Sample henryLinePoints(2, 2);
  henryLinePoints(0, 0) = sortedSample(0, 0); // sample.getMin()[0];
  henryLinePoints(0, 1) = (henryLinePoints(0, 0) - mu) / sigma;
  henryLinePoints(1, 0) = sortedSample(size - 1, 0); // sample.getMax()[0];
  henryLinePoints(1, 1) = (henryLinePoints(1, 0) - mu) / sigma;
  Curve henryLine(henryLinePoints, "Henry line");
  henryLine.setColor("red");
  henryLine.setLineStyle("dashed");
  graphHenry.add(henryLine);

  // Then, the data
  const Normal standard_normal(0.0, 1.0);
  Sample data(size, 2);
  const Scalar step = 1.0 / size;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data(i, 0) = sortedSample(i, 0);
    data(i, 1) = standard_normal.computeQuantile((i + 0.5) * step)[0];
  }
  Cloud dataCloud(data, "Data");
  graphHenry.add(dataCloud);

  return graphHenry;
}


/* Draw the clouds of one Sample and one model when its dimension is 2 */
Graph VisualTest::DrawClouds(const Sample & sample,
                             const Distribution & dist)
{
  if (sample.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw sample clouds only if dimension equals 2, here dimension=" << sample.getDimension();
  if (dist.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw distribution clouds only if dimension equals 2, here dimension=" << dist.getDimension();

  const Sample distSample(dist.getSample(sample.getSize()));

  const Cloud sampleCloud(sample, "blue", "fsquare", "Sample Cloud");
  const Cloud distCloud(distSample, "red", "fsquare", "Model Cloud");

  Graph myGraph("two samples clouds", "x1", "x2", true, "topright");
  // Then, draw it
  myGraph.add(sampleCloud);
  myGraph.add(distCloud);

  return myGraph;
}

/* Draw the clouds of two Sample and one model when its dimension is 2 */
Graph VisualTest::DrawClouds(const Sample & sample1,
                             const Sample & sample2)
{
  if (sample1.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw sample clouds only if dimension equals 2, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw sample clouds only if dimension equals 2, here dimension=" << sample2.getDimension();

  const Cloud sampleCloud1(sample1, "blue", "fsquare", "Sample1 Cloud");
  const Cloud sampleCloud2(sample2, "red", "fsquare", "Sample2 Cloud");

  Graph myGraph("two samples clouds", "x1", "x2", true, "topright");
  // Then, draw it
  myGraph.add(sampleCloud1);
  myGraph.add(sampleCloud2);

  return myGraph;
}

/* Draw the visual test for the LinearModel when its dimension is 1 */
Graph VisualTest::DrawLinearModel(const Sample & sample1,
                                  const Sample & sample2,
                                  const Point & trendCoefficients)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel visual test only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel visual test only if dimension equals 1, here dimension=" << sample2.getDimension();
  if (sample1.getSize() != sample2.getSize()) throw InvalidArgumentException(HERE) << "Error: can draw a LinearModel visual test only if sample 1 and sample 2 have the same size, here sample 1 size=" << sample1.getSize() << " and sample 2 size=" << sample2.getSize();

  if (trendCoefficients.getSize() != sample1.getDimension()+1) throw InvalidArgumentException(HERE) << "Not enough trend coefficients";
  const LinearCombinationFunction fHat(LinearBasisFactory(trendCoefficients.getSize() - 1).build(), trendCoefficients);
  const Sample y(fHat(sample1));
  
  OSS oss;
  oss << sample1.getName() << " LinearModel visualTest";
  const UnsignedInteger size = sample1.getSize();
  Sample sample2D(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Point point(2);
    point[0] = sample1(i, 0);
    point[1] = y(i, 0);
    sample2D[i] = point;
  }
  Curve curveLinearModelTest(sample2D.sortAccordingToAComponent(0));
  curveLinearModelTest.setLegend(oss);
  Cloud cloudLinearModelTest(sample1, sample2);
  cloudLinearModelTest.setColor("red");
  cloudLinearModelTest.setPointStyle("fsquare");
  cloudLinearModelTest.setLegend("Original Sample");

  Graph graphLinearModelTest("original sample versus Linear Model one", "x", "y", true, "topright");
  graphLinearModelTest.add(cloudLinearModelTest);
  graphLinearModelTest.add(curveLinearModelTest);
  return graphLinearModelTest;
}

/* Draw the visual test for the LinearModel when its dimension is 1 */
Graph VisualTest::DrawLinearModel(const Sample & sample1,
                                  const Sample & sample2,
                                  const LinearModel & linearModel)
{
  LOGWARN(OSS() << "DrawLinearModel(..., LinearModel) is deprecated");
  return DrawLinearModel(sample1, sample2, linearModel.getRegression());
}

/* Draw the visual test for the LinearModel residuals */
Graph VisualTest::DrawLinearModelResidual(const Sample & sample1,
    const Sample & sample2,
    const Point & trendCoefficients)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if dimension equals 1, here dimension=" << sample2.getDimension();
  if (sample1.getSize() != sample2.getSize()) throw InvalidArgumentException(HERE) << "Error: can draw a LinearModel residual visual test only if sample 1 and sample 2 have the same size, here sample 1 size=" << sample1.getSize() << " and sample 2 size=" << sample2.getSize();

  if (trendCoefficients.getSize() != sample1.getDimension()+1) throw InvalidArgumentException(HERE) << "Not enough trend coefficients";
  const LinearCombinationFunction fHat(LinearBasisFactory(trendCoefficients.getSize() - 1).build(), trendCoefficients);
  const Sample yHat(fHat(sample1));
  const Sample y(sample2 - yHat);

  const UnsignedInteger size = sample1.getSize();
  Sample data(size - 1, 2);
  for (UnsignedInteger i = 0; i < size - 1; ++i)
  {
    data(i, 0) = y(i, 0);
    data(i, 1) = y(i + 1, 0);
  }

  OSS oss;
  oss << sample1.getName() << " LinearModel residual Test";
  const Cloud cloudLinearModelRTest(data, "red", "fsquare", oss);

  Graph graphLinearModelRTest("residual(i) versus residual(i-1)", "redidual(i-1)", "residual(i)", true, "topright");
  graphLinearModelRTest.add(cloudLinearModelRTest);
  return graphLinearModelRTest;
}

Graph VisualTest::DrawLinearModelResidual(const Sample & sample1,
    const Sample & sample2,
    const LinearModel & linearModel)
{
  LOGWARN(OSS() << "DrawLinearModelResidual(..., LinearModel) is deprecated");
  return DrawLinearModelResidual(sample1, sample2, linearModel.getRegression());
}

/* Draw the CobWeb visual test */
Graph VisualTest::DrawCobWeb(const Sample & inputSample,
                             const Sample & outputSample,
                             const Scalar minValue,
                             const Scalar maxValue,
                             const String & color,
                             const Bool quantileScale)
{
  const UnsignedInteger size = inputSample.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the input sample is empty.";
  if (size != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  if (outputSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the output sample dimension must be 1.";
  if (minValue > maxValue) throw InvalidArgumentException(HERE) << "Error: minValue cannot be greater than maxValue.";
  // If based on quantiles, rank the values
  UnsignedInteger minRank = 0;
  UnsignedInteger maxRank = 0;
  if (!quantileScale)
  {
    const Scalar minCDF = outputSample.computeEmpiricalCDF(Point(1, minValue));
    const Scalar maxCDF = outputSample.computeEmpiricalCDF(Point(1, maxValue));
    minRank = static_cast<UnsignedInteger>(round(size * minCDF));
    maxRank = static_cast<UnsignedInteger>(round(size * maxCDF));
  }
  else
  {
    if ((minValue < 0.0) || (maxValue > 1.0) || (minValue > maxValue))  throw InvalidArgumentException(HERE) << "Error: we must have 0 <= minValue <= maxValue <= 1 when using quantile scale.";
    minRank = static_cast<UnsignedInteger>(size * minValue);
    maxRank = static_cast<UnsignedInteger>(size * maxValue);
  }
  const UnsignedInteger inputDimension = inputSample.getDimension();
  const Sample rankedInput(inputSample.rank());
  const Sample rankedOutput(outputSample.rank());
  // Create the graph
  Graph cobWeb(String(OSS() << "Cobweb graph - " << outputSample.getDescription() << " vs " << inputSample.getDescription()), "", "", false, "topright");
  // First discriminate the filaments: draw the background filaments and memorize the selected ones
  Indices selectedFilaments(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger currentRank = static_cast<UnsignedInteger>(rankedOutput(i, 0));
    if ((currentRank >= minRank) && (currentRank <= maxRank))
      selectedFilaments.add(i);
    else
    {
      Sample data(inputDimension + 1, 1);
      for (UnsignedInteger j = 0; j < inputDimension; ++j) data(j, 0) = rankedInput(i, j);
      data(inputDimension, 0) = rankedOutput(i, 0);
      Curve filament(data);
      filament.setColor("grey");
      cobWeb.add(filament);
    }
  }
  // Draw the selected filaments
  const UnsignedInteger selectedSize = selectedFilaments.getSize();
  for (UnsignedInteger i = 0; i < selectedSize; ++i)
  {
    Sample data(inputDimension + 1, 1);
    for (UnsignedInteger j = 0; j < inputDimension; ++j) data(j, 0) = rankedInput(selectedFilaments[i], j);
    data(inputDimension, 0) = rankedOutput(selectedFilaments[i], 0);
    Curve filament(data);
    filament.setColor(color);
    cobWeb.add(filament);
  }
  // Draw the vertical lines associated with the input variables
  const Description palette(Curve::BuildDefaultPalette(inputDimension));
  for (UnsignedInteger i = 0; i < inputDimension + 1; ++i)
  {
    Sample data(2, 2);
    Point point(2);
    data(0, 0) = i;
    data(1, 0) = i;
    data(1, 1) = size;
    Curve bar(data);
    if (i < inputDimension)
    {
      bar.setColor(palette[i]);
      bar.setLegend(inputSample.getDescription()[i]);
    }
    else
    {
      bar.setColor("black");
      bar.setLegend(outputSample.getDescription()[0]);
    }
    bar.setLineWidth(3);
    cobWeb.add(bar);
  }
  cobWeb.setGrid(false);
  return cobWeb;
}

/* Draw the Kendall plot to assess a copula for a bidimensional sample */
Graph VisualTest::DrawKendallPlot(const Sample & data,
                                  const Distribution & copula)
{
  if (data.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the data sample is empty.";
  if (data.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the data sample has a dimension not equal to 2.";
  if (!copula.isCopula()) throw InvalidArgumentException(HERE) << "Error: the given distribution=" << copula << " is not a copula.";
  if (copula.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the copula has a dimension not equal to 2.";
  const Sample empiricalStatistics(ComputeKendallPlotEmpiricalStatistics(data));
  const Sample theoreticalStatistics(ComputeKendallPlotTheoreticalStatistics(copula, data.getSize()));
  Graph graph("Kendall Plot", copula.getName(), data.getName(), true, "topleft");
  // Draw the first diagonal
  Sample dataDiagonal(0, 2);
  dataDiagonal.add(Point(2, 0.0));
  dataDiagonal.add(Point(2, 1.0));
  Curve diagonal(dataDiagonal);
  diagonal.setColor("red");
  diagonal.setLineStyle("dashed");
  graph.add(diagonal);
  // Draw the Kendall curve
  graph.add(Curve(theoreticalStatistics, empiricalStatistics));
  return graph;
}

/* Draw the Kendall plot to assess if two bidimensional samples share the same copula */
Graph VisualTest::DrawKendallPlot(const Sample & firstSample,
                                  const Sample & secondSample)
{
  if (firstSample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the first sample is empty.";
  if (secondSample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the second sample is empty.";
  if (firstSample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the first sample has a dimension not equal to 2.";
  if (secondSample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the second sample has a dimension not equal to 2.";
  const Sample firstEmpiricalStatistics(ComputeKendallPlotEmpiricalStatistics(firstSample));
  const Sample secondEmpiricalStatistics(ComputeKendallPlotEmpiricalStatistics(secondSample));
  Graph graph("Kendall Plot", firstSample.getName(), secondSample.getName(), true, "topleft");
  // Draw the first diagonal
  Sample data(0, 2);
  data.add(Point(2, 0.0));
  data.add(Point(2, 1.0));
  Curve diagonal(data);
  diagonal.setColor("red");
  diagonal.setLineStyle("dashed");
  graph.add(diagonal);
  // Draw the Kendall curve
  graph.add(Curve(firstEmpiricalStatistics, secondEmpiricalStatistics));
  return graph;
}

/* Compute the Kendall plot empirical statistic associated with a bidimensional sample */
Sample VisualTest::ComputeKendallPlotEmpiricalStatistics(const Sample & sample)
{
  const UnsignedInteger size = sample.getSize();
  Sample result(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point pointI(sample[i]);
    const Scalar uI = pointI[0];
    const Scalar vI = pointI[1];
    UnsignedInteger cardinal = 0;
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      const Point pointJ(sample[j]);
      cardinal += (pointJ[0] <= uI) && (pointJ[1] <= vI);
    }
    for (UnsignedInteger j = i + 1; j < size; ++j)
    {
      const Point pointJ(sample[j]);
      cardinal += (pointJ[0] <= uI) && (pointJ[1] <= vI);
    }
    result[i] = Point(1, cardinal / static_cast<Scalar>(size - 1));
  }
  return result.sort(0);
}

/* Compute the Kendall plot theoretical statistic associated with a bidimensional copula */
Sample VisualTest::ComputeKendallPlotTheoreticalStatistics(const Distribution & copula,
    const UnsignedInteger size)
{
  if (!copula.isCopula()) throw InvalidArgumentException(HERE) << "Error: the given distribution=" << copula << " is not a copula.";
  const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger( "VisualTest-KendallPlot-MonteCarloSize" );
  Sample result(size, 1);
  for (UnsignedInteger i = 0; i < maximumIteration; ++i)
  {
    const Sample empiricalStatistics(ComputeKendallPlotEmpiricalStatistics(copula.getSample(size)));
    for (UnsignedInteger j = 0; j < size; ++j) result[j] = (result[j] * i + empiricalStatistics[j]) / (i + 1);
  }
  return result;
}

END_NAMESPACE_OPENTURNS
