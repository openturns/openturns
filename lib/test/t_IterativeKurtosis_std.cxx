//                                               -*- C++ -*-
/**
 *  @brief The test file of class IterativeKurtosis for standard methods
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
void assertEqual(int value1, int value2)
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
  int dim1 = value1.getDimension();
  int dim2 = value2.getDimension();
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
    Point point4(dimension);
    point4[0] = 15.;
    point4[1] = 25.;

    // We create a Sample
    Sample sample1(0, 2);
    sample1.add(point1);
    sample1.add(point2);
    sample1.add(point3);
    sample1.add(point4);
    // We mix the Sample ans the Points
    Sample mixedsample = sample1;
    mixedsample.add(point1);
    mixedsample.add(point2);
    mixedsample.add(point3);
    mixedsample.add(point4);
    fullprint << "sample1=" << sample1 << std::endl;
    Point referencemean = sample1.computeMean();
    fullprint << "referencemean=" << referencemean << std::endl;
    Point referencevariance = sample1.computeVariance();
    fullprint << "referencevariance=" << referencevariance << std::endl;
    Point referenceskewness = sample1.computeSkewness();
    fullprint << "referenceskewness=" << referenceskewness << std::endl;
    Point referencekurtosis = sample1.computeKurtosis();
    fullprint << "referencekurtosis=" << referencekurtosis << std::endl;
    Point referencemixedmean = mixedsample.computeMean();
    fullprint << "referencemixedmean=" << referencemixedmean << std::endl;
    Point referencemixedvariance = mixedsample.computeVariance();
    fullprint << "referencemixedvariance=" << referencemixedvariance << std::endl;
    Point referencemixedskewness = mixedsample.computeSkewness();
    fullprint << "referencemixedskewness=" << referencemixedskewness << std::endl;
    Point referencemixedkurtosis = mixedsample.computeKurtosis();
    fullprint << "referencemixedkurtosis=" << referencemixedkurtosis << std::endl;

    // Iterative kurtosis, one point at a time
    fullprint << "Iterative kurtosis, one point at a time" << std::endl;
    IterativeKurtosis iterkurtosisPoint(dimension);
    iterkurtosisPoint.increment(point1);
    iterkurtosisPoint.increment(point2);
    iterkurtosisPoint.increment(point3);
    iterkurtosisPoint.increment(point4);
    Point computedmean = iterkurtosisPoint.getMean();
    assertEqual(referencemean, computedmean, 1.0e-12);
    Point computedvariance = iterkurtosisPoint.getVariance();
    assertEqual(referencevariance, computedvariance, 1.0e-12);
    Point computedskewness = iterkurtosisPoint.getSkewness();
    Scalar size = sample1.getSize();
    Scalar factor1 = sqrt(size) * sqrt(size - 1.0) / (size - 2.0);
    assertEqual(referenceskewness, computedskewness * factor1, 1.0e-12);
    Point computedkurtosis = iterkurtosisPoint.getKurtosis();
//     factor1 = (size + 1.0) * size * (size - 1.0) / ((size - 2.0) * (size - 3.0));
//     Scalar factor2 = -3.0 * (3.0 * size - 5.0) / ((size - 2.0) * (size - 3.0));
//     computedkurtosis[0] += 3;
//     computedkurtosis[1] += 3;
//     computedkurtosis /= size*size;
//     computedkurtosis *= factor1;
//     computedkurtosis[0] += factor2;
//     computedkurtosis[1] += factor2;
//     assertEqual(referencekurtosis, computedkurtosis, 1.0e-12);
    UnsignedInteger iteration = iterkurtosisPoint.getIteration();
    assertEqual(iteration, 4);

    // Iterative kurtosis, one single sample
    fullprint << "Iterative kurtosis, one single sample" << std::endl;
    IterativeKurtosis iterkurtosisSample(dimension);
    iterkurtosisSample.increment(sample1);
    computedmean = iterkurtosisSample.getMean();
    assertEqual(referencemean, computedmean, 1.0e-12);
    computedvariance = iterkurtosisSample.getVariance();
    assertEqual(referencevariance, computedvariance, 1.0e-12);
    computedskewness = iterkurtosisSample.getSkewness();
    assertEqual(referenceskewness, computedskewness * factor1, 1.0e-12);
    computedkurtosis = iterkurtosisSample.getKurtosis();
//     assertEqual(referencekurtosis, computedkurtosis, 1.0e-12);
    iteration = iterkurtosisSample.getIteration();
    assertEqual(iteration, 4);

    // Iterative kurtosis, one single sample, then one point at a time
    fullprint << "Iterative kurtosis, one single sample, then one point at a time" << std::endl;
    IterativeKurtosis iterkurtosisMixed(dimension);
    iterkurtosisMixed.increment(sample1);
    iterkurtosisMixed.increment(point1);
    iterkurtosisMixed.increment(point2);
    iterkurtosisMixed.increment(point3);
    iterkurtosisMixed.increment(point4);
    computedmean = iterkurtosisMixed.getMean();
    assertEqual(referencemixedmean, computedmean, 1.0e-12);
    computedvariance = iterkurtosisMixed.getVariance();
    assertEqual(referencemixedvariance, computedvariance, 1.0e-12);
    computedskewness = iterkurtosisMixed.getSkewness();
    size = sample1.getSize() * 2;
    factor1 = sqrt(size) * sqrt(size - 1.0) / (size - 2.0);
    assertEqual(referencemixedskewness, computedskewness * factor1, 1.0e-12);
    computedkurtosis = iterkurtosisMixed.getKurtosis();
//     assertEqual(referencemixedkurtosis, computedkurtosis, 1.0e-12);
    iteration = iterkurtosisMixed.getIteration();
    assertEqual(iteration, 8);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
