//                                               -*- C++ -*-
/**
 *  @brief VisualTest implements statistical tests
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ResourceMap.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/NormalFactory.hxx"
#include "openturns/HistogramFactory.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

static String VisualTestGetPointStyle(const UnsignedInteger size)
{
  const UnsignedInteger smallSize = ResourceMap::GetAsUnsignedInteger("VisualTest-CloudSmallSize");
  const UnsignedInteger mediumSize = ResourceMap::GetAsUnsignedInteger("VisualTest-CloudMediumSize");
  if (size < smallSize)
    return "fcircle";
  else if (size < mediumSize)
    return"bullet";
  else
    return "dot";
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
  cloudQQplot.setPointStyle(VisualTestGetPointStyle(pointNumber));
  Graph graphQQplot("Two sample QQ-plot", sample1.getDescription()[0], sample2.getDescription()[0], true, "topleft");
  // First, the bisector
  Sample diagonal(2, 2);
  diagonal(0, 0) = data(0, 0);
  diagonal(0, 1) = data(0, 0);
  diagonal(1, 0) = data(pointNumber - 1, 0);
  diagonal(1, 1) = data(pointNumber - 1, 0);
  Curve bisector(diagonal, "Test line");
  bisector.setColor("red");
  bisector.setLineStyle("dashed");
  graphQQplot.add(bisector);
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
  const Sample sortedSample(sample.sortUnique());
  if (!(sortedSample.getSize() >= 2)) throw InvalidArgumentException(HERE) << "Sample must have at least 2 distinct points";
  const UnsignedInteger size = sortedSample.getSize() - 1;// avoids last point with p=1
  Sample data(size, 2);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    data(i, 0) = sortedSample(i, 0);
    const Scalar p = sample.computeEmpiricalCDF(sortedSample[i]);
    data(i, 1) = dist.computeQuantile(p)[0];
  }
  Cloud cloudQQplot(data, "Data");
  cloudQQplot.setPointStyle(VisualTestGetPointStyle(size));
  Graph graphQQplot("Sample versus model QQ-plot", sample.getDescription()[0], dist.__str__(), true, "topleft");
  // First, the bisector
  Sample diagonal(2, 2);
  diagonal(0, 0) = data(0, 0);
  diagonal(0, 1) = data(0, 0);
  diagonal(1, 0) = data(size - 1, 0);
  diagonal(1, 1) = data(size - 1, 0);
  Curve bisector(diagonal, "Test line");
  bisector.setColor("red");
  bisector.setLineStyle("dashed");
  graphQQplot.add(bisector);
  // Then the QQ plot
  graphQQplot.add(cloudQQplot);
  return graphQQplot;
}

/* Draw the PPplot of the two Samples when its dimension is 1 */
Graph VisualTest::DrawPPplot(const Sample & sample1,
                             const Sample & sample2,
                             const UnsignedInteger pointNumber)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a PPplot only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a PPplot only if dimension equals 1, here dimension=" << sample2.getDimension();
  Sample data(pointNumber, 2);
  const Scalar step = (sample1.getMax()[0] - sample1.getMin()[0]) / (pointNumber + 1.0);
  for (UnsignedInteger i = 0; i < pointNumber; ++ i)
  {
    const Scalar p = sample1.getMin()[0] + (i + 0.5) * step;
    data(i, 0) = sample1.computeEmpiricalCDF(Point({p}));
    data(i, 1) = sample2.computeEmpiricalCDF(Point({p}));
  }
  Cloud cloud(data, "Data");
  cloud.setPointStyle(VisualTestGetPointStyle(pointNumber));
  Graph graph("Two sample PP-plot", sample1.getDescription()[0], sample2.getDescription()[0], true, "topleft");
  // First, the bisector
  Sample diagonal(2, 2);
  diagonal(0, 0) = data(0, 0);
  diagonal(0, 1) = data(0, 0);
  diagonal(1, 0) = data(pointNumber - 1, 0);
  diagonal(1, 1) = data(pointNumber - 1, 0);
  Curve bisector(diagonal, "Test line");
  bisector.setColor("red");
  bisector.setLineStyle("dashed");
  graph.add(bisector);
  // Then the PP plot
  graph.add(cloud);
  return graph;
}

/* Draw the PPplot of one Sample and one Distribution when its dimension is 1 */
Graph VisualTest::DrawPPplot(const Sample & sample,
                             const Distribution & dist)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a PPplot only if dimension equals 1, here dimension=" << sample.getDimension();
  if (dist.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a PPplot only if dimension equals 1, here dimension=" << dist.getDimension();
  const Sample sortedSample(sample.sortUnique());
  if (!(sortedSample.getSize() >= 2)) throw InvalidArgumentException(HERE) << "Sample must have at least 2 distinct points";
  const UnsignedInteger size = sortedSample.getSize() - 1;// avoids last point with p=1
  Sample data(size, 2);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Scalar p = sample.computeEmpiricalCDF(sortedSample[i]);
    data(i, 0) = p;
    data(i, 1) = dist.computeCDF(sortedSample[i]);
  }
  Cloud cloud(data, "Data");
  cloud.setPointStyle(VisualTestGetPointStyle(size));
  Graph graph("Sample versus model PP-plot", sample.getDescription()[0], dist.__str__(), true, "topleft");
  // First, the bisector
  Sample diagonal(2, 2);
  diagonal(0, 0) = data(0, 0);
  diagonal(0, 1) = data(0, 0);
  diagonal(1, 0) = data(size - 1, 0);
  diagonal(1, 1) = data(size - 1, 0);
  Curve bisector(diagonal, "Test line");
  bisector.setColor("red");
  bisector.setLineStyle("dashed");
  graph.add(bisector);
  // Then the PP plot
  graph.add(cloud);
  return graph;
}

/* Draw the CDFplot of the two Samples when its dimension is 1 */
Graph VisualTest::DrawCDFplot(const Sample & sample1,
                              const Sample & sample2)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a CDFplot only if dimension equals 1, here dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a CDFplot only if dimension equals 1, here dimension=" << sample2.getDimension();
  const Sample sortedSample(sample1.sort(0));
  const UnsignedInteger pointNumber(sample1.getSize());
  const Sample data1(RegularGrid(0.5 / pointNumber, 1.0 / pointNumber, pointNumber).getVertices());
  const Sample data2(UserDefined(sample2).computeCDF(sortedSample));
  Cloud cloudCDFplot(data1, data2, "Data");
  cloudCDFplot.setPointStyle(VisualTestGetPointStyle(pointNumber));
  Graph graphCDFplot("Two sample CDF-plot", sample1.getDescription()[0], sample2.getDescription()[0], true, "topleft");
  // First, the bisector
  Sample diagonal(2, 2);
  diagonal(1, 0) = 1.0;
  diagonal(1, 1) = 1.0;
  Curve bisector(diagonal, "Test line");
  bisector.setColor("red");
  bisector.setLineStyle("dashed");
  graphCDFplot.add(bisector);
  // Then the CDF plot
  graphCDFplot.add(cloudCDFplot);
  return graphCDFplot;
}

/* Draw the CDFplot of one Sample and one Distribution when its dimension is 1 */
Graph VisualTest::DrawCDFplot(const Sample & sample,
                              const Distribution & dist)
{
  if (sample.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a CDFplot only if dimension equals 1, here dimension=" << sample.getDimension();
  if (dist.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a CDFplot only if dimension equals 1, here dimension=" << dist.getDimension();
  const Sample sortedSample(sample.sort(0));
  const UnsignedInteger pointNumber(sample.getSize());
  const Sample data1(RegularGrid(0.5 / pointNumber, 1.0 / pointNumber, pointNumber).getVertices());
  const Sample data2(dist.computeCDF(sortedSample));
  Cloud cloudCDFplot(data1, data2, "Data");
  cloudCDFplot.setPointStyle(VisualTestGetPointStyle(pointNumber));
  Graph graphCDFplot("Sample versus model CDF-plot", sample.getDescription()[0], dist.__str__(), true, "topleft");
  // First, the bisector
  Sample diagonal(2, 2);
  diagonal(1, 0) = 1.0;
  diagonal(1, 1) = 1.0;
  Curve bisector(diagonal, "Test line");
  bisector.setColor("red");
  bisector.setLineStyle("dashed");
  graphCDFplot.add(bisector);
  // Then the CDF plot
  graphCDFplot.add(cloudCDFplot);
  return graphCDFplot;
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
  Sample data(size, 2);
  const Scalar step = 1.0 / size;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data(i, 0) = sortedSample(i, 0);
    data(i, 1) = DistFunc::qNormal((i + 0.5) * step);
  }
  Cloud dataCloud(data, "Data");
  graphHenry.add(dataCloud);

  return graphHenry;
}


/* Draw 2-d projections of a multivariate sample */
GridLayout VisualTest::DrawPairs(const Sample & sample)
{
  const UnsignedInteger dimension = sample.getDimension();
  if (dimension < 2)
    throw InvalidDimensionException(HERE) << "Can only draw clouds from a multivariate sample";
  GridLayout grid(dimension - 1, dimension - 1);
  const Description description(sample.getDescription());
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      const Indices indices = {j, i};
      const Cloud cloud(sample.getMarginal(indices), ResourceMap::GetAsString("Drawable-DefaultColor"), ResourceMap::GetAsString("Drawable-DefaultPointStyle"));
      Graph graph("", i == dimension - 1 ? description[j] : "", j == 0 ? description[i] : "", true, "topright");
      graph.add(cloud);
      int location = GraphImplementation::TICKNONE;
      if (i == dimension - 1)
        location |= GraphImplementation::TICKX;
      if (j == 0)
        location |= GraphImplementation::TICKY;
      graph.setTickLocation(static_cast<GraphImplementation::TickLocation>(location));
      grid.setGraph(i - 1, j, graph);
    }
  }
  return grid;
}


/* Draw 2-d projections of a multivariate sample, plus marginals of a distribution */
GridLayout VisualTest::DrawPairsMarginals(const Sample & sample, const Distribution & distribution)
{
  const UnsignedInteger dimension = sample.getDimension();
  if (dimension < 2)
    throw InvalidDimensionException(HERE) << "Can only draw clouds from a multivariate sample";
  if (distribution.getDimension() != dimension)
    throw InvalidDimensionException(HERE) << "Distribution dimension does not match the sample dimension";
  GridLayout grid(dimension, dimension);
  const Description description(sample.getDescription());
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    Graph pdfGraph(distribution.getMarginal(i).drawPDF());
    pdfGraph.setLegends(Description(1));
    pdfGraph.setYTitle(i == 0 ? sample.getDescription()[i] : "");
    pdfGraph.setXTitle(i == dimension - 1 ? sample.getDescription()[i] : "");
    grid.setGraph(i, i, pdfGraph);
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      const Indices indices = {j, i};
      const Cloud cloud(sample.getMarginal(indices), "blue", "fsquare", "");
      Graph graph("", i == dimension - 1 ? description[j] : "", j == 0 ? description[i] : "", true, "topright");
      graph.add(cloud);
      grid.setGraph(i, j, graph);
    }
  }
  return grid;
}


/* Draw the visual test for a 1D LinearModel */
Graph VisualTest::DrawLinearModel(const Sample & sample1, const Sample & sample2, const LinearModelResult & linearModelResult)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if both input and output dimension equal 1, here input dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if both input and output dimension equal 1, here output dimension=" << sample2.getDimension();
  if (sample1.getSize() != sample2.getSize()) throw InvalidArgumentException(HERE) << "Error: can draw a LinearModel visual test only if sample 1 and sample 2 have the same size, here sample 1 size=" << sample1.getSize() << " and sample 2 size=" << sample2.getSize();

  const Function fHat(linearModelResult.getMetaModel());
  const Sample y(fHat(sample1));

  const UnsignedInteger size = sample1.getSize();
  Sample sample2D(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point point = {sample1(i, 0), y(i, 0)};
    sample2D[i] = point;
  }
  Curve curveLinearModelTest(sample2D.sortAccordingToAComponent(0));
  curveLinearModelTest.setLegend("regression");
  curveLinearModelTest.setColor("red");
  Cloud cloudLinearModelTest(sample1, sample2);
  cloudLinearModelTest.setPointStyle("fsquare");
  cloudLinearModelTest.setLegend("sample");

  Graph graphLinearModelTest("Linear model visual test", sample1.getDescription()[0], sample2.getDescription()[0], true, "topright");
  graphLinearModelTest.add(cloudLinearModelTest);
  graphLinearModelTest.add(curveLinearModelTest);
  return graphLinearModelTest;
}

/* Draw the visual test for a 1D LinearModel using the training Samples **/
Graph VisualTest::DrawLinearModel(const LinearModelResult & linearModelResult)
{
  const Sample & sample1 = linearModelResult.getInputSample();
  const Sample & sample2 = linearModelResult.getOutputSample();
  return VisualTest::DrawLinearModel(sample1, sample2, linearModelResult);
}

/* Draw the visual test for a 1D LinearModel's residuals */
Graph VisualTest::DrawLinearModelResidual(const Sample & sample1, const Sample & sample2, const LinearModelResult & linearModelResult)
{
  if (sample1.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if both input and output dimension equal 1, here input dimension=" << sample1.getDimension();
  if (sample2.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: can draw a LinearModel residual visual test only if both input and output dimension equal 1, here output dimension=" << sample2.getDimension();
  if (sample1.getSize() != sample2.getSize()) throw InvalidArgumentException(HERE) << "Error: can draw a LinearModel residual visual test only if sample 1 and sample 2 have the same size, here sample 1 size=" << sample1.getSize() << " and sample 2 size=" << sample2.getSize();

  const Function fHat(linearModelResult.getMetaModel());
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
  oss << sample1.getDescription()[0] << " LinearModel residual Test";
  const Cloud cloudLinearModelRTest(data, "red", "fsquare", oss);

  Graph graphLinearModelRTest("residual(i) versus residual(i-1)", "residual(i-1)", "residual(i)", true, "topright");
  graphLinearModelRTest.add(cloudLinearModelRTest);
  return graphLinearModelRTest;
}

/* Draw the visual test for a 1D LinearModel's residuals using the training Samples */
Graph VisualTest::DrawLinearModelResidual(const LinearModelResult & linearModelResult)
{
  const Sample sample1(linearModelResult.getInputSample());
  const Sample sample2(linearModelResult.getOutputSample());
  return VisualTest::DrawLinearModelResidual(sample1, sample2, linearModelResult);
}

/* Draw the parallel coordinates visual test */
Graph VisualTest::DrawParallelCoordinates(const Sample & inputSample,
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
  Graph cobWeb(String(OSS() << "Parallel coordinates - " << outputSample.getDescription() << " vs " << inputSample.getDescription()), "", "", false, "topright");
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

/* Compute the Kendall plot empirical statistic associated with a bidimensional sample */
static Sample ComputeKendallPlotEmpiricalStatistics(const Sample & sample)
{
  const UnsignedInteger size = sample.getSize();
  Sample result(size, 1);
  const Scalar scalarSize = static_cast<Scalar>(size - 1);
  /* To ease reading and implementation, we have a full double
     loop for i & j variables from 0 to size
     It is also possible to reduce costs by a factor 2 by completing
     sequentially (not practice for maintenance and similar performance as current
     implementation)

     for (UnsignedInteger i = 0; i < size; ++i)
     {
       const Scalar uI = sample(i, 0);
        const Scalar vI = sample(i, 1);
       UnsignedInteger cardinalIJ = 0;
       for (UnsignedInteger j = 0; j < i; ++j)
       {
         const Scalar uJ = sample(j, 0);
         const Scalar vJ = sample(j, 1);
         cardinalIJ = (uJ <= uI) && (vJ <= vI);
         result(i, 0) += cardinalIJ;
         cardinalIJ = (uI <= uJ) && (vI <= vJ);
         result(j, 0) += cardinalIJ;
       }
     }
     for (UnsignedInteger i = 0; i < size; ++i) result(i, 0) /= scalarSize;
  */
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar uI = sample(i, 0);
    const Scalar vI = sample(i, 1);
    UnsignedInteger cardinal = 0;
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      const Scalar uJ = sample(j, 0);
      const Scalar vJ = sample(j, 1);
      cardinal += (uJ <= uI) && (vJ <= vI);
    }
    for (UnsignedInteger j = i + 1; j < size; ++j)
    {
      const Scalar uJ = sample(j, 0);
      const Scalar vJ = sample(j, 1);
      cardinal += (uJ <= uI) && (vJ <= vI);
    }
    result(i, 0) = cardinal / scalarSize;
  }

  return result.sort(0);
}

/* Compute the Kendall plot theoretical statistic associated with a bidimensional copula */
static Sample ComputeKendallPlotTheoreticalStatistics(const Distribution & copula,
    const UnsignedInteger size)
{
  if (!copula.isCopula()) throw InvalidArgumentException(HERE) << "Error: the given distribution=" << copula << " is not a copula.";
  const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger( "VisualTest-KendallPlot-MonteCarloSize" );
  Sample result(size, 1);
  for (UnsignedInteger i = 0; i < maximumIteration; ++i)
  {
    const Sample empiricalStatistics(ComputeKendallPlotEmpiricalStatistics(copula.getSample(size)));
    for (UnsignedInteger j = 0; j < size; ++j) result(j, 0) = (result(j, 0) * i + empiricalStatistics(j, 0)) / (i + 1);
  }
  return result;
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

/* Draw dependence functions */
static Graph VisualTestDrawDependenceFunction(const Sample & data,
                                       const String & linkFormula,
                                       const String & legend,
                                       const Bool survival = false)
{
  if (!data.getSize())
    throw InvalidArgumentException(HERE) << "The sample must not be empty";
  if (data.getDimension() != 2)
    throw InvalidArgumentException(HERE) << "The sample must be of dimension 2";
  const UnsignedInteger size = data.getSize();
  const Sample ranked((data.rank() + 1.0) / size);
  Graph graph("n/a", "u", legend, true, "bottom");
  const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber");
  Sample valuesU(pointNumber, 1);
  Sample valuesXu(pointNumber, 1);
  Sample valuesXuLow(pointNumber, 1);
  Sample valuesXuUp(pointNumber, 1);
  const Scalar level = ResourceMap::GetAsScalar("VisualTest-DependenceConfidenceLevel");
  const Scalar xq = DistFunc::qNormal(0.5 + 0.5 * level);
  const SymbolicFunction link(Description({"u", "cuu"}), Description(1, linkFormula));
  for (UnsignedInteger i = 0; i < pointNumber; ++ i)
  {
    const Scalar u = (i + 1) * 1.0 / (pointNumber + 1);
    valuesU(i, 0) = u;
    Scalar cuu = 0.0;
    for (UnsignedInteger j = 0; j < size; ++ j)
    {
      if (survival)
      {
        if (u < std::min(ranked(j, 0), ranked(j, 1)))
          cuu += 1.0 / size;
      }
      else
      {
        if (u > std::max(ranked(j, 0), ranked(j, 1)))
          cuu += 1.0 / size;
      }
    }
    if ((cuu > 0.0) && (cuu < 1.0))
    {
      // estimate
      valuesXu[i] = link(Point({u, cuu}));

      // confidence interval of cuu, then xu
      Scalar lb = cuu - xq * std::sqrt((cuu * (1.0 - cuu)) / size);
      // might be < 0, set something positive below cuu
      const Scalar epsilon = SpecFunc::Precision;
      if (lb < 0.0)
        lb = epsilon;
      Scalar ub = cuu + xq * std::sqrt((cuu * (1.0 - cuu)) / size);
      if (ub > 1.0)
        ub = 1.0 - epsilon;
      valuesXuLow[i] = link(Point({u, lb}));
      valuesXuUp [i] = link(Point({u, ub}));

      // clip CI in [-1,1]
      valuesXuLow(i, 0) = std::max(valuesXuLow(i, 0), -1.0);
      valuesXuUp(i, 0) = std::min(valuesXuUp(i, 0), 1.0);
    }
  }

  // estimate
  Curve curveXu(valuesU, valuesXu);
  curveXu.setColor("red");
  curveXu.setLegend(legend);
  graph.add(curveXu);
  // confidence lower bound
  Curve curveXuLow(valuesU, valuesXuLow);
  curveXuLow.setColor("blue");
  curveXuLow.setLineStyle("dashed");
  curveXuLow.setLegend("CI low");
  graph.add(curveXuLow);
  // confidence upper bound
  Curve curveXuUp(valuesU, valuesXuUp);
  curveXuUp.setColor("blue");
  curveXuUp.setLineStyle("dashed");
  curveXuUp.setLegend("CI up");
  graph.add(curveXuUp);
  return graph;
}

Graph VisualTest::DrawUpperTailDependenceFunction(const Sample & data)
{
  Graph result(VisualTestDrawDependenceFunction(data, "2-log(cuu)/log(u)", "$\\chi(u)$"));
  result.setTitle("Upper tail dependence function");
  return result;
}

Graph VisualTest::DrawUpperExtremalDependenceFunction(const Sample & data)
{
  Graph result(VisualTestDrawDependenceFunction(data, "2*log1p(-u)/log(cuu)-1", "$\\bar{\\chi}(u)$", true));
  result.setTitle("Upper extremal dependence function");
  return result;
}

Graph VisualTest::DrawLowerTailDependenceFunction(const Sample & data)
{
  Graph result(VisualTestDrawDependenceFunction(data, "log1p(-cuu)/log1p(-u)", "$\\chi_L(u)$"));
  result.setTitle("Lower tail dependence function");
  return result;
}

Graph VisualTest::DrawLowerExtremalDependenceFunction(const Sample & data)
{
  Graph result(VisualTestDrawDependenceFunction(data, "2*log(u)/log(cuu)-1", "$\\bar{\\chi}_L(u)$"));
  result.setTitle("Lower extremal dependence function");
  return result;
}

END_NAMESPACE_OPENTURNS
