//                                               -*- C++ -*-
/**
 *  @brief The test file of class IterativeExtrema for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // We create three points
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

    // We create a Sample
    Sample sample1(0, 2);
    sample1.add(point1);
    sample1.add(point2);
    sample1.add(point3);
    Point referencemin = point1;
    Point referencemax = point3;

    // Iterative extrema, one point at a time
    IterativeExtrema iterextremaPoint(dimension);
    iterextremaPoint.increment(point1);
    iterextremaPoint.increment(point2);
    iterextremaPoint.increment(point3);
    Point computedmin = iterextremaPoint.getMin();
    assert_equal(referencemin, computedmin);
    Point computedmax = iterextremaPoint.getMax();
    assert_equal(referencemax, computedmax);
    UnsignedInteger iteration = iterextremaPoint.getIterationNumber();
    const UnsignedInteger expectedIteration1 = 3;
    assert_equal(iteration, expectedIteration1);

    // Iterative extrema, one single sample
    IterativeExtrema iterextremaSample(dimension);
    iterextremaSample.increment(sample1);
    computedmin = iterextremaSample.getMin();
    assert_equal(referencemin, computedmin);
    computedmax = iterextremaSample.getMax();
    assert_equal(referencemax, computedmax);
    iteration = iterextremaSample.getIterationNumber();
    const UnsignedInteger expectedIteration2 = 3;
    assert_equal(iteration, expectedIteration2);

    // Iterative extrema, one single sample, then one point at a time
    IterativeExtrema iterextremaMixed(dimension);
    iterextremaMixed.increment(sample1);
    iterextremaMixed.increment(point1);
    iterextremaMixed.increment(point2);
    iterextremaMixed.increment(point3);
    computedmin = iterextremaMixed.getMin();
    assert_equal(referencemin, computedmin);
    computedmax = iterextremaMixed.getMax();
    assert_equal(referencemax, computedmax);
    iteration = iterextremaMixed.getIterationNumber();
    const UnsignedInteger expectedIteration3 = 6;
    assert_equal(iteration, expectedIteration3);

    /* check whether objects are the same */
    assert_equal(iterextremaSample, iterextremaPoint);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
