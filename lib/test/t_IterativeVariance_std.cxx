//                                               -*- C++ -*-
/**
 *  @brief The test file of class IterativeVariance for standard methods
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
    // We mix the Sample ans the Points
    Sample mixedsample = sample1;
    mixedsample.add(point1);
    mixedsample.add(point2);
    mixedsample.add(point3);
    fullprint << "sample1=" << sample1 << std::endl;
    Point referencemean = sample1.computeMean();
    fullprint << "referencemean=" << referencemean << std::endl;
    Point referencevariance = sample1.computeVariance();
    fullprint << "referencevariance=" << referencevariance << std::endl;
    Point referencemixedmean = mixedsample.computeMean();
    fullprint << "referencemixedmean=" << referencemixedmean << std::endl;
    Point referencemixedvariance = mixedsample.computeVariance();
    fullprint << "referencemixedvariance=" << referencemixedvariance << std::endl;

    // Iterative variance, one point at a time
    fullprint << "Iterative variance, one point at a time" << std::endl;
    IterativeVariance itervariancePoint(dimension);
    itervariancePoint.increment(point1);
    itervariancePoint.increment(point2);
    itervariancePoint.increment(point3);
    Point computedmean = itervariancePoint.getMean();
    assertEqual(referencemean, computedmean);
    Point computedvariance = itervariancePoint.getVariance();
    assertEqual(referencevariance, computedvariance);
    UnsignedInteger iteration = itervariancePoint.getIteration();
    assertEqual(iteration, 3);

    // Iterative variance, one single sample
    fullprint << "Iterative variance, one single sample" << std::endl;
    IterativeVariance itervarianceSample(dimension);
    itervarianceSample.increment(sample1);
    computedmean = itervarianceSample.getMean();
    assertEqual(referencemean, computedmean);
    computedvariance = itervarianceSample.getVariance();
    assertEqual(referencevariance, computedvariance);
    iteration = itervarianceSample.getIteration();
    assertEqual(iteration, 3);

    // Iterative variance, one single sample, then one point at a time
    fullprint << "Iterative variance, one single sample, then one point at a time" << std::endl;
    IterativeVariance itervarianceMixed(dimension);
    itervarianceMixed.increment(sample1);
    itervarianceMixed.increment(point1);
    itervarianceMixed.increment(point2);
    itervarianceMixed.increment(point3);
    computedmean = itervarianceMixed.getMean();
    assertEqual(referencemixedmean, computedmean);
    computedvariance = itervarianceMixed.getVariance();
    assertEqual(referencemixedvariance, computedvariance);
    iteration = itervarianceMixed.getIteration();
    assertEqual(iteration, 6);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
