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

// Check if bool is true
void assertTrue(bool condition)
{
  if (!condition) throw TestFailed("Boolean is not true");
}

// Check if bool is false
void assertFalse(bool condition)
{
  if (condition) throw TestFailed("Boolean is not false");
}

// Check if two Scalar are equal
void assertEqual(Scalar value1, Scalar value2, Scalar epsilon = 0.0)
{
  if (abs(value1 - value2) > epsilon)
  {
    throw TestFailed(OSS() << "Value " << value1 << " is not equal to " << value2);
  }
}

// Check if two int are equal
void assertEqual(UnsignedInteger value1, UnsignedInteger value2)
{
  std::cout<<"Checking int. value1="<<value1<<", value2="<<value2<<std::endl;
  if (value1 != value2)
  {
    throw TestFailed(OSS() << "Value " << value1 << " is not equal to " << value2);
  }
}

// Check if two Point are equal
void assertEqual(Point value1, Point value2, Scalar epsilon = 0.0)
{
  UnsignedInteger dim1 = value1.getDimension();
  UnsignedInteger dim2 = value2.getDimension();
  assertEqual(dim1, dim2);
  for ( UnsignedInteger index = 0; index < dim1; ++ index)
  {
    std::cout<<"Checking Point. index="<<index<<", value1="<<value1[index]<<", value2="<<value2[index]<<std::endl;
    if (abs(value1[index] - value2[index]) > epsilon)
    {
      throw TestFailed(OSS() << "Value " << value1[index] \
                       << " is not equal to " << value2[index] \
                       << " at index=" << index);
    }
  }
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // We create three points
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

    // We create a Sample
    Sample sample1(0, 2);
    sample1.add(point1);
    sample1.add(point2);
    sample1.add(point3);
    fullprint << "sample1=" << sample1 << std::endl;
    Point referencethreshold(dimension);
    referencethreshold[0] = 0.;
    referencethreshold[1] = 3.;
    fullprint << "referencethreshold=" << referencethreshold << std::endl;

    // Iterative threshold, one point at a time
    fullprint << "Iterative threshold, one point at a time" << std::endl;
    IterativeThresholdExceedance iterthresholdPoint(dimension, 15.0);
    iterthresholdPoint.increment(point1);
    iterthresholdPoint.increment(point2);
    iterthresholdPoint.increment(point3);
    Point computedthreshold = iterthresholdPoint.getThresholdExceedance();
    assertEqual(referencethreshold, computedthreshold);
    UnsignedInteger iteration = iterthresholdPoint.getIteration();
    assertEqual(iteration, 3);

    // Iterative threshold, one single sample
    fullprint << "Iterative threshold, one single sample" << std::endl;
    IterativeThresholdExceedance iterthresholdSample(dimension, 15.0);
    iterthresholdSample.increment(sample1);
    computedthreshold = iterthresholdSample.getThresholdExceedance();
    assertEqual(referencethreshold, computedthreshold);
    iteration = iterthresholdSample.getIteration();
    assertEqual(iteration, 3);

    // Iterative threshold, one single sample, then one point at a time
    fullprint << "Iterative threshold, one single sample, then one point at a time" << std::endl;
    IterativeThresholdExceedance iterthresholdMixed(dimension, 15.0);
    iterthresholdMixed.increment(sample1);
    iterthresholdMixed.increment(point1);
    iterthresholdMixed.increment(point2);
    iterthresholdMixed.increment(point3);
    computedthreshold = iterthresholdMixed.getThresholdExceedance();
    referencethreshold *= 2;
    assertEqual(referencethreshold, computedthreshold);
    iteration = iterthresholdMixed.getIteration();
    assertEqual(iteration, 6);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
