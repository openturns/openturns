//                                               -*- C++ -*-
/**
 *  @brief The test file of class IterativeThreshold for standard methods
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
    /* We create three points */
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

    /* We create a Sample */
    Sample sample1(0, 2);
    sample1.add(point1);
    sample1.add(point2);
    sample1.add(point3);
    Point referencethreshold(dimension);
    referencethreshold[0] = 0.;
    referencethreshold[1] = 3.;

    /* Iterative threshold, one point at a time */
    IterativeThresholdExceedance iterthresholdPoint(dimension, 15.0);
    iterthresholdPoint.increment(point1);
    iterthresholdPoint.increment(point2);
    iterthresholdPoint.increment(point3);
    Point computedthreshold = iterthresholdPoint.getThresholdExceedance();
    assert_almost_equal(referencethreshold, computedthreshold);
    UnsignedInteger iteration = iterthresholdPoint.getIteration();
    assert_almost_equal(iteration, 3);

    /* Iterative threshold, one single sample */
    IterativeThresholdExceedance iterthresholdSample(dimension, 15.0);
    iterthresholdSample.increment(sample1);
    computedthreshold = iterthresholdSample.getThresholdExceedance();
    assert_almost_equal(referencethreshold, computedthreshold);
    iteration = iterthresholdSample.getIteration();
    assert_almost_equal(iteration, 3);

    /* Iterative threshold, one single sample, then one point at a time */
    IterativeThresholdExceedance iterthresholdMixed(dimension, 15.0);
    iterthresholdMixed.increment(sample1);
    iterthresholdMixed.increment(point1);
    iterthresholdMixed.increment(point2);
    iterthresholdMixed.increment(point3);
    computedthreshold = iterthresholdMixed.getThresholdExceedance();
    referencethreshold *= 2;
    assert_almost_equal(referencethreshold, computedthreshold);
    iteration = iterthresholdMixed.getIteration();
    assert_almost_equal(iteration, 6);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
