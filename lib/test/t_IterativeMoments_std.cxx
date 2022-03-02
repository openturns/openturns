//                                               -*- C++ -*-
/**
 *  @brief The test file of class IterativeMoments for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {
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
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
