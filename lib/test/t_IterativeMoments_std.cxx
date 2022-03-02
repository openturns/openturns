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
  RandomGenerator::SetSeed(0);

  try
  {
    /* We create four points in dimension 2*/
    const UnsignedInteger dimension = 2;
    Point point1(dimension);
    point1[0] = 10.;
    point1[1] = 20.;
    Point point2(dimension);
    point2[0] = 11.;
    point2[1] = 21.;
    Point point3(dimension);
    point3[0] = 12.;
    point3[1] = 22.;
    Point point4(dimension);
    point4[0] = 15.;
    point4[1] = 25.;

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
    assert_almost_equal(iterMoments.getMean(), referenceMean);
    assert_almost_equal(iterMoments.getVariance(), referenceVariance);
    assert_almost_equal(iterMoments.getSkewness(), referenceSkewness);
    assert_almost_equal(iterMoments.getKurtosis(), referenceKurtosis);

    /* Iterative kurtosis, with one sample */
    IterativeMoments iterMoments2(4, dimension);
    iterMoments2.increment(sample1);
    assert_almost_equal(iterMoments2.getMean(), referenceMean);
    assert_almost_equal(iterMoments2.getVariance(), referenceVariance);
    assert_almost_equal(iterMoments2.getSkewness(), referenceSkewness);
    assert_almost_equal(iterMoments2.getKurtosis(), referenceKurtosis);

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
    assert_almost_equal(iterMoments3.getIteration(), 8);
    assert_almost_equal(iterMoments3.getDimension(), 2);
    assert_almost_equal(iterMoments3.getOrder(), 3);

    /* test the moments */
    assert_almost_equal(iterMoments3.getMean(), referenceMixedMean);
    assert_almost_equal(iterMoments3.getVariance(), referenceMixedVariance);
    assert_almost_equal(iterMoments3.getSkewness(), referenceMixedSkewness);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
