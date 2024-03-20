//                                               -*- C++ -*-
/**
 *  @brief The test file of class IterativeMoments for standard methods
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

void test_MainFeatures()
{
  OStream fullprint(std::cout);
  fullprint << "test_MainFeatures" << std::endl;
  /* We create four points in dimension 2*/
  const UnsignedInteger dimension = 2;
  Point point1(dimension);
  point1[0] = 10.0;
  point1[1] = 20.0;
  Point point2(dimension);
  point2[0] = 11.0;
  point2[1] = 21.0;
  Point point3(dimension);
  point3[0] = 12.0;
  point3[1] = 22.0;
  Point point4(dimension);
  point4[0] = 15.0;
  point4[1] = 25.0;

  /* We create a Sample */
  Sample sample1(0, 2);
  sample1.add(point1);
  sample1.add(point2);
  sample1.add(point3);
  sample1.add(point4);
  Point referenceMean = sample1.computeMean();
  Point referenceVariance = sample1.computeVariance();
  Point referenceSkewness = sample1.computeSkewness();
  Point referenceKurtosis = sample1.computeKurtosis();

  /* Iterative kurtosis, one point at a time */
  IterativeMoments iterMoments(4, dimension);
  iterMoments.increment(point1);
  iterMoments.increment(point2);
  iterMoments.increment(point3);
  iterMoments.increment(point4);
  const Scalar rtol = 1.e-14;
  assert_almost_equal(iterMoments.getMean(), referenceMean, rtol);
  assert_almost_equal(iterMoments.getVariance(), referenceVariance, rtol);
  assert_almost_equal(iterMoments.getSkewness(), referenceSkewness, rtol);
  assert_almost_equal(iterMoments.getKurtosis(), referenceKurtosis, rtol);

  /* Iterative kurtosis, with one sample */
  IterativeMoments iterMoments2(4, dimension);
  iterMoments2.increment(sample1);
  assert_almost_equal(iterMoments2.getMean(), referenceMean);
  assert_almost_equal(iterMoments2.getVariance(), referenceVariance, rtol);
  assert_almost_equal(iterMoments2.getSkewness(), referenceSkewness, rtol);
  assert_almost_equal(iterMoments2.getKurtosis(), referenceKurtosis, rtol);

  /* We mix the Sample and the Points */
  Sample mixedSample = sample1;
  mixedSample.add(point1);
  mixedSample.add(point2);
  mixedSample.add(point3);
  mixedSample.add(point4);
  Point referenceMixedMean = mixedSample.computeMean();
  Point referenceMixedVariance = mixedSample.computeVariance();
  Point referenceMixedSkewness = mixedSample.computeSkewness();
  Point referenceMixedKurtosis = mixedSample.computeKurtosis();

  /* Here we declare an iterative object of maximum order 3 */
  IterativeMoments iterMoments3(3, dimension);
  iterMoments3.increment(mixedSample);
  const UnsignedInteger expectedIteration = 8;
  assert_equal(iterMoments3.getIterationNumber(), expectedIteration);
  const UnsignedInteger expectedDimension = 2;
  assert_equal(iterMoments3.getDimension(), expectedDimension);
  const UnsignedInteger expectedOrder = 3;
  assert_equal(iterMoments3.getOrder(), expectedOrder);

  /* test the moments */
  assert_almost_equal(iterMoments3.getMean(), referenceMixedMean, rtol);
  assert_almost_equal(iterMoments3.getVariance(), referenceMixedVariance, rtol);
  assert_almost_equal(iterMoments3.getSkewness(), referenceMixedSkewness, rtol);

  /* check whether objects are different */
  assert_equal(iterMoments2 != iterMoments3, true);
}

void test_SampleSize1()
{
  OStream fullprint(std::cout);
  fullprint << "test_SampleSize1" << std::endl;
  Log::Show(Log::ALL);
  /* We create one point in dimension 2*/
  const UnsignedInteger dimension = 2;
  Point point1(dimension);
  point1[0] = 10.0;
  point1[1] = 20.0;

  /* We create a Sample with 1 point : the mean and variance must work */
  Sample sample1(0, 2);
  sample1.add(point1);
  Point referenceMean = sample1.computeMean();
  Point referenceVariance = sample1.computeVariance();

  /* Iterative mean and variance, one point at a time */
  IterativeMoments iterMoments(2, dimension);
  iterMoments.increment(point1);
  const Scalar rtol = 1.e-14;
  assert_almost_equal(iterMoments.getMean(), referenceMean, rtol);
  assert_almost_equal(iterMoments.getVariance(), referenceVariance, rtol);
}

void test_SampleSize2()
{
  OStream fullprint(std::cout);
  fullprint << "test_SampleSize2" << std::endl;
  Log::Show(Log::ALL);
  /* We create one point in dimension 2*/
  const UnsignedInteger dimension = 2;
  Point point1(dimension);
  point1[0] = 10.0;
  point1[1] = 20.0;
  Point point2(dimension);
  point2[0] = 11.0;
  point2[1] = 21.0;

  /* We create a Sample with 2 points : the mean, variance and skewness must work */
  Sample sample1(0, 2);
  sample1.add(point1);
  sample1.add(point2);
  Point referenceMean = sample1.computeMean();
  Point referenceVariance = sample1.computeVariance();
  Point referenceSkewness = sample1.computeSkewness();

  /* Iterative mean, variance and skewness, one point at a time */
  IterativeMoments iterMoments(3, dimension);
  iterMoments.increment(point1);
  iterMoments.increment(point2);
  const Scalar rtol = 1.e-14;
  assert_almost_equal(iterMoments.getMean(), referenceMean, rtol);
  assert_almost_equal(iterMoments.getVariance(), referenceVariance, rtol);
  assert_almost_equal(iterMoments.getSkewness(), referenceSkewness, rtol);
}

void test_SampleSize3()
{
  OStream fullprint(std::cout);
  fullprint << "test_SampleSize3" << std::endl;
  Log::Show(Log::ALL);
  /* We create one point in dimension 2*/
  const UnsignedInteger dimension = 2;
  Point point1(dimension);
  point1[0] = 10.0;
  point1[1] = 20.0;
  Point point2(dimension);
  point2[0] = 11.0;
  point2[1] = 21.0;
  Point point3(dimension);
  point3[0] = 12.0;
  point3[1] = 22.0;

  /* We create a Sample with 3 points : the mean, variance, skewness and kurtosis must work */
  Sample sample1(0, 2);
  sample1.add(point1);
  sample1.add(point2);
  sample1.add(point3);
  Point referenceMean = sample1.computeMean();
  Point referenceVariance = sample1.computeVariance();
  Point referenceSkewness = sample1.computeSkewness();
  Point referenceKurtosis = sample1.computeKurtosis();

  /* Iterative mean, variance, skewness and kurtosis, one point at a time */
  IterativeMoments iterMoments(4, dimension);
  iterMoments.increment(point1);
  iterMoments.increment(point2);
  iterMoments.increment(point3);
  const Scalar rtol = 1.e-14;
  assert_almost_equal(iterMoments.getMean(), referenceMean, rtol);
  assert_almost_equal(iterMoments.getVariance(), referenceVariance, rtol);
  assert_almost_equal(iterMoments.getSkewness(), referenceSkewness, rtol);
  assert_almost_equal(iterMoments.getKurtosis(), referenceKurtosis, rtol);
}

void test_SampleSize3_with_zero_variance()
{
  OStream fullprint(std::cout);
  fullprint << "test_SampleSize3_with_zero_variance" << std::endl;
  Log::Show(Log::ALL);
  /* We create one point in dimension 2*/
  const UnsignedInteger dimension = 2;
  Point point1(dimension);
  point1[0] = 10.0;
  point1[1] = 20.0;

  /* We create a Sample with 1 point repeated 3 times:
   * the mean, variance must pass, skewness and kurtosis must fail */
  Sample sample1(0, 2);
  sample1.add(point1);
  sample1.add(point1);
  sample1.add(point1);
  Point referenceMean = sample1.computeMean();
  Point referenceVariance = sample1.computeVariance();

  /* Iterative mean, variance and skewness, one point at a time */
  IterativeMoments iterMoments(4, dimension);
  iterMoments.increment(point1);
  iterMoments.increment(point1);
  iterMoments.increment(point1);
  const Scalar rtol = 1.e-14;
  assert_almost_equal(iterMoments.getMean(), referenceMean, rtol);
  assert_almost_equal(iterMoments.getVariance(), referenceVariance, rtol);
  bool isSkewnessExceptionGenerated = false;
  try
  {
    const Point skewness(iterMoments.getSkewness());
    fullprint << "  skewness  = " << skewness << std::endl;
  }
  catch (const NotDefinedException &)
  {
    fullprint << "Expected skewness exception" << std::endl;
    isSkewnessExceptionGenerated = true;
  }
  assert_equal(isSkewnessExceptionGenerated, true);
}

void test_SampleSize4_with_zero_variance()
{
  OStream fullprint(std::cout);
  fullprint << "test_SampleSize4_with_zero_variance" << std::endl;
  Log::Show(Log::ALL);
  /* We create one point in dimension 2*/
  const UnsignedInteger dimension = 2;
  Point point1(dimension);
  point1[0] = 10.0;
  point1[1] = 20.0;

  /* We create a Sample with 1 point repeated 4 times:
   * the mean, variance must pass, skewness and kurtosis must fail */
  Sample sample1(0, 2);
  sample1.add(point1);
  sample1.add(point1);
  sample1.add(point1);
  sample1.add(point1);
  Point referenceMean = sample1.computeMean();
  Point referenceVariance = sample1.computeVariance();

  /* Iterative mean, variance and skewness, one point at a time */
  IterativeMoments iterMoments(4, dimension);
  iterMoments.increment(point1);
  iterMoments.increment(point1);
  iterMoments.increment(point1);
  iterMoments.increment(point1);
  const Scalar rtol = 1.e-14;
  assert_almost_equal(iterMoments.getMean(), referenceMean, rtol);
  assert_almost_equal(iterMoments.getVariance(), referenceVariance, rtol);
  bool isSkewnessExceptionGenerated = false;
  try
  {
    const Point skewness(iterMoments.getSkewness());
    fullprint << "  skewness  = " << skewness << std::endl;
  }
  catch (const NotDefinedException &)
  {
    fullprint << "Expected skewness exception" << std::endl;
    isSkewnessExceptionGenerated = true;
  }
  assert_equal(isSkewnessExceptionGenerated, true);
  bool isKurtosisExceptionGenerated = false;
  try
  {
    const Point kurtosis(iterMoments.getKurtosis());
    fullprint << "  kurtosis  = " << kurtosis << std::endl;
  }
  catch (const NotDefinedException &)
  {
    fullprint << "Expected kurtosis exception" << std::endl;
    isKurtosisExceptionGenerated = true;
  }
  assert_equal(isKurtosisExceptionGenerated, true);
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    test_MainFeatures();
    test_SampleSize1();
    test_SampleSize2();
    test_SampleSize3();
    test_SampleSize3_with_zero_variance();
    test_SampleSize4_with_zero_variance();
  }
  catch (const TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
