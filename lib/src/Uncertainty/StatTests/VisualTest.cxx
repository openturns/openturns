//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include <cmath>
#include "openturns/VisualTest.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/BarPlot.hxx"
#include "openturns/Staircase.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Description.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Normal.hxx"
#include "openturns/NormalFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

VisualTest::VisualTest()
{
}

/* Draw the empirical CDF of the Sample when its dimension is 1 */
Graph VisualTest::DrawEmpiricalCDF(const NumericalSample & sample,
                                   const NumericalScalar xMin,
                                   const NumericalScalar xMax)
{
  return UserDefined(sample).drawCDF(xMin, xMax);
}


/* Draw the Histogram of the Sample when its dimension is 1 */
Graph VisualTest::DrawHistogram(const NumericalSample & sample,
                                const UnsignedInteger binNumber)
{
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw an Histogram based on an empty sample.";
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw an Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  if (binNumber == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw an Histogram with 0 bar";

  // Construct the histogram
  // It will extends from min to max, with BarNumber bars.
  NumericalScalar min(sample.getMin()[0]);
  // Small adjustment in order to have classes defined as [x_k, x_k+1[ intervalles
  NumericalScalar max(sample.getMax()[0]);
  NumericalSample data(binNumber, 2);
  // If xmax == xmin, it is either because there is only one value in the sample, or because all the values are equal. It is the same problem in both cases: what is a meaningfull bin width?
  if (max == min)
  {
    LOGWARN("You are drawing an Histogram for a sample with constant realizations, which is questionable. The number of bins is set to 1.");
    data = NumericalSample(1, 2);
    data[0][0] = 1.0 / size;
    data[0][1] = size;
    min -= 0.5 / size;
  }
  // Here, we know that size > 1
  else
  {
    NumericalScalar h;
    // Special case: only one bin, we choose a bin width equal to the data range
    if (binNumber == 1)
    {
      h = max - min;
      data[0][0] = h;
      data[0][1] = 1.0 / h;
    }
    // More than one bin: we cover the data range with
    else
    {
      h = (max - min) / (binNumber - 1);
      min -= 0.5 * h;
      for (UnsignedInteger i = 0; i < binNumber; ++i) data[i][0] = h;
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        const UnsignedInteger index(static_cast< UnsignedInteger >((sample[i][0] - min) / h));
        ++data[index][1];
      }
    }
    const NumericalScalar inverseArea(1.0 / (h * size));
    for (UnsignedInteger i = 0; i < binNumber; ++i) data[i][1] *= inverseArea;
  }
  // Create an empty graph
  Graph graphHist("sample histogram", "realizations", "frequency", true, "topright");

  // Create the barplot
  OSS oss;
  oss << sample.getName() << " histogram";
  const BarPlot histHist(data, min, "purple", "shaded", "solid", oss);
  // Then, draw it
  graphHist.add(histHist);
  return graphHist;
}

/* Draw the Histogram of the Sample when its dimension is 1, Normal empirical rule for bin number */
Graph VisualTest::DrawHistogram(const NumericalSample & sample)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a Histogram only if dimension equals 1, here dimension=" << sample.getDimension();
  const NumericalScalar std(sample.computeStandardDeviationPerComponent()[0]);
  UnsignedInteger nBars(1);
  if (std == 0.0)
  {
    LOGWARN("You are drawing an Histogram for a sample with constant realizations, which is questionable.");
  }
  else
  {
    const NumericalScalar hOpt(std * pow(24.0 * sqrt(M_PI) / sample.getSize(), 1.0 / 3.0));
    nBars = (UnsignedInteger)(ceil((sample.getMax()[0] - sample.getMin()[0]) / hOpt + 0.5));
  }
  return DrawHistogram(sample, nBars);
}

/* Draw the QQplot of the two Samples when its dimension is 1 */
Graph VisualTest::DrawQQplot(const NumericalSample & sample1,
                             const NumericalSample & sample2,
                             const UnsignedInteger pointNumber)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << sample2.getDimension();
  NumericalSample data(pointNumber, 2);
  const NumericalScalar step(1.0 / pointNumber);
  for (UnsignedInteger i = 0; i < pointNumber; ++i)
  {
    const NumericalScalar q((i + 0.5) * step);
    data[i][0] = sample1.computeQuantilePerComponent(q)[0];
    data[i][1] = sample2.computeQuantilePerComponent(q)[0];
  }
  Cloud cloudQQplot(data, "Data");
  if (pointNumber < 100) cloudQQplot.setPointStyle("fcircle");
  else if (pointNumber < 1000) cloudQQplot.setPointStyle("bullet");
  else cloudQQplot.setPointStyle("dot");
  Graph graphQQplot("Two sample QQ-plot", sample1.getDescription()[0], sample2.getDescription()[0], true, "topleft");
  // First, the bisectrice
  NumericalSample diagonal(2, 2);
  NumericalPoint point(2);
  diagonal[0][0] = data[0][0];
  diagonal[0][1] = data[0][0];
  diagonal[1][0] = data[pointNumber - 1][0];
  diagonal[1][1] = data[pointNumber - 1][0];
  Curve bisectrice(diagonal, "Test line");
  bisectrice.setColor("red");
  bisectrice.setLineStyle("dashed");
  graphQQplot.add(bisectrice);
  // Then the QQ plot
  graphQQplot.add(cloudQQplot);
  // Adapt the margins
  NumericalPoint boundingBox(graphQQplot.getBoundingBox());
  NumericalScalar width(boundingBox[1] - boundingBox[0]);
  NumericalScalar height(boundingBox[3] - boundingBox[2]);
  boundingBox[0] -= 0.1 * width;
  boundingBox[1] += 0.1 * width;
  boundingBox[2] -= 0.1 * height;
  boundingBox[3] += 0.1 * height;
  graphQQplot.setBoundingBox(boundingBox);
  return graphQQplot;
}

/* Draw the QQplot of one Sample and one Distribution when its dimension is 1 */
Graph VisualTest::DrawQQplot(const NumericalSample & sample,
                             const Distribution & dist)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << sample.getDimension();
  if (dist.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a QQplot only if dimension equals 1, here dimension=" << dist.getDimension();
  const UnsignedInteger size(sample.getSize());
  const NumericalSample sortedSample(sample.sort(0));
  NumericalSample data(size, 2);
  const NumericalScalar step(1.0 / size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data[i][0] = sortedSample[i][0];
    data[i][1] = dist.computeQuantile((i + 0.5) * step)[0];
  }
  Cloud cloudQQplot(data, "Data");
  if (size < 100) cloudQQplot.setPointStyle("fcircle");
  else if (size < 1000) cloudQQplot.setPointStyle("bullet");
  else cloudQQplot.setPointStyle("dot");
  Graph graphQQplot("Sample versus model QQ-plot", sample.getDescription()[0], dist.__str__(), true, "topleft");
  // First, the bisectrice
  NumericalSample diagonal(2, 2);
  NumericalPoint point(2);
  diagonal[0][0] = data[0][0];
  diagonal[0][1] = data[0][0];
  diagonal[1][0] = data[size - 1][0];
  diagonal[1][1] = data[size - 1][0];
  Curve bisectrice(diagonal, "Test line");
  bisectrice.setColor("red");
  bisectrice.setLineStyle("dashed");
  graphQQplot.add(bisectrice);
  // Then the QQ plot
  graphQQplot.add(cloudQQplot);
  // Adapt the margins
  NumericalPoint boundingBox(graphQQplot.getBoundingBox());
  NumericalScalar width(boundingBox[1] - boundingBox[0]);
  NumericalScalar height(boundingBox[3] - boundingBox[2]);
  boundingBox[0] -= 0.1 * width;
  boundingBox[1] += 0.1 * width;
  boundingBox[2] -= 0.1 * height;
  boundingBox[3] += 0.1 * height;
  graphQQplot.setBoundingBox(boundingBox);
  return graphQQplot;
}

/* Draw the Henry line for a sample when its dimension is 1 */
Graph VisualTest::DrawHenryLine(const NumericalSample & sample)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a Henry line only if the sample dimension equals 1, here dimension=" << sample.getDimension();
  return DrawHenryLine(sample, NormalFactory().buildAsNormal(sample));
}

/* Draw the Henry line for a sample and a given normal distribution when its dimension is 1 */
Graph VisualTest::DrawHenryLine(const NumericalSample & sample, const Distribution & normal)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a Henry plot only if the sample dimension equals 1, here dimension=" << sample.getDimension();
  if (normal.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a Henry plot only if the normal distribution dimension equals 1, here dimension=" << normal.getDimension();
  if (normal.getImplementation()->getClassName() != "Normal") throw InvalidArgumentException(HERE) << "Normal distribution expected";

  Graph graphHenry("Henry plot", "Sample", "Standard normal quantiles", true, "topleft");
  const UnsignedInteger size(sample.getSize());
  const NumericalSample sortedSample(sample.sort(0));

  // First, the Henry line: y = (x - mu) / sigma
  const NumericalScalar mu = normal.getMean()[0];
  const NumericalScalar sigma = normal.getStandardDeviation()[0];
  NumericalSample henryLinePoints(2, 2);
  henryLinePoints[0][0] = sortedSample[0][0]; // sample.getMin()[0];
  henryLinePoints[0][1] = (henryLinePoints[0][0] - mu) / sigma;
  henryLinePoints[1][0] = sortedSample[size - 1][0]; // sample.getMax()[0];
  henryLinePoints[1][1] = (henryLinePoints[1][0] - mu) / sigma;
  Curve henryLine(henryLinePoints, "Henry line");
  henryLine.setColor("red");
  henryLine.setLineStyle("dashed");
  graphHenry.add(henryLine);

  // Then, the data
  const Normal standard_normal(0.0, 1.0);
  NumericalSample data(size, 2);
  const NumericalScalar step(1.0 / size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data[i][0] = sortedSample[i][0];
    data[i][1] = standard_normal.computeQuantile((i + 0.5) * step)[0];
  }
  Cloud dataCloud(data, "Data");
  graphHenry.add(dataCloud);

  return graphHenry;
}


/* Draw the clouds of one Sample and one model when its dimension is 2 */
Graph VisualTest::DrawClouds(const NumericalSample & sample,
                             const Distribution & dist)
{
  if (sample.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw sample clouds only if dimension equals 2, here dimension=" << sample.getDimension();
  if (dist.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw distribution clouds only if dimension equals 2, here dimension=" << dist.getDimension();

  const NumericalSample distSample(dist.getSample(sample.getSize()));

  const Cloud sampleCloud(sample, "blue", "fsquare", "Sample Cloud");
  const Cloud distCloud(distSample, "red", "fsquare", "Model Cloud");

  OSS oss;
  oss << sample.getName() << " 2D Clouds";

  Graph myGraph("two samples clouds", "x1", "x2", true, "topright");
  // Then, draw it
  myGraph.add(sampleCloud);
  myGraph.add(distCloud);

  return myGraph;
}

/* Draw the clouds of two Sample and one model when its dimension is 2 */
Graph VisualTest::DrawClouds(const NumericalSample & sample1,
                             const NumericalSample & sample2)
{
  if (sample1.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw sample clouds only if dimension equals 2, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 2) throw InvalidDimensionException(HERE) << "Error: can draw sample clouds only if dimension equals 2, here dimension=" << sample2.getDimension();

  const Cloud sampleCloud1(sample1, "blue", "fsquare", "Sample1 Cloud");
  const Cloud sampleCloud2(sample2, "red", "fsquare", "Sample2 Cloud");

  OSS oss;
  oss << sample1.getName() << " 2D Clouds";

  Graph myGraph("two samples clouds", "x1", "x2", true, "topright");
  // Then, draw it
  myGraph.add(sampleCloud1);
  myGraph.add(sampleCloud2);

  return myGraph;
}

/* Draw the visual test for the LinearModel when its dimension is 1 */
Graph VisualTest::DrawLinearModel(const NumericalSample & sample1,
                                  const NumericalSample & sample2,
                                  const LinearModel & linearModel)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel visual test only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel visual test only if dimension equals 1, here dimension=" << sample2.getDimension();
  if (sample1.getSize() != sample2.getSize()) throw InvalidArgumentException(HERE) << "Error: can draw a LinearModel visual test only if sample 1 and sample 2 have the same size, here sample 1 size=" << sample1.getSize() << " and sample 2 size=" << sample2.getSize();

  const NumericalSample y(linearModel.getPredicted(sample1));
  OSS oss;
  oss << sample1.getName() << " LinearModel visualTest";
  const UnsignedInteger size(sample1.getSize());
  NumericalSample sample2D(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    NumericalPoint point(2);
    point[0] = sample1[i][0];
    point[1] = y[i][0];
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

/* Draw the visual test for the LinearModel residuals */
Graph VisualTest::DrawLinearModelResidual(const NumericalSample & sample1,
    const NumericalSample & sample2,
    const LinearModel & linearModel)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if dimension equals 1, here dimension=" << sample2.getDimension();
  if (sample1.getSize() != sample2.getSize()) throw InvalidArgumentException(HERE) << "Error: can draw a LinearModel residual visual test only if sample 1 and sample 2 have the same size, here sample 1 size=" << sample1.getSize() << " and sample 2 size=" << sample2.getSize();

  const NumericalSample y(linearModel.getResidual(sample1, sample2));
  const UnsignedInteger size(sample1.getSize());
  NumericalSample data(size - 1, 2);
  for (UnsignedInteger i = 0; i < size - 1; ++i)
  {
    data[i][0] = y[i][0];
    data[i][1] = y[i + 1][0];
  }

  OSS oss;
  oss << sample1.getName() << " LinearModel residual Test";
  const Cloud cloudLinearModelRTest(data, "red", "fsquare", oss);

  Graph graphLinearModelRTest("residual(i) versus residual(i-1)", "redidual(i-1)", "residual(i)", true, "topright");
  graphLinearModelRTest.add(cloudLinearModelRTest);
  return graphLinearModelRTest;
}

/* Draw the CobWeb visual test */
Graph VisualTest::DrawCobWeb(const NumericalSample & inputSample,
                             const NumericalSample & outputSample,
                             const NumericalScalar minValue,
                             const NumericalScalar maxValue,
                             const String & color,
                             const Bool quantileScale)
{
  const UnsignedInteger size(inputSample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the input sample is empty.";
  if (size != outputSample.getSize()) throw InvalidArgumentException(HERE) << "Error: the input sample and the output sample must have the same size.";
  if (outputSample.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the output sample dimension must be 1.";
  if (minValue > maxValue) throw InvalidArgumentException(HERE) << "Error: minValue cannot be greater than maxValue.";
  // If based on quantiles, rank the values
  UnsignedInteger minRank;
  UnsignedInteger maxRank;
  if (!quantileScale)
  {
    const NumericalScalar minCDF(outputSample.computeEmpiricalCDF(NumericalPoint(1, minValue)));
    const NumericalScalar maxCDF(outputSample.computeEmpiricalCDF(NumericalPoint(1, maxValue)));
    minRank = static_cast<UnsignedInteger>(round(size * minCDF));
    maxRank = static_cast<UnsignedInteger>(round(size * maxCDF));
  }
  else
  {
    if ((minValue < 0.0) || (maxValue > 1.0) || (minValue > maxValue))  throw InvalidArgumentException(HERE) << "Error: we must have 0 <= minValue <= maxValue <= 1 when using quantile scale.";
    minRank = static_cast<UnsignedInteger>(size * minValue);
    maxRank = static_cast<UnsignedInteger>(size * maxValue);
  }
  const UnsignedInteger inputDimension(inputSample.getDimension());
  const NumericalSample rankedInput(inputSample.rank());
  const NumericalSample rankedOutput(outputSample.rank());
  // Create the graph
  Graph cobWeb(String(OSS() << "Cobweb graph - " << outputSample.getDescription() << " vs " << inputSample.getDescription()), "", "", false, "topright");
  // First discriminate the filaments: draw the background filaments and memorize the selected ones
  Indices selectedFilaments(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger currentRank(static_cast<UnsignedInteger>(rankedOutput[i][0]));
    if ((currentRank >= minRank) && (currentRank <= maxRank))
      selectedFilaments.add(i);
    else
    {
      NumericalSample data(inputDimension + 1, 1);
      for (UnsignedInteger j = 0; j < inputDimension; ++j) data[j][0] = rankedInput[i][j];
      data[inputDimension][0] = rankedOutput[i][0];
      Curve filament(data);
      filament.setColor("grey");
      cobWeb.add(filament);
    }
  }
  // Draw the selected filaments
  const UnsignedInteger selectedSize(selectedFilaments.getSize());
  for (UnsignedInteger i = 0; i < selectedSize; ++i)
  {
    NumericalSample data(inputDimension + 1, 1);
    for (UnsignedInteger j = 0; j < inputDimension; ++j) data[j][0] = rankedInput[selectedFilaments[i]][j];
    data[inputDimension][0] = rankedOutput[selectedFilaments[i]][0];
    Curve filament(data);
    filament.setColor(color);
    cobWeb.add(filament);
  }
  // Draw the vertical lines associated with the input variables
  const Description palette(Curve::BuildDefaultPalette(inputDimension));
  for (UnsignedInteger i = 0; i < inputDimension + 1; ++i)
  {
    NumericalSample data(2, 2);
    NumericalPoint point(2);
    data[0][0] = i;
    data[1][0] = i;
    data[1][1] = size;
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
  NumericalPoint minPoint(2);
  minPoint[0] = 0.0;
  minPoint[1] = 0.0;
  NumericalPoint maxPoint(2);
  maxPoint[0] = 1.1 * inputDimension;
  maxPoint[1] = size;
  cobWeb.setBoundingBox(Interval(minPoint, maxPoint));
  cobWeb.setGrid(false);
  return cobWeb;
}

/* Draw the Kendall plot to assess a copula for a bidimensional sample */
Graph VisualTest::DrawKendallPlot(const NumericalSample & data,
                                  const Distribution & copula)
{
  if (data.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the data sample is empty.";
  if (data.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the data sample has a dimension not equal to 2.";
  if (!copula.isCopula()) throw InvalidArgumentException(HERE) << "Error: the given distribution=" << copula << " is not a copula.";
  if (copula.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the copula has a dimension not equal to 2.";
  const NumericalSample empiricalStatistics(ComputeKendallPlotEmpiricalStatistics(data));
  const NumericalSample theoreticalStatistics(ComputeKendallPlotTheoreticalStatistics(copula, data.getSize()));
  Graph graph("Kendall Plot", copula.getName(), data.getName(), true, "topleft");
  // Draw the first diagonal
  NumericalSample dataDiagonal(0, 2);
  dataDiagonal.add(NumericalPoint(2, 0.0));
  dataDiagonal.add(NumericalPoint(2, 1.0));
  Curve diagonal(dataDiagonal);
  diagonal.setColor("red");
  diagonal.setLineStyle("dashed");
  graph.add(diagonal);
  // Draw the Kendall curve
  graph.add(Curve(theoreticalStatistics, empiricalStatistics));
  return graph;
}

/* Draw the Kendall plot to assess if two bidimensional samples share the same copula */
Graph VisualTest::DrawKendallPlot(const NumericalSample & firstSample,
                                  const NumericalSample & secondSample)
{
  if (firstSample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the first sample is empty.";
  if (secondSample.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the second sample is empty.";
  if (firstSample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the first sample has a dimension not equal to 2.";
  if (secondSample.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: cannot build a Kendall plot if the second sample has a dimension not equal to 2.";
  const NumericalSample firstEmpiricalStatistics(ComputeKendallPlotEmpiricalStatistics(firstSample));
  const NumericalSample secondEmpiricalStatistics(ComputeKendallPlotEmpiricalStatistics(secondSample));
  Graph graph("Kendall Plot", firstSample.getName(), secondSample.getName(), true, "topleft");
  // Draw the first diagonal
  NumericalSample data(0, 2);
  data.add(NumericalPoint(2, 0.0));
  data.add(NumericalPoint(2, 1.0));
  Curve diagonal(data);
  diagonal.setColor("red");
  diagonal.setLineStyle("dashed");
  graph.add(diagonal);
  // Draw the Kendall curve
  graph.add(Curve(firstEmpiricalStatistics, secondEmpiricalStatistics));
  return graph;
}

/* Compute the Kendall plot empirical statistic associated with a bidimensional sample */
NumericalSample VisualTest::ComputeKendallPlotEmpiricalStatistics(const NumericalSample & sample)
{
  const UnsignedInteger size(sample.getSize());
  NumericalSample result(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint pointI(sample[i]);
    const NumericalScalar uI(pointI[0]);
    const NumericalScalar vI(pointI[1]);
    UnsignedInteger cardinal(0);
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      const NumericalPoint pointJ(sample[j]);
      cardinal += (pointJ[0] <= uI) && (pointJ[1] <= vI);
    }
    for (UnsignedInteger j = i + 1; j < size; ++j)
    {
      const NumericalPoint pointJ(sample[j]);
      cardinal += (pointJ[0] <= uI) && (pointJ[1] <= vI);
    }
    result[i] = NumericalPoint(1, cardinal / static_cast<NumericalScalar>(size - 1));
  }
  return result.sort();
}

/* Compute the Kendall plot theoretical statistic associated with a bidimensional copula */
NumericalSample VisualTest::ComputeKendallPlotTheoreticalStatistics(const Distribution & copula,
    const UnsignedInteger size)
{
  if (!copula.isCopula()) throw InvalidArgumentException(HERE) << "Error: the given distribution=" << copula << " is not a copula.";
  const UnsignedInteger maximumIteration(ResourceMap::GetAsUnsignedInteger( "VisualTest-KendallPlot-MonteCarloSize" ));
  NumericalSample result(size, 1);
  for (UnsignedInteger i = 0; i < maximumIteration; ++i)
  {
    const NumericalSample empiricalStatistics(ComputeKendallPlotEmpiricalStatistics(copula.getSample(size)));
    for (UnsignedInteger j = 0; j < size; ++j) result[j] = (result[j] * i + empiricalStatistics[j]) / (i + 1);
  }
  return result;
}

END_NAMESPACE_OPENTURNS
